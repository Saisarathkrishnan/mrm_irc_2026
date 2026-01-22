#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/float32.hpp>

#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>

#include <algorithm>
#include <vector>
#include <cmath>

class DitchDetector : public rclcpp::Node
{
public:
    DitchDetector()
    : Node("ditch_detector"), ditch_active_(false), clear_count_(0)
    {
        RCLCPP_WARN(this->get_logger(), "DitchDetector node started");

        sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
            "/zed/zed_node/point_cloud/cloud_registered", 10,
            std::bind(&DitchDetector::cloudCb, this, std::placeholders::_1));

        ditch_pub_ = this->create_publisher<std_msgs::msg::Bool>("/ditch_detected", 10);
        depth_pub_ = this->create_publisher<std_msgs::msg::Float32>("/ditch_depth", 10);
        wall_pub_  = this->create_publisher<sensor_msgs::msg::PointCloud2>("/ditch_wall", 10);

        RCLCPP_WARN(this->get_logger(), "Publishers created (ditch_wall advertised)");
    }

private:
    void cloudCb(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        RCLCPP_WARN_THROTTLE(
            this->get_logger(), *this->get_clock(), 2000,
            "PointCloud callback alive");

        pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(
            new pcl::PointCloud<pcl::PointXYZRGB>());

        try {
            pcl::fromROSMsg(*msg, *cloud);
        } catch (...) {
            RCLCPP_ERROR(this->get_logger(), "fromROSMsg failed");
            return;
        }

        std::vector<float> z_vals;
        z_vals.reserve(1000);

        for (const auto &p : cloud->points)
        {
            if (!std::isfinite(p.z)) continue;
            if (p.x < 0.5 || p.x > 3.0) continue;
            if (p.y < -0.6 || p.y > 0.6) continue;
            if (p.z < -2.0 || p.z > 0.3) continue;
            z_vals.push_back(p.z);
        }

        if (z_vals.size() < 80)
        {
            RCLCPP_WARN_THROTTLE(
                this->get_logger(), *this->get_clock(), 2000,
                "Not enough points: %zu", z_vals.size());
            return;
        }

        std::sort(z_vals.begin(), z_vals.end());

        float ground_z = z_vals[static_cast<size_t>(z_vals.size() * 0.15)];
        float min_z    = z_vals.front();
        float drop     = ground_z - min_z;

        constexpr float DETECT_DROP  = 0.15;
        constexpr float CLEAR_DROP   = 0.05;
        constexpr int   CLEAR_FRAMES = 6;

        if (drop > DETECT_DROP)
        {
            ditch_active_ = true;
            clear_count_ = 0;
        }
        else if (ditch_active_ && drop < CLEAR_DROP)
        {
            if (++clear_count_ > CLEAR_FRAMES)
                ditch_active_ = false;
        }

        std_msgs::msg::Bool dmsg;
        dmsg.data = ditch_active_;
        ditch_pub_->publish(dmsg);

        std_msgs::msg::Float32 fmsg;
        fmsg.data = drop;
        depth_pub_->publish(fmsg);

        if (!ditch_active_)
            return;

        pcl::PointCloud<pcl::PointXYZ>::Ptr wall_cloud(
            new pcl::PointCloud<pcl::PointXYZ>());

        wall_cloud->header = cloud->header;

        constexpr float DITCH_MARGIN = 0.08;
        constexpr float WALL_HEIGHT  = 0.8;
        constexpr float WALL_STEP    = 0.05;

        for (const auto &p : cloud->points)
        {
            if (!std::isfinite(p.z)) continue;
            if (p.x < 0.5 || p.x > 3.0) continue;
            if (p.y < -0.6 || p.y > 0.6) continue;

            if (p.z < ground_z - DITCH_MARGIN)
            {
                for (float h = 0.0; h <= WALL_HEIGHT; h += WALL_STEP)
                {
                    wall_cloud->points.emplace_back(p.x, p.y, ground_z + h);
                }
            }
        }

        RCLCPP_WARN_THROTTLE(
            this->get_logger(), *this->get_clock(), 1000,
            "Wall points: %zu", wall_cloud->points.size());

        sensor_msgs::msg::PointCloud2 wall_msg;
        pcl::toROSMsg(*wall_cloud, wall_msg);
        wall_msg.header = msg->header;

        wall_pub_->publish(wall_msg);
    }

    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr sub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr ditch_pub_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr depth_pub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr wall_pub_;

    bool ditch_active_;
    int clear_count_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DitchDetector>());
    rclcpp::shutdown();
    return 0;
}
