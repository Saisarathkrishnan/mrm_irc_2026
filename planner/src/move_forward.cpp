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
        auto t = (now() - start_).seconds();
        geometry_msgs::msg::Twist cmd;

        if (t < 5.0) {
            cmd.linear.x = 3.0;
            pub_->publish(cmd);
            return;
        }

        pub_->publish(cmd);      // final stop
        timer_->cancel();        // stop callbacks
        rclcpp::shutdown();      // node dies
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Time start_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MoveForward>());
    return 0;
}
