#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/float32.hpp>

#include <cmath>
#include <limits>

class EnvironmentPresenceLogger : public rclcpp::Node
{
public:
    EnvironmentPresenceLogger()
    : Node("environment_presence_logger")
    {
        declare_parameter<std::string>("cloud_topic", "/local_grid_safe");
        declare_parameter<std::string>("ditch_topic", "/ditch_detected");
        declare_parameter<std::string>("ditch_dist_topic", "/ditch_distance");

        declare_parameter<double>("min_x", 0.5);
        declare_parameter<double>("max_x", 3.0);
        declare_parameter<double>("half_width", 0.40);
        declare_parameter<double>("side_thresh", 0.15);
        declare_parameter<double>("log_rate_hz", 3.0);

        cloud_topic_ = get_parameter("cloud_topic").as_string();
        ditch_topic_ = get_parameter("ditch_topic").as_string();
        ditch_dist_topic_ = get_parameter("ditch_dist_topic").as_string();

        min_x_ = get_parameter("min_x").as_double();
        max_x_ = get_parameter("max_x").as_double();
        half_w_ = get_parameter("half_width").as_double();
        side_thresh_ = get_parameter("side_thresh").as_double();
        log_rate_hz_ = get_parameter("log_rate_hz").as_double();

        cloud_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            cloud_topic_, rclcpp::SensorDataQoS(),
            std::bind(&EnvironmentPresenceLogger::cloudCallback, this, std::placeholders::_1));

        ditch_sub_ = create_subscription<std_msgs::msg::Bool>(
            ditch_topic_, 10,
            std::bind(&EnvironmentPresenceLogger::ditchCallback, this, std::placeholders::_1));

        ditch_dist_sub_ = create_subscription<std_msgs::msg::Float32>(
            ditch_dist_topic_, 10,
            std::bind(&EnvironmentPresenceLogger::ditchDistCallback, this, std::placeholders::_1));

        log_timer_ = create_wall_timer(
            std::chrono::duration<double>(1.0 / log_rate_hz_),
            std::bind(&EnvironmentPresenceLogger::logState, this));
    }

private:
    std::string cloud_topic_, ditch_topic_, ditch_dist_topic_;
    double min_x_, max_x_, half_w_, side_thresh_, log_rate_hz_;

    bool obstacle_detect_{false};
    float obs_x_{0.0f}, obs_y_{0.0f};

    bool ditch_detect_{false};
    bool ditch_latched_{false};
    float ditch_dist_{0.0f};
    rclcpp::Time ditch_start_time_;

    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr ditch_sub_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr ditch_dist_sub_;
    rclcpp::TimerBase::SharedPtr log_timer_;

    void cloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        bool found = false;
        float best_x = std::numeric_limits<float>::max();

        sensor_msgs::PointCloud2ConstIterator<float> it_x(*msg, "x");
        sensor_msgs::PointCloud2ConstIterator<float> it_y(*msg, "y");

        for (; it_x != it_x.end(); ++it_x, ++it_y)
        {
            const float px = *it_x;
            const float py = *it_y;

            if (px < min_x_ || px > max_x_) continue;
            if (std::abs(py) > half_w_) continue;

            if (px < best_x)
            {
                best_x = px;
                obs_x_ = px;
                obs_y_ = py;
                found = true;
            }
        }

        obstacle_detect_ = found;
    }

    void ditchCallback(const std_msgs::msg::Bool::SharedPtr msg)
    {
        if (msg->data && !ditch_latched_)
        {
            ditch_latched_ = true;
            ditch_start_time_ = now();
            RCLCPP_WARN(
                get_logger(),
                "[DITCH][ENTER] dist=%.2f m",
                ditch_dist_);
        }

        if (!msg->data && ditch_latched_)
        {
            double t = (now() - ditch_start_time_).seconds();
            ditch_latched_ = false;
            RCLCPP_INFO(
                get_logger(),
                "[DITCH][EXIT] duration=%.2f s",
                t);
        }

        ditch_detect_ = msg->data;
    }

    void ditchDistCallback(const std_msgs::msg::Float32::SharedPtr msg)
    {
        ditch_dist_ = msg->data;
    }

    void logState()
    {
        if (obstacle_detect_)
        {
            const float dist = std::hypot(obs_x_, obs_y_);
            const char* side =
                (obs_y_ >  side_thresh_) ? "LEFT" :
                (obs_y_ < -side_thresh_) ? "RIGHT" : "CENTER";

            RCLCPP_INFO(
                get_logger(),
                "[OBS] x=%.2f y=%.2f dist=%.2f side=%s",
                obs_x_, obs_y_, dist, side);
        }
        else
        {
            RCLCPP_INFO(get_logger(), "[OBS] CLEAR");
        }

        if (ditch_latched_)
        {
            double age = (now() - ditch_start_time_).seconds();
            const char* sev =
                (ditch_dist_ < 1.0) ? "CRITICAL" :
                (ditch_dist_ < 1.8) ? "HIGH" : "MEDIUM";

            RCLCPP_WARN(
                get_logger(),
                "[DITCH][HOLD] dist=%.2f m age=%.2f s severity=%s",
                ditch_dist_, age, sev);
        }
        else
        {
            RCLCPP_INFO(get_logger(), "[DITCH] CLEAR");
        }
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<EnvironmentPresenceLogger>());
    rclcpp::shutdown();
    return 0;
}
