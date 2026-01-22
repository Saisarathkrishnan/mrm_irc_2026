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

        wall_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>("/fake_wall_cloud", 10);
        ditch_pub_ = create_publisher<std_msgs::msg::Bool>("/ditch_detected", 10);
        ditch_dist_pub_ = create_publisher<std_msgs::msg::Float32>("/ditch_distance", 10);

        declare_parameter("target_frame", "base_link");

        declare_parameter("xmin", 0.6);
        declare_parameter("xmax", 3.0);
        declare_parameter("bin_size", 0.15);
        declare_parameter("corridor_width", 2.0);

        declare_parameter("min_points_per_bin", 5);
        declare_parameter("min_drop_m", 0.25);

        declare_parameter("wall_height", 0.7);

        RCLCPP_INFO(get_logger(), "Ditch detector (ΔZ-based, stereo-correct) started");
    }

private:
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_sub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr wall_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr ditch_pub_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr ditch_dist_pub_;

    tf2_ros::Buffer tf_buffer_;
    tf2_ros::TransformListener tf_listener_;

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
        catch (...)
        {
            publishEmpty(msg->header);
            return;
        }

        pcl::PointCloud<pcl::PointXYZ> cloud;
        pcl::fromROSMsg(cloud_tf, cloud);

        double xmin, xmax, bin_size, corridor_w, min_drop, wall_h;
        int min_pts;

        get_parameter("xmin", xmin);
        get_parameter("xmax", xmax);
        get_parameter("bin_size", bin_size);
        get_parameter("corridor_width", corridor_w);
        get_parameter("min_points_per_bin", min_pts);
        get_parameter("min_drop_m", min_drop);
        get_parameter("wall_height", wall_h);

        int bins = static_cast<int>((xmax - xmin) / bin_size);
        if (bins <= 1)
        {
            publishEmpty(cloud_tf.header);
            return;
        }

        std::vector<float> mean_z(bins, 0.0f);
        std::vector<int> counts(bins, 0);

        for (const auto &p : cloud.points)
        {
            if (!std::isfinite(p.x) || !std::isfinite(p.z)) continue;
            if (p.x < xmin || p.x > xmax) continue;
            if (std::abs(p.y) > corridor_w * 0.5) continue;

            int idx = static_cast<int>((p.x - xmin) / bin_size);
            if (idx < 0 || idx >= bins) continue;

            mean_z[idx] += p.z;
            counts[idx]++;
        }

        for (int i = 0; i < bins; i++)
            if (counts[i] > 0)
                mean_z[i] /= counts[i];

        bool ditch_found = false;
        int ditch_bin = -1;

        for (int i = 1; i < bins; i++)
        {
            if (counts[i] < min_pts || counts[i - 1] < min_pts)
                continue;

            float dz = mean_z[i] - mean_z[i - 1];

            if (dz < -min_drop)
            {
                ditch_found = true;
                ditch_bin = i;
                break;
            }
        }

        publishResults(
            ditch_found,
            ditch_bin,
            xmin,
            bin_size,
            corridor_w,
            wall_h,
            cloud_tf.header);
    }

    void publishResults(bool detected, int bin,
                        double xmin, double bin_size,
                        double width, double wall_h,
                        const std_msgs::msg::Header &header)
    {
        std_msgs::msg::Bool b;
        b.data = detected;
        ditch_pub_->publish(b);

        pcl::PointCloud<pcl::PointXYZ> wall;
        wall.header.frame_id = header.frame_id;

        if (detected && bin >= 0)
        {
            float dist = xmin + (bin - 1) * bin_size;

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
    rclcpp::spin(std::make_shared<DitchDetectorDZ>());
    rclcpp::shutdown();
    return 0;
}
