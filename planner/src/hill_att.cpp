#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <custom_msgs/msg/imu_data.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>

#include <cmath>
#include <algorithm>
#include <limits>
#include <vector>

struct Coordinates {
    double latitude{0.0};
    double longitude{0.0};
};

class HillAtt : public rclcpp::Node
{
public:
    HillAtt() : Node("hill_att")
    {
        declare_parameter("goal1_lat", 0.0);
        declare_parameter("goal1_lon", 0.0);
        declare_parameter("goal2_lat", 0.0);
        declare_parameter("goal2_lon", 0.0);
        declare_parameter("gps_topic", "/fix");
        declare_parameter("imu_topic", "/external_imu");
        declare_parameter("cloud_topic", "/local_grid_safe");
        declare_parameter("cmd_vel_topic", "/cmd_vel");

        goals_.push_back({
            get_parameter("goal1_lat").as_double(),
            get_parameter("goal1_lon").as_double()
        });
        goals_.push_back({
            get_parameter("goal2_lat").as_double(),
            get_parameter("goal2_lon").as_double()
        });

        cmd_pub_ = create_publisher<geometry_msgs::msg::Twist>(
            get_parameter("cmd_vel_topic").as_string(), 10);

        gps_sub_ = create_subscription<sensor_msgs::msg::NavSatFix>(
            get_parameter("gps_topic").as_string(), 10,
            std::bind(&HillAtt::gpsCallback, this, std::placeholders::_1));

        imu_sub_ = create_subscription<custom_msgs::msg::ImuData>(
            get_parameter("imu_topic").as_string(), 10,
            std::bind(&HillAtt::imuCallback, this, std::placeholders::_1));

        cloud_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            get_parameter("cloud_topic").as_string(), 10,
            std::bind(&HillAtt::cloudCallback, this, std::placeholders::_1));

        timer_ = create_wall_timer(
            std::chrono::milliseconds(10),
            std::bind(&HillAtt::controlLoop, this));

        RCLCPP_INFO(get_logger(), "Node started");
        RCLCPP_INFO(get_logger(), "Goal 1 set: %.6f %.6f", goals_[0].latitude, goals_[0].longitude);
        RCLCPP_INFO(get_logger(), "Goal 2 set: %.6f %.6f", goals_[1].latitude, goals_[1].longitude);
    }

private:
    void gpsCallback(const sensor_msgs::msg::NavSatFix::SharedPtr msg)
    {
        if (msg->status.status < sensor_msgs::msg::NavSatStatus::STATUS_FIX) return;
        curr_.latitude = msg->latitude;
        curr_.longitude = msg->longitude;
        last_gps_time_ = now();
        gps_valid_ = true;
    }

    void imuCallback(const custom_msgs::msg::ImuData::SharedPtr msg)
    {
        yaw_deg_ = normalize360(msg->orientation.z);
    }

    void cloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        obstacle_detected_ = false;
        obs_x_ = obs_y_ = 0.0;

        float best_x = std::numeric_limits<float>::max();
        sensor_msgs::PointCloud2ConstIterator<float> it_x(*msg, "x");
        sensor_msgs::PointCloud2ConstIterator<float> it_y(*msg, "y");

        for (; it_x != it_x.end(); ++it_x, ++it_y)
        {
            float px = *it_x;
            float py = *it_y;
            if (px < 0.5 || px > 3.0) continue;
            if (std::abs(py) > 0.4) continue;
            if (px < best_x) {
                best_x = px;
                obs_x_ = px;
                obs_y_ = py;
                obstacle_detected_ = true;
            }
        }
    }

    void controlLoop()
    {
        if (!gps_valid_ || goal_index_ >= goals_.size()) return;

        if ((now() - last_gps_time_).seconds() > 1.5) {
            publishZero();
            return;
        }

        if (obstacle_detected_) {
            geometry_msgs::msg::Twist cmd;
            cmd.angular.z = (obs_y_ >= 0.0 ? -1.0 : 1.0) * 0.8;
            cmd_pub_->publish(cmd);
            return;
        }

        double dist = haversine(curr_, goals_[goal_index_]);

        if (dist <= 1.5) {
            publishZero();
            RCLCPP_INFO(
                get_logger(),
                "Reached goal %zu → %s",
                goal_index_ + 1,
                goal_index_ == 0 ? "switching to goal 2" : "stopping");
            goal_index_++;
            return;
        }

        double target = gpsAngleFix(gpsBearing(curr_, goals_[goal_index_]));
        double err = headingError(target, yaw_deg_);

        geometry_msgs::msg::Twist cmd;
        cmd.linear.x  = std::clamp(dist * 0.4, 0.4, 2.0);
        cmd.angular.z = std::clamp(err * 0.01, -0.6, 0.6);
        cmd_pub_->publish(cmd);
    }

    void publishZero()
    {
        geometry_msgs::msg::Twist z;
        cmd_pub_->publish(z);
    }

    double haversine(Coordinates c, Coordinates d)
    {
        double lat1 = c.latitude * M_PI / 180.0;
        double lat2 = d.latitude * M_PI / 180.0;
        double dLat = lat2 - lat1;
        double dLon = (d.longitude - c.longitude) * M_PI / 180.0;
        double h = sin(dLat * 0.5) * sin(dLat * 0.5) +
                   cos(lat1) * cos(lat2) *
                   sin(dLon * 0.5) * sin(dLon * 0.5);
        return 2.0 * 6371000.0 * asin(sqrt(h));
    }

    double gpsBearing(Coordinates c, Coordinates d)
    {
        double lat1 = c.latitude * M_PI / 180.0;
        double lon1 = c.longitude * M_PI / 180.0;
        double lat2 = d.latitude * M_PI / 180.0;
        double lon2 = d.longitude * M_PI / 180.0;
        double dLon = lon2 - lon1;
        double x = sin(dLon) * cos(lat2);
        double y = cos(lat1) * sin(lat2) -
                   sin(lat1) * cos(lat2) * cos(dLon);
        double a = atan2(x, y) * 180.0 / M_PI;
        return a < 0 ? a + 360.0 : a;
    }

    double gpsAngleFix(double a)
    {
        double r = -(fmod(a + 90.0, 360.0));
        if (r > 180.0) r -= 360.0;
        if (r < -180.0) r += 360.0;
        return r;
    }

    double headingError(double t, double c)
    {
        double d = t - c;
        if (d > 180.0) d -= 360.0;
        if (d < -180.0) d += 360.0;
        return d;
    }

    double normalize360(double a)
    {
        a = fmod(a, 360.0);
        return a < 0 ? a + 360.0 : a;
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
    rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr gps_sub_;
    rclcpp::Subscription<custom_msgs::msg::ImuData>::SharedPtr imu_sub_;
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_sub_;
    rclcpp::TimerBase::SharedPtr timer_;

    Coordinates curr_;
    std::vector<Coordinates> goals_;
    size_t goal_index_{0};

    rclcpp::Time last_gps_time_;
    bool gps_valid_{false};
    bool obstacle_detected_{false};

    double yaw_deg_{0.0};
    double obs_x_{0.0}, obs_y_{0.0};
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<HillAtt>());
    rclcpp::shutdown();
    return 0;
}
