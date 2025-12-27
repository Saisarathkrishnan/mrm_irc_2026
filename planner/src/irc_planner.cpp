#include "stack/irc_planner.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace planner
{

// Constructorr

SensorCallback::SensorCallback()
: Node("planner_node"),
  CurrState(kManualState),
  PrevState(kManualState),
  FollowPattern(kTurnRight),
  nav_selected(false),
  gps_goal_set(false),
  cone_detect(false),
  gps_goal_reached(false),
  cone_goal_reached(false),
  obstacle_detect(false),
  rover_state(false),
  last_rover_state(false),
  gps_aligned_(false),
  delivery_active_(false),
  delivery_start_time_(this->get_clock()->now()),
  nav_mode(-1),
  target_cone_id_(0),
  nav_select_done_(false),
  current_orientation(0.0),
  cone_x(0.0),
  cone_y(0.0),
  obs_x(0.0),
  obs_y(0.0),
  search_init_(false),
  search_timing_(false),
  search_base_heading_(0.0),
  search_offset_deg_(0.0),
  search_skew(kNoSkew),
  curr_location{0.0, 0.0},
  goal_location{0.0, 0.0},
  locked_bearing_deg_(0.0),
  bearing_locked_(false),
  last_gps_time_(this->get_clock()->now()),
  obs_avoid_linear(),
  obs_avoid_angular(),
  obj_follow_linear(),
  obj_follow_angular(),

  cloud(std::make_shared<pcl::PointCloud<pcl::PointXYZ>>())
{
    // Time
    this->set_parameter(rclcpp::Parameter("use_sim_time", false));

    // Topics & Parameters
    declare_parameter("imu_topic", "/imu_data");
    declare_parameter("gps_topic", "/fix");
    declare_parameter("cone_topic", "/marker_detect");
    declare_parameter("point_cloud_topic", "/obstacles");
    declare_parameter("cmd_vel_topic", "/cmd_vel");
    declare_parameter("arm_topic", "/arm_vel");
    declare_parameter("state_topic", "/autonomous_mode_state");
    declare_parameter("target_cone_id", 1);

    const auto imu_topic   = get_parameter("imu_topic").as_string();
    const auto gps_topic   = get_parameter("gps_topic").as_string();
    const auto cone_topic  = get_parameter("cone_topic").as_string();
    const auto cloud_topic = get_parameter("point_cloud_topic").as_string();
    const auto cmd_vel     = get_parameter("cmd_vel_topic").as_string();
    const auto arm_topic   = get_parameter("arm_topic").as_string();
    const auto state_topic = get_parameter("state_topic").as_string();

    target_cone_id_ = get_parameter("target_cone_id").as_int();

    // Publishers
    vel_pub = create_publisher<geometry_msgs::msg::Twist>(cmd_vel, 10);
    arm_pub = create_publisher<std_msgs::msg::String>(arm_topic, 10);
    status_pub_ = create_publisher<custom_msgs::msg::PlannerStatus>("/planner/status", 10);

    // Subscribers
    imu_sub_ = create_subscription<custom_msgs::msg::ImuData>(imu_topic, 10,std::bind(&SensorCallback::imuCallback, this, std::placeholders::_1));
    gps_sub_ = create_subscription<sensor_msgs::msg::NavSatFix>(gps_topic, 10,std::bind(&SensorCallback::gpsCallback, this, std::placeholders::_1));
    cone_sub_ = create_subscription<custom_msgs::msg::MarkerTag>(cone_topic, 10,std::bind(&SensorCallback::coneCallback, this, std::placeholders::_1));
    pcl_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(cloud_topic, 10,std::bind(&SensorCallback::pclCallback, this, std::placeholders::_1));
    auto_sub_ = create_subscription<std_msgs::msg::Bool>(state_topic, 10,std::bind(&SensorCallback::stateCallback, this, std::placeholders::_1));
    //gui_sub_ = create_subscription<custom_msgs::msg::GuiCommand>("/gui/command", 10,std::bind(&SensorCallback::guiCommandCallback, this, std::placeholders::_1));

    // Timers & Services
    stack_timer_ = create_wall_timer(std::chrono::milliseconds(50),std::bind(&SensorCallback::stackRun, this));
    toggle_client_ = create_client<std_srvs::srv::Trigger>("/toggle_autonomous");
    last_gps_time_ = this->get_clock()->now();

    // Search-Related 
    gps_aligned_ = false;
    last_cone_time_ = this->get_clock()->now();

    // Random Equations
    obs_avoid_linear =straightLineEquation(kMinObsThreshold, kStopVel,kMaxObsThreshold, kMaxLinearVel);
    obs_avoid_angular =straightLineEquation(kRoverBreadth / 2.0, kStopVel,kMinYObsThreshold, kMaxAngularVel);
    obj_follow_linear =straightLineEquation(kMaxXObsDistThreshold, kMaxLinearVel,kMinXObsDistThreshold, kStopVel);
    obj_follow_angular =straightLineEquation(kMinYObjDistThreshold, kStopVel,kMaxYObjDistThreshold, kMaxAngularVel);
}


// Core Functions :

// Planner Main control loop
void SensorCallback::stackRun()
{
    std::lock_guard<std::mutex> lock(state_mutex_);

    auto now = this->get_clock()->now();

    if (cone_detect && !isConeFresh())
    {
        cone_detect = false;
    }

    // Publish status regardless of motion
    custom_msgs::msg::PlannerStatus s;
    s.state = CurrState;
    s.nav_mode = nav_mode;
    s.curr_lat = curr_location.latitude;
    s.curr_lon = curr_location.longitude;
    s.goal_lat = goal_location.latitude;
    s.goal_lon = goal_location.longitude;
    s.current_yaw_deg = current_orientation;
    s.cone_detected = cone_detect;
    s.cone_x = cone_x;
    s.cone_y = cone_y;
    s.target_cone_id = target_cone_id_;
    s.obstacle_detected = obstacle_detect;
    s.gps_goal_reached = gps_goal_reached;
    s.cone_goal_reached = cone_goal_reached;
    s.autonomous_enabled = rover_state;

    if (gps_goal_set)
    {
        s.distance_to_goal_m = haversine(curr_location, goal_location);
        s.target_yaw_deg = gpsAngleFix(gpsBearing(curr_location, goal_location));
        s.heading_error_deg = headingError(s.target_yaw_deg, current_orientation);
    }
    else
    {
        s.distance_to_goal_m = -1.0;
        s.target_yaw_deg = 0.0;
        s.heading_error_deg = 0.0;
    }

    s.stamp = now;
    status_pub_->publish(s);

    if (!rover_state)
    {
        publishVel(geometry_msgs::msg::Twist());
        return;
    }

    if (CurrState == kNavigationModeSelect)
    {
        navigationModeSelect();
        return;   
    }

    // GPS guard
    if (nav_mode == 0 && !gps_goal_set)
    {
        publishVel(geometry_msgs::msg::Twist());
        return;
    }

    // Cone guard — ONLY block SEARCH
    // FIX: NO SKEW is valid, do NOT block on search_skew
    if (nav_mode == 1 &&
        CurrState == kSearchPattern &&
        target_cone_id_ <= 0)
    {
        publishVel(geometry_msgs::msg::Twist());
        return;
    }

    RoverStateClassifier();
    setGoalStatus();

    switch (CurrState)
    {
        case kSearchPattern:
            callSearchPattern();
            break;

        case kConeFollowing:
            objectFollowing();
            break;

        case kObstacleAvoidance:
            obstacleAvoidance();
            break;

        case kCoordinateFollowing:
            coordinateFollowing();
            break;

        case kObjectDelivery:
            objectDelivery();
            break;

        default:
            publishVel(geometry_msgs::msg::Twist());
            break;
    }
}

// Planner state decision logic
void SensorCallback::RoverStateClassifier()
{
    if (CurrState == kObjectDelivery)
        return;

    if ((nav_mode == 0 && gps_goal_reached) ||
        (nav_mode == 1 && cone_goal_reached))
    {
        CurrState = kObjectDelivery;
        delivery_start_time_ = this->get_clock()->now();
        return;
    }

    if (obstacle_detect)
    {
        if (CurrState != kObstacleAvoidance)
            PrevState = CurrState;
        CurrState = kObstacleAvoidance;
        return;
    }

    if (CurrState == kObstacleAvoidance && !obstacle_detect)
    {
        CurrState =
            (PrevState == kCoordinateFollowing ||
             PrevState == kConeFollowing ||
             PrevState == kSearchPattern)
                ? PrevState
                : (nav_mode == 0 ? kCoordinateFollowing : kSearchPattern);
        return;
    }

    if (nav_mode == 0)
    {
        if (gps_goal_set && !gps_goal_reached)
            CurrState = kCoordinateFollowing;
        return;
    }

    if (nav_mode == 1)
    {
        if (CurrState == kSearchPattern && isConeFresh())
        {
            CurrState = kConeFollowing;
            return;
        }

        if (CurrState == kConeFollowing && !isConeFresh())
        {
            CurrState = kSearchPattern;
            return;
        }
    }
}


// Callbacks:-

void SensorCallback::imuCallback(const custom_msgs::msg::ImuData::SharedPtr imu_msg)
{
    std::lock_guard<std::mutex> lock(state_mutex_);
    current_orientation = imu_msg->orientation.z;
}

void SensorCallback::gpsCallback(const sensor_msgs::msg::NavSatFix::SharedPtr fix_)
{
    if (fix_->status.status < sensor_msgs::msg::NavSatStatus::STATUS_FIX) // STATUS_FIX means at least a 2D GPS fix
        return;

    std::lock_guard<std::mutex> lock(state_mutex_);

    curr_location.latitude  = fix_->latitude;
    curr_location.longitude = fix_->longitude;
    last_gps_time_ = this->get_clock()->now();
}


void SensorCallback::pclCallback(const sensor_msgs::msg::PointCloud2::SharedPtr cloud_msg)
{
    if (!rover_state)
        return;

    std::lock_guard<std::mutex> lock(state_mutex_); // To make sure only one thread is using the protected variables :/

    pcl::PointCloud<pcl::PointXYZ>::Ptr temp(
        new pcl::PointCloud<pcl::PointXYZ>);

    pcl::fromROSMsg(*cloud_msg, *temp);
    cloud.swap(temp);
    obstacleClassifier();
}

void SensorCallback::coneCallback(const custom_msgs::msg::MarkerTag::SharedPtr cone)
{
    if (!rover_state)
        return;

    std::lock_guard<std::mutex> lock(state_mutex_);

    if (cone->is_found && cone->id == target_cone_id_)
    {
        cone_detect = true;
        cone_x = cone->x;
        cone_y = cone->y;
        last_cone_time_ = this->get_clock()->now();
    }
}

void SensorCallback::stateCallback(const std_msgs::msg::Bool::SharedPtr state)
{
    std::lock_guard<std::mutex> lock(state_mutex_);

    if (state->data == last_rover_state)
        return;

    last_rover_state = state->data;
    rover_state      = state->data;

    if (!rover_state)
    {
        publishVel(geometry_msgs::msg::Twist());

        CurrState = kManualState;
        PrevState = kManualState;

        nav_mode = -1;
        gps_goal_set = false;
        gps_goal_reached  = false;
        cone_goal_reached = false;
        gps_aligned_      = false;

        cone_detect = false;
        obstacle_detect = false;

        nav_select_done_ = false;
        delivery_start_time_ = this->get_clock()->now();

        /* SEARCH + IMU RESET ON MANUAL */
        resetSearchPattern();
        search_skew = kNoSkew;
        last_cone_time_ = this->get_clock()->now();

        RCLCPP_WARN(this->get_logger(), "[MODE] MANUAL MODE");
        return;
    }

    CurrState = kNavigationModeSelect;
    PrevState = kManualState;

    nav_select_done_ = false;

    gps_goal_set      = false;
    gps_goal_reached  = false;
    cone_goal_reached = false;
    delivery_start_time_ = this->get_clock()->now();


    cone_detect = false;
    obstacle_detect = false;

    gps_aligned_ = false;
    last_gps_time_ = this->get_clock()->now();

    /* HARD RESET ON AUTONOMOUS ENTRY */
    resetSearchPattern();
    search_skew = kNoSkew;
    last_cone_time_ = this->get_clock()->now();

    RCLCPP_INFO(this->get_logger(),
        "[MODE] AUTONOMOUS MODE → NAVIGATION SELECT");
}


/*void SensorCallback::guiCommandCallback(const custom_msgs::msg::GuiCommand::SharedPtr msg)
{
    std::lock_guard<std::mutex> lock(state_mutex_);
    nav_mode = msg->nav_mode;

    if (nav_mode != 1) {
        search_skew = kNoSkew;
    }

    if (nav_mode == 0 && msg->goal_lat != 0.0 && msg->goal_lon != 0.0) {
        goal_location.latitude  = msg->goal_lat;
        goal_location.longitude = msg->goal_lon;
        gps_goal_set = true;
        gps_goal_reached = false;
        gps_aligned_ = false;
    }

    if (nav_mode == 1) {
        target_cone_id_ = msg->target_cone_id;

        if (msg->set_search_skew) {
            setSearchSkew(msg->search_skew);
        }
    }
}
*/
// States : 

// Aligns to GPS goal and drives toward it until the target is reached.

// Used for selection of the navigation mode and other variables
void SensorCallback::navigationModeSelect()
{
    if (nav_select_done_)
        return;

    publishVel(geometry_msgs::msg::Twist());

    std::cout << "\nNAVIGATION MODE SELECT\n";
    std::cout << "0 → GPS Navigation\n";
    std::cout << "1 → Cone Navigation\n";
    std::cout << "Select mode: ";
    std::cin >> nav_mode;

    if (nav_mode == 0)
    {
        std::cout << "Enter goal latitude  : ";
        std::cin >> goal_location.latitude;
        std::cout << "Enter goal longitude : ";
        std::cin >> goal_location.longitude;

        gps_goal_set = true;
        gps_goal_reached = false;
        gps_aligned_ = false;

        CurrState = kCoordinateFollowing;
        std::cout << "[CLI] GPS navigation selected\n";
    }
    else if (nav_mode == 1)
    {
        std::cout << "\nCONE COLOR → ID MAP\n";
        std::cout << "1 → ORANGE\n";
        std::cout << "2 → RED\n";
        std::cout << "3 → BLUE\n";
        std::cout << "4 → GREEN\n";
        std::cout << "5 → YELLOW\n";

        std::cout << "Enter target cone ID : ";
        std::cin >> target_cone_id_;

        std::cout << "Search skew (-1 = NONE, 0 = LEFT, 1 = RIGHT): ";
        int skew;
        std::cin >> skew;

        setSearchSkew(skew);

        cone_detect = false;
        cone_goal_reached = false;
        obstacle_detect = false;

        resetSearchPattern();

        CurrState = kSearchPattern;
        PrevState = kSearchPattern;

        std::cout << "[CLI] Cone navigation selected → SEARCH\n";
    }
    else
    {
        CurrState = kManualState;
        std::cout << "[CLI] Invalid selection → MANUAL\n";
    }

    nav_select_done_ = true;
}

void SensorCallback::coordinateFollowing()
{
    if (!gps_goal_set)
    {
        publishVel(geometry_msgs::msg::Twist()); //Zero vel gng
        return;
    }

    auto now = this->get_clock()->now();

    if ((now - last_gps_time_).seconds() > 1.5)
    {
        gps_aligned_ = false;
        bearing_locked_ = false;
        publishVel(geometry_msgs::msg::Twist()); //stale gps
        return;
    }

    double dist = haversine(curr_location, goal_location);

    if (dist <= kDistanceThreshold)
    {
        gps_goal_reached = true;
        gps_aligned_ = false;
        bearing_locked_ = false;

        RCLCPP_INFO(this->get_logger(),
            "[GPS] Goal reached | remaining_dist=%.2f m", dist);

        hardStop();
        return;
    }

    geometry_msgs::msg::Twist cmd;

    // align phase
    if (!gps_aligned_)
    {
        if (!bearing_locked_)
        {
            locked_bearing_deg_ = gpsBearing(curr_location, goal_location);
            bearing_locked_ = true;

            RCLCPP_INFO(this->get_logger(),
                "[GPS][ALIGN] Bearing locked = %.2f deg", locked_bearing_deg_);
        }

        double err_deg = headingError(locked_bearing_deg_, current_orientation);
        double err_rad = err_deg * M_PI / 180.0;

        if (std::abs(err_deg) > 6.0)
        {
            cmd.linear.x  = 0.0;
            cmd.angular.z = std::clamp(
                err_rad * 1.8,
                -kMaxAngularVel, kMaxAngularVel);

            RCLCPP_INFO_THROTTLE(
                this->get_logger(), *this->get_clock(), 500,
                "[GPS][ALIGN] dist=%.2f | err=%.2f deg | ang=%.2f",
                dist, err_deg, cmd.angular.z);

            publishVel(cmd);
            return;
        }

        gps_aligned_ = true;
        bearing_locked_ = false;

        RCLCPP_INFO(this->get_logger(),
            "[GPS] Alignment complete | remaining_dist=%.2f m", dist);
    }

    //Reenter align if drifted off course
    double target_deg = gpsBearing(curr_location, goal_location);
    double err_deg = headingError(target_deg, current_orientation);

    if (gps_aligned_ && std::abs(err_deg) > 10.0)
    {
        gps_aligned_ = false;
        bearing_locked_ = false;
        return;
    }

    // Track Phasee
    cmd.linear.x = std::clamp(dist * 0.12, 0.2, kMaxLinearVel);

    double ang = (err_deg * M_PI / 180.0) * 1.0;
    if (std::abs(err_deg) < 3.0)
        ang = 0.0;

    cmd.angular.z = std::clamp(ang, -kMaxAngularVel, kMaxAngularVel);

    RCLCPP_INFO_THROTTLE(
        this->get_logger(), *this->get_clock(), 1000,
        "[GPS][TRACK] dist=%.2f | err=%.2f deg | lin=%.2f | ang=%.2f",
        dist, err_deg, cmd.linear.x, cmd.angular.z);

    publishVel(cmd);
}



// Function for obstacle avoidance 

void SensorCallback::obstacleAvoidance()
{
    geometry_msgs::msg::Twist cmd;

    cmd.linear.x = std::clamp(
        obs_avoid_linear[0] * obs_x + obs_avoid_linear[1],
        0.0, kMaxLinearVel);

    cmd.angular.z = std::clamp(
        obs_avoid_angular[0] * std::abs(obs_y) + obs_avoid_angular[1],
        0.4, kMaxAngularVel);

    cmd.angular.z = std::copysign(cmd.angular.z, -obs_y);

    publishVel(cmd);
}


// Follows the detected cone using its relative position.

void SensorCallback::objectFollowing()
{
    static double last_ang = 0.0;

    if (!isConeFresh() || cone_goal_reached)
    {
        last_ang = 0.0;
        publishVel(geometry_msgs::msg::Twist());
        return;
    }

    geometry_msgs::msg::Twist vel;

    double abs_y = std::abs(cone_y);
    double heading_scale = std::clamp(1.0 - abs_y * 1.8, 0.25, 1.0);

    vel.linear.x =
        std::clamp(
            (obj_follow_linear[0] * cone_x + obj_follow_linear[1]) * heading_scale,
            0.15, kMaxLinearVel);

    double ang = 0.0;
    if (abs_y > 0.03)
        ang = obj_follow_angular[0] * abs_y + obj_follow_angular[1];

    ang = std::clamp(ang, 0.0, kMaxAngularVel);
    ang = std::copysign(ang, cone_y);

    double max_delta = 0.08;
    ang = std::clamp(ang, last_ang - max_delta, last_ang + max_delta);
    last_ang = ang;

    vel.angular.z = ang;
    publishVel(vel);
}


// Controls how the rover searches when no cone is visible.
void SensorCallback::callSearchPattern()
{
    if (isConeFresh())
        return;

    geometry_msgs::msg::Twist cmd;

    auto turnTo = [&](double target)
    {
        double err = headingError(target, current_orientation);
        double abs_err = std::abs(err);

        if (abs_err < 5.0)
        {
            publishVel(geometry_msgs::msg::Twist());
            return true;
        }

        double ang = std::clamp(
            (kMaxAngularVel / 90.0) * abs_err,
            0.6,
            kMaxAngularVel
        );

        cmd.linear.x = 0.0;
        cmd.angular.z = std::copysign(ang, err);
        publishVel(cmd);
        return false;
    };

    if (!search_init_)
    {
        search_init_ = true;
        search_timing_ = false;
        search_offset_deg_ = 0.0;
        search_base_heading_ = current_orientation;
        FollowPattern = kTurnLeft;
        return;
    }

    double left  = search_base_heading_ + 90.0;
    double right = search_base_heading_ - 90.0;

    double offset = 0.0;
    if (search_skew == kLeftSkew)  offset =  search_offset_deg_;
    if (search_skew == kRightSkew) offset = -search_offset_deg_;

    double forward = search_base_heading_ + offset;

    switch (FollowPattern)
    {
        case kTurnLeft:
            if (!turnTo(left)) return;
            FollowPattern = kTurnRight;
            break;

        case kTurnRight:
            if (!turnTo(right)) return;
            FollowPattern = kFaceForward;
            break;

        case kFaceForward:
            if (!turnTo(forward)) return;
            FollowPattern = kMoveForward;
            break;

        case kMoveForward:
            if (!search_timing_)
            {
                search_end_time_ =
                    this->get_clock()->now() +
                    rclcpp::Duration::from_seconds(4.0);
                search_timing_ = true;
            }

            if (this->get_clock()->now() < search_end_time_)
            {
                cmd.linear.x = kMaxLinearVel;
                cmd.angular.z = 0.0;
                publishVel(cmd);
                return;
            }

            search_timing_ = false;

            if (search_skew != kNoSkew)
            {
                search_offset_deg_ =
                    (search_offset_deg_ == 0.0)
                        ? 15.0
                        : std::min(search_offset_deg_ + 10.0, 90.0);
            }

            search_base_heading_ = current_orientation;
            FollowPattern = kTurnLeft;
            break;
    }
}


// Have to change after imu's are put on by ECS
// Sends angles to the RM to move to a hard-coded position & switches mode to manual

void SensorCallback::objectDelivery()
{
    auto now = this->get_clock()->now();

    if (delivery_start_time_.nanoseconds() == 0)
    {
        delivery_start_time_ = now;

        std_msgs::msg::String msg;
        msg.data = "DROP";
        arm_pub->publish(msg);

        RCLCPP_WARN(this->get_logger(), "[DELIVERY] Drop command issued");
        return;
    }

    if ((now - delivery_start_time_).seconds() < 5.0)
    {
        publishVel(geometry_msgs::msg::Twist());
        return;
    }

    std_msgs::msg::String msg;
    msg.data = "STOP";
    arm_pub->publish(msg);

    hardStop();
    disableAutonomous();

    CurrState = kManualState;
    PrevState = kManualState;

    nav_mode = -1;
    nav_select_done_ = false;

    gps_goal_set = false;
    gps_goal_reached = false;
    cone_goal_reached = false;

    cone_detect = false;
    obstacle_detect = false;
    gps_aligned_ = false;

    resetSearchPattern();
    search_skew = kNoSkew;

    last_cone_time_ = this->get_clock()->now();
    last_gps_time_  = this->get_clock()->now();
    delivery_start_time_ = this->get_clock()->now();

    RCLCPP_WARN(this->get_logger(),
        "[MISSION] Delivery complete → MANUAL | FSM + IMU reset");
}

// Helpers : 

void SensorCallback::publishVel(const geometry_msgs::msg::Twist& msg)
{
    geometry_msgs::msg::Twist cmd = msg;

    cmd.linear.x = std::clamp(cmd.linear.x, 0.0, kMaxLinearVel);

    if (std::abs(cmd.angular.z) < 1e-3)
        cmd.angular.z = 0.0;
    else
        cmd.angular.z = std::clamp(cmd.angular.z, -kMaxAngularVel, kMaxAngularVel);

    vel_pub->publish(cmd);
}

void SensorCallback::hardStop()
{
    geometry_msgs::msg::Twist stop;
    stop.linear.x  = 0.0;
    stop.linear.y  = 0.0;
    stop.linear.z  = 0.0;
    stop.angular.x = 0.0;
    stop.angular.y = 0.0;
    stop.angular.z = 0.0;

    for (int i = 0; i < 5; ++i)
    {
        vel_pub->publish(stop);
        rclcpp::sleep_for(std::chrono::milliseconds(20));
    }
}

void SensorCallback::disableAutonomous()
{
    if (!toggle_client_->wait_for_service(std::chrono::seconds(1)))
    {
        RCLCPP_ERROR(this->get_logger(),"[MODE] toggle_autonomous service not available");
        return;
    }

    auto req = std::make_shared<std_srvs::srv::Trigger::Request>();
    toggle_client_->async_send_request(
        req,
        [this](rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future)
        {
            auto res = future.get();
            if (res->success)
            {
                RCLCPP_WARN(this->get_logger(),"[MODE] Autonomous DISABLED via service");
            }
            else
            {
                RCLCPP_ERROR(this->get_logger(),"[MODE] Failed to disable autonomy: %s",res->message.c_str());
            }
        });
}

void SensorCallback::obstacleClassifier()
{
    obstacle_detect = false;
    obs_x = kMaxObsThreshold + 1.0;
    obs_y = 0.0;

    if (!cloud || cloud->points.empty())
        return;

    const float x_min = kMinObsThreshold;
    const float x_max = kMaxObsThreshold;
    const float y_min = -(kRoverBreadth / 2.0 + 0.2);
    const float y_max =  (kRoverBreadth / 2.0 + 0.2);

    float closest_x = std::numeric_limits<float>::max();

    for (const auto& point : cloud->points)
    {
        if (point.x < x_min || point.x > x_max)
            continue;
        if (point.y < y_min || point.y > y_max)
            continue;

        if (cone_detect)
        {
            double dx = point.x - cone_x;
            double dy = point.y - cone_y;
            if (std::sqrt(dx*dx + dy*dy) < 0.4)
                continue;

        }

        if (point.x < closest_x)
        {
            closest_x = point.x;
            obs_x = point.x;
            obs_y = point.y;
            obstacle_detect = true;
        }
    }
}


// Checks if the goal has been reached or not
void SensorCallback::setGoalStatus()
{
    static rclcpp::Time close_since = this->get_clock()->now();
    static bool timing_active = false;

    if (nav_mode != 1 || !cone_detect || CurrState != kConeFollowing || cone_goal_reached)
    {
        timing_active = false;
        return;
    }

    // Cone must be continuously close AND visible
    if (cone_x <= kDistanceThreshold)
    {
        if (!timing_active)
        {
            close_since = this->get_clock()->now();
            timing_active = true;
        }

        // Require continuous visibility inside threshold
        if ((this->get_clock()->now() - close_since).seconds() > 0.8)
        {
            cone_goal_reached = true;
            RCLCPP_INFO(this->get_logger(),
                "Goal is reached - entering delivery state");
        }
    }
    else
    {
        // If cone moves away, reset timer
        timing_active = false;
    }
}


void SensorCallback::setSearchSkew(int skew)
{
    if (skew == kLeftSkew)
        search_skew = kLeftSkew;
    else if (skew == kRightSkew)
        search_skew = kRightSkew;
    else
        search_skew = kNoSkew;
}

void SensorCallback::resetSearchPattern()
{
    FollowPattern = kTurnLeft;
    search_init_ = false;
    search_timing_ = false;
    search_offset_deg_ = 0.0;
}

bool SensorCallback::isConeFresh() 
{
    return (this->get_clock()->now() - last_cone_time_).seconds() < 0.7;
}

// Math Functions :

// Computes the straight-line equation passing through two points (x1,y1) and (x2,y2)
std::vector<double> SensorCallback::straightLineEquation(double x1, double y1,double x2, double y2)
{
    std::vector<double> eq(2);
    if (std::abs(x2 - x1) < 1e-6)
    {
        eq[0] = 0.0;
        eq[1] = y1;
        return eq;
    }

    double m = (y2 - y1) / (x2 - x1);
    double c = y1 - m * x1;

    eq[0] = m;
    eq[1] = c;
    return eq;
}

// Computes the distance (in meters) between two GPS coordinates
double SensorCallback::haversine(Coordinates curr, Coordinates dest)
{
    double lat1 = curr.latitude * M_PI / 180.0;
    double lat2 = dest.latitude * M_PI / 180.0;
    double dLat = lat2 - lat1;
    double dLon = (dest.longitude - curr.longitude) * M_PI / 180.0;

    double h = sin(dLat*0.5)*sin(dLat*0.5) +
               cos(lat1)*cos(lat2)*sin(dLon*0.5)*sin(dLon*0.5);

    return 2.0 * 6371000.0 * asin(sqrt(h));
}

// Computes the initial bearing (in degrees) from the current GPS coordinate to the destination GPS coordinate, 
// measured clockwise from geographic North and normalized to the range [0, 360).
double SensorCallback::gpsBearing(Coordinates curr, Coordinates dest)
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
    if (angle < 0) angle += 360.0;

    return angle;
}

// Converts a GPS bearing (0° = North, clockwise positive) into the rover’s
// internal heading convention by applying a +90° frame shift, sign inversion,
// and wrapping the result to the range [-180, 180].
double SensorCallback::gpsAngleFix(double angle)
{
    double bearing = fmod((angle + 90.0), 360.0);
    double toAngle = -bearing;

    if (toAngle > 180.0)
        toAngle -= 360.0;
    if (toAngle < -180.0)
        toAngle += 360.0;

    return toAngle;
}

// Computes the shortest signed angular difference (in degrees) between a target
// heading and the current heading, normalized to the range [-180, 180] so that
// the result represents the minimal rotation direction and magnitude.
double SensorCallback::headingError(double target, double current)
{
    double diff = target - current;

    if (diff > 180.0)
        diff -= 360.0;
    if (diff < -180.0)
        diff += 360.0;

    return diff;
}

// Normalizes any input angle (in degrees) into the range [0, 360)
double SensorCallback::normalize360(double angle)
{
    angle = fmod(angle, 360.0);
    if (angle < 0.0)
        angle += 360.0;
    return angle;
}


} // namespace planner
