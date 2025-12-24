#ifndef IRC_PLANNER_H_
#define IRC_PLANNER_H_

#include <rclcpp/rclcpp.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <mutex>
#include <memory>

#include "sensor_msgs/msg/nav_sat_fix.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "geometry_msgs/msg/twist.hpp"

#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/bool.hpp"
#include "std_srvs/srv/trigger.hpp"

#include "custom_msgs/msg/marker_tag.hpp"
#include "custom_msgs/msg/imu_data.hpp"
#include "custom_msgs/msg/gui_command.hpp"
#include "custom_msgs/msg/planner_status.hpp"

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>

namespace planner
{

// Constants 

const double kRoverLength  = 1.50;
const double kRoverBreadth = 1.25;

const double kMaxLinearVel  = 0.6;
const double kMinLinearVel  = 0.0;
const double kMaxAngularVel = 0.85;


const double kMaxObsThreshold = 3.0;
const double kMinObsThreshold = 0.5;


const double kMaxXObsDistThreshold = 2.0;
const double kMinXObsDistThreshold = 1.0;
const double kMaxYObjDistThreshold = 2.0;
const double kMinYObsThreshold = 0.0;
const double kMinYObjDistThreshold = 0.0;

const double kStopVel = 0.0;
const double kDistanceThreshold = 2.0;

// All State Machines

enum State
{
    kManualState,
    kNavigationModeSelect,
    kCoordinateFollowing,
    kSearchPattern,
    kConeFollowing,
    kObstacleAvoidance,
    kObjectDelivery
};

enum SearchPatternType
{
    kFaceForward,
    kMoveForward,
    kTurnRight,
    kTurnLeft,
};

enum SearchSkew
{
    kNoSkew    = -1,
    kLeftSkew  = 0,
    kRightSkew = 1
};

struct Coordinates
{
    double latitude;
    double longitude;
};

class SensorCallback : public rclcpp::Node
{
public:
    SensorCallback();

private:
    // Publishers 
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr vel_pub;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr arm_pub;
    rclcpp::Publisher<custom_msgs::msg::PlannerStatus>::SharedPtr status_pub_;

    // Subscribers
    rclcpp::Subscription<custom_msgs::msg::ImuData>::SharedPtr imu_sub_;
    rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr gps_sub_;
    rclcpp::Subscription<custom_msgs::msg::MarkerTag>::SharedPtr cone_sub_;
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr pcl_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr auto_sub_;
    rclcpp::Subscription<custom_msgs::msg::GuiCommand>::SharedPtr gui_sub_;

    // Services & Timers
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr toggle_client_;
    rclcpp::TimerBase::SharedPtr stack_timer_;

    State CurrState;
    State PrevState;
    SearchPatternType FollowPattern;

    // Various Flags
    bool nav_selected;
    bool gps_goal_set;
    bool cone_detect;
    bool gps_goal_reached;
    bool cone_goal_reached;
    bool obstacle_detect;
    bool rover_state;
    bool last_rover_state;
    bool gps_aligned_;
    bool delivery_active_;
    rclcpp::Time delivery_start_time_;

    // Navigation Variables
    int nav_mode;
    int target_cone_id_;
    bool nav_select_done_;
    double offset_accum_;

    // Measurements
    double current_orientation;
    double cone_x;
    double cone_y;
    double obs_x;
    double obs_y;
    
    SearchSkew search_skew;


    Coordinates curr_location;
    Coordinates goal_location;

    rclcpp::Time last_gps_time_;

    std::vector<double> obs_avoid_linear;
    std::vector<double> obs_avoid_angular;
    std::vector<double> obj_follow_linear;
    std::vector<double> obj_follow_angular;

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud;

    std::mutex state_mutex_;

    // Core Functions
    void stackRun();
    void RoverStateClassifier();

    // Callbacks
    void imuCallback(const custom_msgs::msg::ImuData::SharedPtr imu_msg);
    void gpsCallback(const sensor_msgs::msg::NavSatFix::SharedPtr fix);
    void pclCallback(const sensor_msgs::msg::PointCloud2::SharedPtr cloud_msg);
    void coneCallback(const custom_msgs::msg::MarkerTag::SharedPtr cone);
    void stateCallback(const std_msgs::msg::Bool::SharedPtr state);
    void guiCommandCallback(const custom_msgs::msg::GuiCommand::SharedPtr msg);

    // State Functions
    void coordinateFollowing();
    void obstacleAvoidance();
    void objectFollowing();
    void callSearchPattern();
    void objectDelivery();
    void navigationModeSelect();

    // Helper Functions
    void publishVel(const geometry_msgs::msg::Twist& msg);
    void hardStop();
    void disableAutonomous();
    void obstacleClassifier();
    void setGoalStatus();
    void setSearchSkew(int skew);

    // Math Functions 
    std::vector<double> straightLineEquation(double x1, double y1,double x2, double y2);

    double haversine(Coordinates curr, Coordinates dest);
    double gpsBearing(Coordinates curr, Coordinates dest);
    double gpsAngleFix(double angle);
    double headingError(double target, double current);
    double normalize360(double angle);
};

} 

#endif  
