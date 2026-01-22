#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>
#include <cmath>
#include <limits>

class ObstaclePresenceLogger : public rclcpp::Node
{
public:
    ObstaclePresenceLogger()
    : Node("obstacle_presence_logger"), obstacle_detected_(false)
    {
        declare_parameter<std::string>("cloud_topic", "/local_grid_obstacle");
        declare_parameter<double>("min_x", 0.5);
        declare_parameter<double>("max_x", 3.0);
        declare_parameter<double>("half_width", 0.4);
        declare_parameter<double>("side_thresh", 0.15);
        declare_parameter<double>("log_rate_hz", 10.0);

        loadParams();

        cloud_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            cloud_topic_, rclcpp::SensorDataQoS(),
            std::bind(&ObstaclePresenceLogger::cloudCallback, this, std::placeholders::_1));

        log_timer_ = create_wall_timer(
            std::chrono::duration<double>(1.0 / log_rate_hz_),
            std::bind(&ObstaclePresenceLogger::logState, this));

        param_cb_ = add_on_set_parameters_callback(
            std::bind(&ObstaclePresenceLogger::paramCallback, this, std::placeholders::_1));
    }

private:
    std::string cloud_topic_;
    double min_x_, max_x_, half_w_, side_thresh_, log_rate_hz_;
    bool obstacle_detected_;
    float obs_x_{0.0f}, obs_y_{0.0f};

    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_sub_;
    rclcpp::TimerBase::SharedPtr log_timer_;
    rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_cb_;

    void loadParams()
    {
        cloud_topic_ = get_parameter("cloud_topic").as_string();
        min_x_ = get_parameter("min_x").as_double();
        max_x_ = get_parameter("max_x").as_double();
        half_w_ = get_parameter("half_width").as_double();
        side_thresh_ = get_parameter("side_thresh").as_double();
        log_rate_hz_ = get_parameter("log_rate_hz").as_double();
    }

    rcl_interfaces::msg::SetParametersResult
    paramCallback(const std::vector<rclcpp::Parameter>&)
    {
        loadParams();
        rcl_interfaces::msg::SetParametersResult result;
        result.successful = true;
        return result;
    }

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

        obstacle_detected_ = found;
    }

    void logState()
    {
        if (obstacle_detected_)
        {
            const float dist = std::hypot(obs_x_, obs_y_);
            const char* side =
                (obs_y_ > side_thresh_) ? "LEFT" :
                (obs_y_ < -side_thresh_) ? "RIGHT" : "CENTER";

            RCLCPP_INFO(
                get_logger(),
                "[OBS] DETECTED | x=%.2f y=%.2f dist=%.2f side=%s",
                obs_x_, obs_y_, dist, side);
        }
        else
        {
            RCLCPP_INFO(get_logger(), "[OBS] CLEAR");
        }
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ObstaclePresenceLogger>());
    rclcpp::shutdown();
    return 0;
}
