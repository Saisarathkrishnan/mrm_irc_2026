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
#include <limits>

class GapBasedDitchDetector : public rclcpp::Node
{
public:
    GapBasedDitchDetector()
    : Node("gap_based_ditch_detector"),
      tf_buffer_(get_clock()),
      tf_listener_(tf_buffer_)
    {
        cloud_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            "/zed/zed_node/point_cloud/cloud_registered",
            rclcpp::SensorDataQoS(),
            std::bind(&GapBasedDitchDetector::cloudCallback, this, std::placeholders::_1));

        wall_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>("/fake_wall_cloud", 10);
        ditch_pub_ = create_publisher<std_msgs::msg::Bool>("/ditch_detected", 10);
        ditch_dist_pub_ = create_publisher<std_msgs::msg::Float32>("/ditch_distance", 10);

        declare_parameter("target_frame", "base_link");

        declare_parameter("xmin", 0.6);
        declare_parameter("xmax", 3.5);
        declare_parameter("bin_size", 0.2);
        declare_parameter("corridor_width", 1.6);

        declare_parameter("z_flatness_tol", 0.25);
        declare_parameter("ditch_depth", 0.12);
        declare_parameter("min_gap_length_m", 0.4);

        declare_parameter("temporal_confirm_frames", 2);
        declare_parameter("wall_height", 0.6);

        RCLCPP_INFO(get_logger(),
            "Gap-based ditch detector (RELAXED / REAL-WORLD) started");
    }

private:
    struct BinStats
    {
        int count = 0;
        double min_z = std::numeric_limits<double>::max();
        double max_z = -std::numeric_limits<double>::max();
    };

    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_sub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr wall_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr ditch_pub_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr ditch_dist_pub_;

    tf2_ros::Buffer tf_buffer_;
    tf2_ros::TransformListener tf_listener_;

    int confirm_count_ = 0;

    void cloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        std::string target_frame;
        get_parameter("target_frame", target_frame);

        sensor_msgs::msg::PointCloud2 cloud_tf;
        try
        {
            auto tf = tf_buffer_.lookupTransform(
                target_frame,
                msg->header.frame_id,
                tf2::TimePointZero);

            tf2::doTransform(*msg, cloud_tf, tf);
        }
        catch (const tf2::TransformException &)
        {
            publishEmpty(msg->header);
            return;
        }

        pcl::PointCloud<pcl::PointXYZ> cloud;
        pcl::fromROSMsg(cloud_tf, cloud);

        double xmin, xmax, bin_size, corridor_w;
        double z_tol, ditch_depth, min_gap_m, wall_h;
        int confirm_req;

        get_parameter("xmin", xmin);
        get_parameter("xmax", xmax);
        get_parameter("bin_size", bin_size);
        get_parameter("corridor_width", corridor_w);
        get_parameter("z_flatness_tol", z_tol);
        get_parameter("ditch_depth", ditch_depth);
        get_parameter("min_gap_length_m", min_gap_m);
        get_parameter("temporal_confirm_frames", confirm_req);
        get_parameter("wall_height", wall_h);

        int bins = static_cast<int>((xmax - xmin) / bin_size);
        if (bins <= 0)
        {
            publishEmpty(cloud_tf.header);
            return;
        }

        std::vector<BinStats> stats(bins);

        for (const auto &p : cloud.points)
        {
            if (!std::isfinite(p.x) || !std::isfinite(p.z)) continue;
            if (p.x < xmin || p.x > xmax) continue;
            if (std::abs(p.y) > corridor_w * 0.5) continue;

            int idx = (p.x - xmin) / bin_size;
            if (idx < 0 || idx >= bins) continue;

            auto &b = stats[idx];
            b.count++;
            b.min_z = std::min(b.min_z, (double)p.z);
            b.max_z = std::max(b.max_z, (double)p.z);
        }

        bool ditch_raw = false;
        int gap_start = -1;
        int gap_len = 0;
        double ground_z_ref = NAN;

        for (int i = 0; i < bins; i++)
        {
            const auto &b = stats[i];

            bool ground_like =
                (b.count >= 4) &&
                ((b.max_z - b.min_z) < z_tol);

            bool gap_like =
                (b.count < 3);

            if (ground_like && !std::isfinite(ground_z_ref))
                ground_z_ref = b.min_z;

            if (gap_like && std::isfinite(ground_z_ref))
            {
                if (gap_len == 0) gap_start = i;
                gap_len++;

                if (gap_len * bin_size >= min_gap_m)
                {
                    ditch_raw = true;
                    break;
                }
            }

            if (ground_like && std::isfinite(ground_z_ref))
            {
                double drop = ground_z_ref - b.min_z;
                if (drop >= ditch_depth)
                {
                    ditch_raw = true;
                    break;
                }
            }
        }

        if (ditch_raw)
            confirm_count_++;
        else
            confirm_count_ = 0;

        bool ditch_confirmed = confirm_count_ >= confirm_req;

        publishResults(
            ditch_confirmed,
            gap_start,
            xmin,
            bin_size,
            corridor_w,
            wall_h,
            cloud_tf.header);
    }

    void publishResults(bool detected, int gap_bin, double xmin,
                        double bin_size, double width, double wall_h,
                        const std_msgs::msg::Header &header)
    {
        std_msgs::msg::Bool b;
        b.data = detected;
        ditch_pub_->publish(b);

        pcl::PointCloud<pcl::PointXYZ> wall;
        wall.header.frame_id = header.frame_id;

        if (detected && gap_bin >= 0)
        {
            float dist = xmin + gap_bin * bin_size;
            std_msgs::msg::Float32 d;
            d.data = dist;
            ditch_dist_pub_->publish(d);

            for (double y = -width * 0.5; y <= width * 0.5; y += 0.05)
                for (double z = 0.0; z <= wall_h; z += 0.05)
                    wall.points.emplace_back(dist, y, z);
        }

        sensor_msgs::msg::PointCloud2 out;
        pcl::toROSMsg(wall, out);
        out.header = header;
        wall_pub_->publish(out);
    }

    void publishEmpty(const std_msgs::msg::Header &header)
    {
        pcl::PointCloud<pcl::PointXYZ> empty;
        empty.header.frame_id = header.frame_id;

        sensor_msgs::msg::PointCloud2 out;
        pcl::toROSMsg(empty, out);
        out.header = header;
        wall_pub_->publish(out);

        std_msgs::msg::Bool b;
        b.data = false;
        ditch_pub_->publish(b);
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<GapBasedDitchDetector>());
    rclcpp::shutdown();
    return 0;
}
