#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/float32.hpp>

#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <tf2_sensor_msgs/tf2_sensor_msgs.hpp>

#include <vector>
#include <cmath>
#include <algorithm>

class DitchDetectorFinal : public rclcpp::Node
{
public:
    DitchDetectorFinal()
    : Node("ditch_detector_final"),
      tf_buffer_(get_clock()),
      tf_listener_(tf_buffer_)
    {
        cloud_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            "/zed/zed_node/point_cloud/cloud_registered",
            rclcpp::SensorDataQoS(),
            std::bind(&DitchDetectorFinal::cloudCb, this, std::placeholders::_1));

        local_grid_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            "/local_grid_obstacle",
            rclcpp::SensorDataQoS(),
            std::bind(&DitchDetectorFinal::gridCb, this, std::placeholders::_1));

        wall_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>("/fake_wall_cloud", 10);
        safe_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>("/local_grid_safe", 10);
        ditch_pub_ = create_publisher<std_msgs::msg::Bool>("/ditch_detected", 10);
        ditch_dist_pub_ = create_publisher<std_msgs::msg::Float32>("/ditch_distance", 10);

        declare_parameter("target_frame", "base_link");
        declare_parameter("xmin", 0.6);
        declare_parameter("xmax", 3.0);
        declare_parameter("bin_size", 0.15);
        declare_parameter("corridor_width", 1.6);
        declare_parameter("min_points", 8);
        declare_parameter("gap_bins", 2);
        declare_parameter("ditch_depth", 0.30);
        declare_parameter("wall_height", 0.8);
        declare_parameter("ground_z", -0.4);
    }

private:
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_sub_, local_grid_sub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr wall_pub_, safe_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr ditch_pub_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr ditch_dist_pub_;

    tf2_ros::Buffer tf_buffer_;
    tf2_ros::TransformListener tf_listener_;

    pcl::PointCloud<pcl::PointXYZ>::Ptr last_wall_{new pcl::PointCloud<pcl::PointXYZ>()};
    pcl::PointCloud<pcl::PointXYZ>::Ptr last_grid_{new pcl::PointCloud<pcl::PointXYZ>()};

    int confirm_{0}, release_{0};
    bool latched_{false};
    int latched_bin_{-1};

    void cloudCb(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        sensor_msgs::msg::PointCloud2 tf_cloud;
        try {
            auto tf = tf_buffer_.lookupTransform(
                get_parameter("target_frame").as_string(),
                msg->header.frame_id,
                tf2::TimePointZero);
            tf2::doTransform(*msg, tf_cloud, tf);
        } catch (...) {
            return;
        }

        pcl::PointCloud<pcl::PointXYZ> cloud;
        pcl::fromROSMsg(tf_cloud, cloud);

        double xmin, xmax, bin, width, ditch_depth;
        int gap_bins;
        int min_pts_i;
        size_t min_pts;

        get_parameter("xmin", xmin);
        get_parameter("xmax", xmax);
        get_parameter("bin_size", bin);
        get_parameter("corridor_width", width);
        get_parameter("min_points", min_pts_i);
        get_parameter("gap_bins", gap_bins);
        get_parameter("ditch_depth", ditch_depth);

        min_pts = static_cast<size_t>(min_pts_i);

        const int bins = static_cast<int>((xmax - xmin) / bin);
        constexpr int lanes = 3;

        std::vector<std::vector<std::vector<float>>> zvals(
            lanes, std::vector<std::vector<float>>(bins));

        for (const auto &p : cloud.points) {
            if (!std::isfinite(p.x) || !std::isfinite(p.z)) continue;
            if (p.x < xmin || p.x > xmax) continue;

            const int lane = static_cast<int>((p.y + width / 2.0) / (width / lanes));
            if (lane < 0 || lane >= lanes) continue;

            const int idx = static_cast<int>((p.x - xmin) / bin);
            if (idx >= 0 && idx < bins)
                zvals[lane][idx].push_back(p.z);
        }

        auto median = [](std::vector<float> &v) -> float {
            if (v.empty()) return NAN;
            const size_t k = v.size() / 2;
            std::nth_element(v.begin(), v.begin() + k, v.end());
            return v[k];
        };

        int votes = 0;
        int vote_bin = -1;

        for (int l = 0; l < lanes; ++l) {
            for (int i = 1; i < bins - gap_bins; ++i) {
                if (zvals[l][i - 1].size() < min_pts ||
                    zvals[l][i].size() >= min_pts ||
                    zvals[l][i + gap_bins].size() < min_pts)
                    continue;

                const float z_before = median(zvals[l][i - 1]);
                const float z_after  = median(zvals[l][i + gap_bins]);

                if (!std::isfinite(z_before) || !std::isfinite(z_after)) continue;

                if ((z_before - z_after) >= ditch_depth) {
                    ++votes;
                    vote_bin = i;
                    break;
                }
            }
        }

        if (votes >= 2) {
            ++confirm_;
            release_ = 0;
            if (confirm_ >= 3) {
                latched_ = true;
                latched_bin_ = vote_bin;
            }
        } else if (latched_) {
            ++release_;
            confirm_ = 0;
            if (release_ > 25) latched_ = false;
        }

        buildWall(latched_, latched_bin_, xmin, bin, width, tf_cloud.header);
        fuse(tf_cloud.header);
    }

    void gridCb(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        pcl::fromROSMsg(*msg, *last_grid_);
    }

    void buildWall(bool active, int bin, double xmin, double step,
                   double width, const std_msgs::msg::Header &h)
    {
        last_wall_->clear();
        if (!active || bin < 0) return;

        const double x = xmin + bin * step;

        double ground_z, wall_h;
        get_parameter("ground_z", ground_z);
        get_parameter("wall_height", wall_h);

        for (double y = -width / 2.0; y <= width / 2.0; y += 0.05)
            for (double z = ground_z; z <= ground_z + wall_h; z += 0.05)
                last_wall_->points.emplace_back(x, y, z);

        sensor_msgs::msg::PointCloud2 out;
        pcl::toROSMsg(*last_wall_, out);
        out.header = h;
        wall_pub_->publish(out);

        std_msgs::msg::Bool b;
        b.data = true;
        ditch_pub_->publish(b);

        std_msgs::msg::Float32 d;
        d.data = static_cast<float>(x);
        ditch_dist_pub_->publish(d);
    }

    void fuse(const std_msgs::msg::Header &h)
    {
        pcl::PointCloud<pcl::PointXYZ> fused = *last_grid_;
        fused += *last_wall_;

        sensor_msgs::msg::PointCloud2 out;
        pcl::toROSMsg(fused, out);
        out.header = h;
        safe_pub_->publish(out);
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DitchDetectorFinal>());
    rclcpp::shutdown();
    return 0;
}
