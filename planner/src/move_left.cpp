#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

class MoveLeft : public rclcpp::Node {
public:
    MoveLeft() : Node("move_left"), start_(now()) {
        pub_ = create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
        timer_ = create_wall_timer(20ms, std::bind(&MoveLeft::run, this));
    }

private:
    void run() {
        geometry_msgs::msg::Twist cmd;
        if ((now() - start_).seconds() < 3.0) {
            cmd.linear.x = 0.8;
            cmd.angular.z = 0.8;
        }
        pub_->publish(cmd);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Time start_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MoveLeft>());
    rclcpp::shutdown();
}
