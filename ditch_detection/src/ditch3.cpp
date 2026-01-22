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

class DitchDetectorDZ : public rclcpp::Node
{
public:
    DitchDetectorDZ()
    : Node("ditch_detector_dz"),
      tf_buffer_(get_clock()),
      tf_listener_(tf_buffer_)
    {
        cloud_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            "/zed/zed_node/point_cloud/cloud_registered",
            rclcpp::SensorDataQoS(),
            std::bind(&DitchDetectorDZ::cloudCallback, this, std::placeholders::_1));

        local_grid_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            "/local_grid_obstacle",
            rclcpp::SensorDataQoS(),
            std::bind(&DitchDetectorDZ::localGridCallback, this, std::placeholders::_1));

        wall_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>("/fake_wall_cloud", 10);
        safe_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>("/local_grid_safe", 10);
        ditch_pub_ = create_publisher<std_msgs::msg::Bool>("/ditch_detected", 10);
        ditch_dist_pub_ = create_publisher<std_msgs::msg::Float32>("/ditch_distance", 10);

        declare_parameter("target_frame", "base_link");
        declare_parameter("xmin", 0.6);
        declare_parameter("xmax", 3.0);
        declare_parameter("bin_size", 0.15);
        declare_parameter("corridor_width", 2.0);
        declare_parameter("min_points_per_bin", 8);
        declare_parameter("wall_height", 0.7);
        declare_parameter("ground_z", -0.25);
    }

private:
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_sub_;
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr local_grid_sub_;

    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr wall_pub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr safe_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr ditch_pub_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr ditch_dist_pub_;

    tf2_ros::Buffer tf_buffer_;
    tf2_ros::TransformListener tf_listener_;

    pcl::PointCloud<pcl::PointXYZ>::Ptr last_wall_{new pcl::PointCloud<pcl::PointXYZ>()};
    pcl::PointCloud<pcl::PointXYZ>::Ptr last_local_grid_{new pcl::PointCloud<pcl::PointXYZ>()};

    bool ditch_latched_ = false;
    int confirm_count_ = 0;
    int release_count_ = 0;
    int latched_bin_ = -1;

    void cloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        std::string target_frame;
        get_parameter("target_frame", target_frame);

        sensor_msgs::msg::PointCloud2 cloud_tf;
        try
        {
            auto tf = tf_buffer_.lookupTransform(
                target_frame, msg->header.frame_id, tf2::TimePointZero);
            tf2::doTransform(*msg, cloud_tf, tf);
        }
        catch (...)
        {
            return;
        }

        pcl::PointCloud<pcl::PointXYZ> cloud;
        pcl::fromROSMsg(cloud_tf, cloud);

        double xmin, xmax, bin_size, corridor_w, wall_h, ground_z;
        int min_pts;

        get_parameter("xmin", xmin);
        get_parameter("xmax", xmax);
        get_parameter("bin_size", bin_size);
        get_parameter("corridor_width", corridor_w);
        get_parameter("min_points_per_bin", min_pts);
        get_parameter("wall_height", wall_h);
        get_parameter("ground_z", ground_z);

        int bins = static_cast<int>((xmax - xmin) / bin_size);
        std::vector<int> counts(bins, 0);

        for (const auto &p : cloud.points)
        {
            if (!std::isfinite(p.x)) continue;
            if (p.x < xmin || p.x > xmax) continue;
            if (std::abs(p.y) > corridor_w * 0.5) continue;

            int idx = static_cast<int>((p.x - xmin) / bin_size);
            if (idx >= 0 && idx < bins)
                counts[idx]++;
        }

        bool raw_detect = false;
        int raw_bin = -1;

        for (int i = 1; i < bins; i++)
        {
            if (counts[i - 1] >= min_pts && counts[i] < min_pts)
            {
                raw_detect = true;
                raw_bin = i;
                break;
            }
        }

        if (raw_detect)
        {
            confirm_count_++;
            release_count_ = 0;

            if (confirm_count_ >= 3)
            {
                ditch_latched_ = true;
                latched_bin_ = raw_bin;
            }
        }
        else if (ditch_latched_)
        {
            release_count_++;
            confirm_count_ = 0;

            if (release_count_ > 30)
                ditch_latched_ = false;
        }

        buildWall(ditch_latched_, latched_bin_, xmin, bin_size,
                  corridor_w, wall_h, ground_z, cloud_tf.header);
        fuseAndPublish(cloud_tf.header);
    }

    void localGridCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        pcl::fromROSMsg(*msg, *last_local_grid_);
    }

    void buildWall(bool active, int bin, double xmin, double bin_size,
                   double width, double wall_h, double ground_z,
                   const std_msgs::msg::Header &header)
    {
        last_wall_->clear();
        last_wall_->header.frame_id = header.frame_id;

        if (!active || bin < 0) return;

        float dist = xmin + (bin - 1.5) * bin_size;

        for (double y = -width; y <= width; y += 0.05)
            for (double z = ground_z; z <= ground_z + wall_h; z += 0.05)
                last_wall_->points.emplace_back(dist, y, z);

        sensor_msgs::msg::PointCloud2 out;
        pcl::toROSMsg(*last_wall_, out);
        out.header = header;
        wall_pub_->publish(out);

        std_msgs::msg::Bool b; b.data = true;
        ditch_pub_->publish(b);

        std_msgs::msg::Float32 d; d.data = dist;
        ditch_dist_pub_->publish(d);
    }

    void fuseAndPublish(const std_msgs::msg::Header &header)
    {
        pcl::PointCloud<pcl::PointXYZ> fused = *last_local_grid_;
        fused += *last_wall_;

        sensor_msgs::msg::PointCloud2 out;
        pcl::toROSMsg(fused, out);
        out.header = header;
        safe_pub_->publish(out);
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DitchDetectorDZ>());
    rclcpp::shutdown();
    return 0;
}
