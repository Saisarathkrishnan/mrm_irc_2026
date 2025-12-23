#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/bool.hpp"
#include "std_srvs/srv/trigger.hpp"

using namespace std::chrono_literals;

static constexpr float track_width = 1.005f;
static constexpr float wheel_diameter = 0.30f;
static constexpr float max_wheel_RPM = 100.0f;

static constexpr const char* ESP_IP = "10.0.0.7";
static constexpr int ESP_PORT = 5005;

static constexpr int LISTEN_PORT = 5010;

class RelayNode : public rclcpp::Node {
public:
    RelayNode() : Node("relay_node_udp_bridge") {
        RCLCPP_INFO(this->get_logger(), "RelayNode starting...");

        cmd_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel", 10, std::bind(&RelayNode::cmdVelCallback, this, std::placeholders::_1));

        arm_sub_ = this->create_subscription<std_msgs::msg::String>(
            "/arm_vel", 10, std::bind(&RelayNode::armCallback, this, std::placeholders::_1));

        mode_cmd_sub_ = this->create_subscription<std_msgs::msg::Bool>(
            "/autonomous_mode_cmd", 10, std::bind(&RelayNode::modeCmdCallback, this, std::placeholders::_1));

        mode_state_pub_ = this->create_publisher<std_msgs::msg::Bool>("/autonomous_mode_state", 10);
        mode_timer_ = this->create_wall_timer(200ms, std::bind(&RelayNode::publishModeState, this));

        toggle_srv_ = this->create_service<std_srvs::srv::Trigger>(
            "/toggle_autonomous",
            std::bind(&RelayNode::toggleAutonomousService, this, std::placeholders::_1, std::placeholders::_2));

        listener_thread_ = std::thread(&RelayNode::udpListenerThread, this);
        sender_thread_ = std::thread(&RelayNode::udpSenderThread, this);

        RCLCPP_INFO(this->get_logger(), "RelayNode fully initialized.");
    }

    ~RelayNode() {
        RCLCPP_WARN(this->get_logger(), "Shutting down RelayNode...");
        running_.store(false);
        if (listener_thread_.joinable()) listener_thread_.join();
        if (sender_thread_.joinable()) sender_thread_.join();
        RCLCPP_INFO(this->get_logger(), "RelayNode shutdown complete.");
    }

private:
    void toggleAutonomousService(
        const std::shared_ptr<std_srvs::srv::Trigger::Request>,
        std::shared_ptr<std_srvs::srv::Trigger::Response> response)
    {
        bool new_mode = !autonomous_mode_.load();
        autonomous_mode_.store(new_mode);

        response->success = true;
        response->message = new_mode ? "Autonomous ON" : "Autonomous OFF";

        RCLCPP_INFO(this->get_logger(), "Autonomous mode toggled: %s", new_mode ? "ON" : "OFF");
    }

