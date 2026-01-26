#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

class MoveForward : public rclcpp::Node {
public:
    MoveForward() : Node("move_forward"), start_(now()) {
        pub_ = create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
        timer_ = create_wall_timer(20ms, std::bind(&MoveForward::run, this));
    }

private:
    void run() {
        geometry_msgs::msg::Twist cmd;
        if ((now() - start_).seconds() < 3.0)
            cmd.linear.x = 1.0;
        pub_->publish(cmd);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Time start_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MoveForward>());
    rclcpp::shutdown();
}
