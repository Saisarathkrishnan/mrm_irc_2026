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
#include <sensor_msgs/point_cloud2_iterator.hpp>
#include "geometry_msgs/msg/twist.hpp"

#include "std_msgs/msg/bool.hpp"
#include "std_srvs/srv/trigger.hpp"

#include "custom_msgs/msg/marker_tag.hpp"
#include "custom_msgs/msg/imu_data.hpp"
#include "custom_msgs/msg/planner_status.hpp"

#include <sensor_msgs/msg/point_cloud2.hpp>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/segmentation/extract_clusters.h>
#include <pcl/search/kdtree.h>

namespace planner
{

constexpr double kRoverLength  = 1.50;
constexpr double kRoverBreadth = 1.25;

constexpr double kMaxLinearVel  = 1.0;
constexpr double kMaxAngularVel = 1.5;
constexpr double kDistanceThreshold = 2.0;


enum State
{
    kManualState,
    kNavigationModeSelect,
    kCoordinateFollowing,
    kSearchPattern,
    kConeFollowing,
    kObjectDelivery,
    kObstacleAvoidance
};

enum SearchPattern
{
    kTurnA,
    kTurnB,
    kTurnC,
    kMoveForward
};

enum SearchSkew
{
    kNoSkew    = 0,
    kLeftSkew  = -1,
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
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr vel_pub;
    rclcpp::Publisher<custom_msgs::msg::PlannerStatus>::SharedPtr status_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr deliver_pub_;

    rclcpp::Subscription<custom_msgs::msg::ImuData>::SharedPtr imu_sub_;
    rclcpp::Subscription<custom_msgs::msg::ImuData>::SharedPtr external_imu_sub_;
    rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr gps_sub_;
    rclcpp::Subscription<custom_msgs::msg::MarkerTag>::SharedPtr cone_sub_;
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr pcl_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr auto_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr delivered_sub_;

    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr toggle_client_;
    rclcpp::TimerBase::SharedPtr stack_timer_;

    State CurrState;
    SearchPattern FollowPattern;

    int nav_mode;
    int target_cone_id_;
    bool nav_select_done_;

    bool rover_state;
    bool last_rover_state;

    // GPS Related
    bool gps_goal_set;
    bool gps_goal_reached;
    bool gps_aligned_;
    Coordinates curr_location;
    Coordinates goal_location;
    rclcpp::Time last_gps_time_;

    // Cone Related
    bool cone_detect;
    bool cone_goal_reached;
    double cone_x;
    double cone_y;
    rclcpp::Time last_cone_time_;

    // Obstacle Related
    bool obstacle_detect;
    double obs_x;
    double obs_y;
    sensor_msgs::msg::PointCloud2::SharedPtr last_obstacle_cloud_;

    // Search Related
    bool search_ref_set_;
    bool spot_turn_back_;
    bool spot_done_;
    int search_cycle_;
    rclcpp::Time search_end_time_;
    double search_forward_time_;
    SearchSkew search_skew;
    bool avoiding_obstacle_;
    State prev_state_;
    SearchPattern prev_search_pattern_;


    // Delivery Related
    bool delivery_requested_;
    bool delivery_done_;
    rclcpp::Time delivery_start_time_;

    // Orientation
    double zed_yaw;
    double bno_yaw;
    double current_orientation;

    std::vector<double> obj_follow_linear;
    std::vector<double> obj_follow_angular;

    std::mutex state_mutex_;

    //idk bro
    rclcpp::Time last_cloud_time_;

    // Obstacle clearance timing
    bool obstacle_clear_timing_{false};
    rclcpp::Time obstacle_clear_since_;


    bool bearing_locked_;
    double locked_bearing_deg_;

    bool search_aligned_;
    bool search_timing_;
    double search_offset_deg_;


    void stackRun();
    void RoverStateClassifier();

    // Callbacks
    void imuCallback(const custom_msgs::msg::ImuData::SharedPtr msg);
    void externalImuCallback(const custom_msgs::msg::ImuData::SharedPtr msg);
    void gpsCallback(const sensor_msgs::msg::NavSatFix::SharedPtr fix);
    void pclCallback(const sensor_msgs::msg::PointCloud2::SharedPtr cloud_msg);
    void coneCallback(const custom_msgs::msg::MarkerTag::SharedPtr cone);
    void stateCallback(const std_msgs::msg::Bool::SharedPtr state);
    void deliveredCallback(const std_msgs::msg::Bool::SharedPtr msg);

    // Core States 
    void coordinateFollowing();
    void obstacleAvoidance();
    void objectFollowing();
    void callSearchPattern();
    void objectDelivery();
    void navigationModeSelect();

    // Helpers
    void publishVel(const geometry_msgs::msg::Twist& msg);
    void hardStop();
    void disableAutonomous();
    void obstacleClassifier();
    void setGoalStatus();
    void setSearchSkew(int skew);
    void resetSearchPattern();
    bool isConeFresh();

    std::vector<double> straightLineEquation(double x1, double y1, double x2, double y2);

    double haversine(Coordinates curr, Coordinates dest);
    double gpsBearing(Coordinates curr, Coordinates dest);
    double gpsAngleFix(double angle);
    double headingError(double target, double current);
    double normalize360(double angle);

    //
      sensor_msgs::msg::PointCloud2::SharedPtr att_latest_pcl;
      std::vector<std::vector<double>> obstacleDataType1;
      bool obstacleIs;


};

} // namespace planner

#endif
