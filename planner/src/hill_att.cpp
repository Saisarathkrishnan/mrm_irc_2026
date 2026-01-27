#include "stack/irc_planner.hpp"

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <custom_msgs/msg/imu_data.hpp>

#include <cmath>
#include <algorithm>

using namespace planner;
using std::placeholders::_1;

class HillATT : public rclcpp::Node
{
public:
    HillATT() : Node("hill_att")
    {
        declare_parameter("att1_lat", 0.0);
        declare_parameter("att1_lon", 0.0);
        declare_parameter("att2_lat", 0.0);
        declare_parameter("att2_lon", 0.0);

        att_[0].latitude  = get_parameter("att1_lat").as_double();
        att_[0].longitude = get_parameter("att1_lon").as_double();
        att_[1].latitude  = get_parameter("att2_lat").as_double();
        att_[1].longitude = get_parameter("att2_lon").as_double();

        if (att_[0].latitude == 0.0 || att_[1].latitude == 0.0) {
            RCLCPP_FATAL(get_logger(), "hill_att: ATT GPS params missing");
            rclcpp::shutdown();
        }

        vel_pub_ = create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

        gps_sub_ = create_subscription<sensor_msgs::msg::NavSatFix>(
            "/fix", 10, std::bind(&HillATT::gpsCb, this, _1));

        imu_sub_ = create_subscription<custom_msgs::msg::ImuData>(
            "/external_imu", 10, std::bind(&HillATT::imuCb, this, _1));

        pcl_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            "/local_grid_safe", 10, std::bind(&HillATT::pclCb, this, _1));

        timer_ = create_wall_timer(
            std::chrono::milliseconds(10),
            std::bind(&HillATT::run, this));
    }

private:
    Coordinates curr_{};
    Coordinates att_[2];
    int att_idx_{0};

    bool gps_ok_{false};
    double yaw_{0.0};

    bool obstacle_{false};
    double obs_x_{0.0};
    double obs_y_{0.0};

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr vel_pub_;
    rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr gps_sub_;
    rclcpp::Subscription<custom_msgs::msg::ImuData>::SharedPtr imu_sub_;
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr pcl_sub_;
    rclcpp::TimerBase::SharedPtr timer_;

    void gpsCb(const sensor_msgs::msg::NavSatFix::SharedPtr msg)
    {
        if (msg->status.status < sensor_msgs::msg::NavSatStatus::STATUS_FIX)
            return;

        curr_.latitude  = msg->latitude;
        curr_.longitude = msg->longitude;
        gps_ok_ = true;
    }

    void imuCb(const custom_msgs::msg::ImuData::SharedPtr msg)
    {
        yaw_ = normalize360(msg->orientation.z);
    }

    void pclCb(const sensor_msgs::msg::PointCloud2::SharedPtr cloud)
    {
        obstacle_ = false;

        sensor_msgs::PointCloud2ConstIterator<float> it_x(*cloud, "x");
        sensor_msgs::PointCloud2ConstIterator<float> it_y(*cloud, "y");

        float best_x = std::numeric_limits<float>::max();

        for (; it_x != it_x.end(); ++it_x, ++it_y)
        {
            float x = *it_x;
            float y = *it_y;

            if (x < 0.5 || x > 3.0) continue;
            if (std::abs(y) > 0.5) continue;

            if (x < best_x)
            {
                best_x = x;
                obs_x_ = x;
                obs_y_ = y;
                obstacle_ = true;
            }
        }
    }

    void run()
    {
        geometry_msgs::msg::Twist cmd;

        if (!gps_ok_ || att_idx_ >= 2) {
            vel_pub_->publish(cmd);
            return;
        }

        if (obstacle_ && obs_x_ < 2.5)
        {
            cmd.linear.x  = 0.0;
            cmd.angular.z = (obs_y_ >= 0.0 ? -1.0 : 1.0) * 0.8;
            vel_pub_->publish(cmd);
            return;
        }

        Coordinates target = att_[att_idx_];
        double dist = haversine(curr_, target);

        if (dist <= kDistanceThreshold)
        {
            vel_pub_->publish(geometry_msgs::msg::Twist());
            att_idx_++;
            return;
        }

        double target_yaw = gpsBearing(curr_, target);
        double err = headingError(target_yaw, yaw_);

        cmd.linear.x =
            std::clamp(std::pow(dist, 3) / 10.0,
                       0.4,
                       kMaxLinearVel);

        cmd.angular.z =
            std::clamp(-err * 0.02,
                       -kMaxAngularVel,
                       kMaxAngularVel);

        vel_pub_->publish(cmd);
    }

    double haversine(Coordinates a, Coordinates b)
    {
        double lat1 = a.latitude * M_PI / 180.0;
        double lat2 = b.latitude * M_PI / 180.0;
        double dLat = lat2 - lat1;
        double dLon = (b.longitude - a.longitude) * M_PI / 180.0;

        double h = sin(dLat * 0.5) * sin(dLat * 0.5) +
                   cos(lat1) * cos(lat2) *
                   sin(dLon * 0.5) * sin(dLon * 0.5);

        return 2.0 * 6371000.0 * asin(sqrt(h));
    }

    double gpsBearing(Coordinates curr, Coordinates dest)
    {
        double lat1 = curr.latitude * M_PI / 180.0;
        double lon1 = curr.longitude * M_PI / 180.0;
        double lat2 = dest.latitude * M_PI / 180.0;
        double lon2 = dest.longitude * M_PI / 180.0;

        double dLon = lon2 - lon1;

        double x = sin(dLon) * cos(lat2);
        double y = cos(lat1) * sin(lat2) -
                   sin(lat1) * cos(lat2) * cos(dLon);

        double angle = atan2(x, y) * 180.0 / M_PI;
        return angle < 0 ? angle + 360.0 : angle;
    }

    double headingError(double target, double current)
    {
        double diff = target - current;
        if (diff > 180.0) diff -= 360.0;
        if (diff < -180.0) diff += 360.0;
        return diff;
    }

    double normalize360(double angle)
    {
        angle = fmod(angle, 360.0);
        if (angle < 0.0) angle += 360.0;
        return angle;
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<HillATT>());
    rclcpp::shutdown();
    return 0;
}