    void cmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg) {
        if (!autonomous_mode_.load()) {
            return;
        }

        float linear_vel = msg->linear.x * 1.2f;
        float angular_vel = -msg->angular.z * 1.2f;

        float max_linear_velocity = ((static_cast<float>(M_PI) * wheel_diameter) * max_wheel_RPM) / 60.0f;

        linear_vel = std::clamp(linear_vel, -max_linear_velocity, max_linear_velocity);

        float maxAngularVel = (max_linear_velocity - std::fabs(linear_vel)) * 2.0f / track_width;
        angular_vel = std::clamp(angular_vel, -maxAngularVel, maxAngularVel);

        float right_vel = std::fabs(linear_vel + angular_vel * track_width / 2.0f);
        float left_vel  = std::fabs(linear_vel - angular_vel * track_width / 2.0f);

        float right_rpm = right_vel * 60.0f / (wheel_diameter * static_cast<float>(M_PI));
        float left_rpm  = left_vel  * 60.0f / (wheel_diameter * static_cast<float>(M_PI));

        float right_pct = std::min(right_rpm / max_wheel_RPM * 100.0f, 99.0f);
        float left_pct  = std::min(left_rpm  / max_wheel_RPM * 100.0f, 99.0f);

        int direction;
        float rtest = linear_vel + angular_vel * track_width / 2.0f;
        float ltest = linear_vel - angular_vel * track_width / 2.0f;

        if (rtest > 0.0f && ltest > 0.0f) direction = 1;
        else if (rtest < 0.0f && ltest < 0.0f) direction = 2;
        else if (rtest < 0.0f && ltest > 0.0f) direction = 3;
        else direction = 4;

        std::string packet;
        if (direction == 1)
            packet = "L" + std::to_string(static_cast<int>(left_pct)) + "R" + std::to_string(static_cast<int>(right_pct)) + "E";
        else if (direction == 2)
            packet = "L-" + std::to_string(static_cast<int>(left_pct)) + "R-" + std::to_string(static_cast<int>(right_pct)) + "E";
        else if (direction == 3)
            packet = "L-" + std::to_string(static_cast<int>(left_pct)) + "R" + std::to_string(static_cast<int>(right_pct)) + "E";
        else
            packet = "L" + std::to_string(static_cast<int>(left_pct)) + "R-" + std::to_string(static_cast<int>(right_pct)) + "E";

        {
            std::lock_guard<std::mutex> lock(mtx_);
            last_motor_packet_ = packet;
        }

        RCLCPP_INFO(this->get_logger(), "[AUTONOMOUS] Motor packet updated: %s", packet.c_str());
    }

    void armCallback(const std_msgs::msg::String::SharedPtr msg) {
        if (!autonomous_mode_.load()) {
            return;
        }

        {
            std::lock_guard<std::mutex> lock(mtx_);
            last_arm_packet_ = msg->data;
        }

        RCLCPP_INFO(this->get_logger(), "[AUTONOMOUS] Arm packet received: %s", msg->data.c_str());
    }

    void modeCmdCallback(const std_msgs::msg::Bool::SharedPtr msg) {
        autonomous_mode_.store(msg->data);
        // Neutral log for mode set
        RCLCPP_INFO(this->get_logger(), "Autonomous mode set to: %s", msg->data ? "ON" : "OFF");
    }

    void publishModeState() {
        auto m = std::make_shared<std_msgs::msg::Bool>();
        m->data = autonomous_mode_.load();
        mode_state_pub_->publish(*m);
    }

    void udpListenerThread() {
        RCLCPP_INFO(this->get_logger(), "Starting UDP listener on port %d", LISTEN_PORT);

        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            RCLCPP_ERROR(this->get_logger(), "UDP socket creation failed.");
            return;
        }

        sockaddr_in serv{}, cli{};
        serv.sin_family = AF_INET;
        serv.sin_addr.s_addr = INADDR_ANY;
        serv.sin_port = htons(LISTEN_PORT);

        if (bind(sockfd, (sockaddr*)&serv, sizeof(serv)) < 0) {
            RCLCPP_ERROR(this->get_logger(), "UDP bind failed.");
            close(sockfd);
            return;
        }

        char buf[1500];
        socklen_t len = sizeof(cli);

        while (rclcpp::ok() && running_.load()) {
            ssize_t n = recvfrom(sockfd, buf, sizeof(buf), 0, (sockaddr*)&cli, &len);
            if (n > 0) {
                std::string s(buf, n);
                if (!autonomous_mode_.load()) {
                    std::lock_guard<std::mutex> lock(mtx_);
                    last_manual_packet_ = s;
                    RCLCPP_INFO(this->get_logger(), "[MANUAL] Received manual packet: %s", s.c_str());
                }
            }
        }

        close(sockfd);
        RCLCPP_WARN(this->get_logger(), "UDP listener stopped.");
    }

    void udpSenderThread() {
        RCLCPP_INFO(this->get_logger(), "Starting UDP sender to %s:%d", ESP_IP, ESP_PORT);

        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            RCLCPP_ERROR(this->get_logger(), "UDP sender socket creation failed.");
            return;
        }

        sockaddr_in esp{};
        esp.sin_family = AF_INET;
        esp.sin_port = htons(ESP_PORT);
        if (inet_pton(AF_INET, ESP_IP, &esp.sin_addr) != 1) {
            RCLCPP_ERROR(this->get_logger(), "Invalid ESP IP address: %s", ESP_IP);
            close(sockfd);
            return;
        }

        while (rclcpp::ok() && running_.load()) {
            std::string packet;
            bool mode = autonomous_mode_.load();

            {
                std::lock_guard<std::mutex> lock(mtx_);

                if (!mode) {
                    if (last_motor_packet_.empty())
                        last_motor_packet_ = "L0R0E";

                    packet = last_manual_packet_ + "|" + last_motor_packet_ + "|Z0";
                    // send manual packet
                    sendto(sockfd, packet.c_str(), packet.size(), 0, (sockaddr*)&esp, sizeof(esp));
                    RCLCPP_INFO(this->get_logger(), "[MANUAL] Sent UDP packet: %s", packet.c_str());
                } else {
                    std::string motor = last_motor_packet_.empty() ? "L0R0E" : last_motor_packet_;
                    if (last_arm_packet_.empty())
                        packet = motor + "|Z1";
                    else
                        packet = last_arm_packet_ + motor + "|Z1";

                    // send autonomous packet
                    sendto(sockfd, packet.c_str(), packet.size(), 0, (sockaddr*)&esp, sizeof(esp));
                    RCLCPP_INFO(this->get_logger(), "[AUTONOMOUS] Sent UDP packet: %s", packet.c_str());
                }
            }

            std::this_thread::sleep_for(50ms);
        }

        close(sockfd);
        RCLCPP_WARN(this->get_logger(), "UDP sender stopped.");
    }

    std::atomic<bool> running_{true};
    std::atomic<bool> autonomous_mode_{false};

    std::thread listener_thread_;
    std::thread sender_thread_;

    std::mutex mtx_;
    std::string last_manual_packet_ = "M0X0Y0P0Q0A0S0J0DE";
    std::string last_motor_packet_  = "L0R0E";
    std::string last_arm_packet_    = "";

    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr arm_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr mode_cmd_sub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr mode_state_pub_;
    rclcpp::TimerBase::SharedPtr mode_timer_;
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr toggle_srv_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<RelayNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
