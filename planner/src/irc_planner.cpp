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
  delivery_start_time_(rclcpp::Time(0)),
  nav_mode(-1),
  target_cone_id_(0),
  nav_select_done_(false),
  offset_accum_(0.0),
  current_orientation(0.0),
  cone_x(0.0),
  cone_y(0.0),
  obs_x(0.0),
  obs_y(0.0),
  search_skew(kNoSkew),
  curr_location{0.0, 0.0},
  goal_location{0.0, 0.0},
  last_gps_time_(rclcpp::Time(0)),
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
    if (nav_mode == 1 &&
        CurrState == kSearchPattern &&
        (target_cone_id_ <= 0 || search_skew == kNoSkew))
    {
        publishVel(geometry_msgs::msg::Twist());
        return;
    }

    setGoalStatus();
    RoverStateClassifier();

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
    // If delivery is in progress, do not allow any state transitions
    if (CurrState == kObjectDelivery)
        return;

    // If any goal is reached switch to delivery state 
    // (MODE-SAFE: prevents wrong or stale goal triggering delivery)
    if ((nav_mode == 0 && gps_goal_reached) ||
        (nav_mode == 1 && cone_goal_reached))
    {
        CurrState = kObjectDelivery;
        delivery_start_time_ = rclcpp::Time(0);
        return;
    }

    // Obstacle has highest priority during navigation :/
    if (obstacle_detect)
    {
        if (CurrState != kObstacleAvoidance)
            PrevState = CurrState;

        CurrState = kObstacleAvoidance;
        return;
    }

    // Resume previous state once obstacle is cleared
    if (CurrState == kObstacleAvoidance && !obstacle_detect)
    {
        CurrState = PrevState;
        return;
    }

    // GPS-based navigation
    if (nav_mode == 0)
    {
        if (gps_goal_set && !gps_goal_reached)
            CurrState = kCoordinateFollowing;
        return;
    }

    // Cone navigation state handling
    if (nav_mode == 1)
    {
        // Stay in SEARCH until a cone is actually detected
        if (CurrState == kSearchPattern)
        {
            if (cone_detect)
            {
                RCLCPP_INFO(this->get_logger(),
                    "[FSM] SEARCH → CONE_FOLLOW");
                CurrState = kConeFollowing;
            }
            return;
        }

        // If cone is lost during follow, return to SEARCH
        if (CurrState == kConeFollowing && !cone_detect)
        {
            RCLCPP_WARN(this->get_logger(),
                "[FSM] CONE LOST → SEARCH");
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
    last_gps_time_ = rclcpp::Time(fix_->header.stamp);
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

    if (cone->is_found)
    {
        if (cone->id == target_cone_id_)
        {
            cone_detect = true;
            cone_x = cone->x;
            cone_y = cone->y;
            return;
        }
    }

    cone_detect = false;
    cone_x = 100.0;
    cone_y = 100.0;
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
        delivery_start_time_ = rclcpp::Time(0);

        RCLCPP_WARN(this->get_logger(), "[MODE] MANUAL MODE");
        return;
    }

    CurrState = kNavigationModeSelect;
    PrevState = kManualState;

    nav_select_done_ = false;

    gps_goal_set      = false;
    gps_goal_reached  = false;
    cone_goal_reached = false;
    delivery_start_time_ = rclcpp::Time(0);

    cone_detect = false;
    obstacle_detect = false;

    gps_aligned_ = false;
    last_gps_time_ = this->get_clock()->now();

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

    std::cout << "\nNAVIGATION MODE SELECT \n";
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
        std::cout << "Enter target cone ID : ";
        std::cin >> target_cone_id_;

        std::cout << "Search skew (0 = LEFT, 1 = RIGHT): ";
        int skew;
        std::cin >> skew;

        setSearchSkew(skew);

        cone_detect = false;                
        cone_goal_reached = false;
        obstacle_detect = false;

        offset_accum_ = 0.0; 

        PrevState = kSearchPattern;
        CurrState = kSearchPattern;

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
        publishVel(geometry_msgs::msg::Twist());
        return;
    }

    auto now = this->get_clock()->now();

    if ((now - last_gps_time_).seconds() > 1.5)
    {
        gps_aligned_ = false;
        publishVel(geometry_msgs::msg::Twist());
        return;
    }

    double dist = haversine(curr_location, goal_location);
    double target_deg = gpsAngleFix(gpsBearing(curr_location, goal_location));
    double yaw_deg    = current_orientation;
    double err_deg    = headingError(target_deg, yaw_deg);
    double err_rad    = err_deg * M_PI / 180.0;

    if (dist <= kDistanceThreshold)
    {
        gps_goal_reached = true;
        gps_aligned_ = false;

        RCLCPP_INFO(this->get_logger(),"[GPS] Goal reached | remaining_dist=%.2f m",dist);
        hardStop();
        return;
    }

    geometry_msgs::msg::Twist cmd;

    if (!gps_aligned_)
    {
        if (std::abs(err_deg) > 6.0)
        {
            cmd.linear.x = 0.0;
            cmd.angular.z = std::copysign(std::max(0.4, std::abs(err_rad)),err_rad);
            cmd.angular.z = std::clamp(cmd.angular.z,-kMaxAngularVel,kMaxAngularVel);

            RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 500,"[GPS][ALIGN] dist=%.2f | err=%.2f deg | ang=%.2f",dist, err_deg, cmd.angular.z);
            publishVel(cmd);
            return;
        }
        gps_aligned_ = true;
        RCLCPP_INFO(this->get_logger(),"[GPS] Alignment complete | remaining_dist=%.2f m",dist);
    }

    cmd.linear.x = std::clamp(dist * 0.15,0.4,kMaxLinearVel);
    double ang = err_rad * 1.2;

    if (std::abs(ang) < 0.4)
        ang = std::copysign(0.4, ang);

    cmd.angular.z = std::clamp(ang,-kMaxAngularVel,kMaxAngularVel);

    RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 1000,"[GPS][TRACK] dist=%.2f | err=%.2f deg | lin=%.2f | ang=%.2f",dist, err_deg, cmd.linear.x, cmd.angular.z);
    publishVel(cmd);
}

// Function for obstacle avoidance 

void SensorCallback::obstacleAvoidance()
{
    geometry_msgs::msg::Twist cmd;
    cmd.linear.x  = std::clamp(obs_avoid_linear[0] * obs_x + obs_avoid_linear[1],0.0,kMaxLinearVel);
    cmd.angular.z = std::clamp(std::copysign(std::max(0.4, std::abs(obs_y * 2.0)),-obs_y),
                               -kMaxAngularVel, kMaxAngularVel);
    publishVel(cmd);
}

// Follows the detected cone using its relative position.

void SensorCallback::objectFollowing()
{
    if (cone_goal_reached)
    {
        publishVel(geometry_msgs::msg::Twist());
        return;
    }

    static rclcpp::Time last_seen = this->get_clock()->now();
    static bool was_tracking = false;

    auto now = this->get_clock()->now();
    geometry_msgs::msg::Twist vel;

    if (cone_detect)
    {
        last_seen = now;

        if (!was_tracking)
        {
            RCLCPP_INFO(this->get_logger(),
                "[CONE] Tracking started | x=%.2f y=%.2f",
                cone_x, cone_y);
            was_tracking = true;
        }
    }

    if ((now - last_seen).seconds() > 1.0)
    {
        was_tracking = false;
        publishVel(geometry_msgs::msg::Twist());
        return;
    }

    vel.linear.x = std::clamp(obj_follow_linear[0] * cone_x + obj_follow_linear[1],0.2, kMaxLinearVel);

    double abs_y = std::abs(cone_y);
    double ang = 0.0;

    if (abs_y > 0.05) 
    {
        ang = obj_follow_angular[0] * abs_y + obj_follow_angular[1];

        if (ang < 0.4)
            ang = 0.4;
    }

    vel.angular.z = std::copysign(
        std::clamp(ang, 0.0, kMaxAngularVel),
        cone_y);

    publishVel(vel);
}

// Controls how the rover searches when no cone is visible.
void SensorCallback::callSearchPattern()
{
    static bool init=false;
    static bool timing=false;
    static rclcpp::Time end_time;
    static double base_heading=0.0;
    static double offset_deg=0.0;

    if(search_skew==kNoSkew){
        RCLCPP_WARN_THROTTLE(this->get_logger(),*this->get_clock(),2000,"[SEARCH][BLOCKED] search_skew not set");
        publishVel(geometry_msgs::msg::Twist());
        return;
    }

    if(CurrState!=kSearchPattern){
        RCLCPP_INFO(this->get_logger(),"[SEARCH][EXIT] state=%d reset FSM",CurrState);
        init=false;timing=false;offset_deg=0.0;FollowPattern=kTurnRight;
        return;
    }

    geometry_msgs::msg::Twist cmd;
    auto now=this->get_clock()->now();
    double yaw=current_orientation;

    if(cone_detect)
    {
    RCLCPP_INFO(this->get_logger(),"[SEARCH][FOUND] cone detected → FOLLOW | yaw=%.2f",yaw);
    init=false;timing=false;offset_deg=0.0;
    return;
    }   

    if(obstacle_detect){
        RCLCPP_WARN_THROTTLE(this->get_logger(),*this->get_clock(),1000,"[SEARCH][PAUSE] obstacle detected");
        return;
    }

    if(!init){
        init=true;timing=false;
        base_heading=yaw;
        offset_deg=0.0;
        FollowPattern=(search_skew==kLeftSkew)?kTurnRight:kTurnLeft;

        RCLCPP_INFO(this->get_logger(),"[SEARCH][INIT] base=%.2f skew=%s first=%s",
            base_heading,
            (search_skew==kLeftSkew?"LEFT":"RIGHT"),
            (FollowPattern==kTurnRight?"TURN_RIGHT":"TURN_LEFT"));
        return;
    }

    auto turnTo=[&](double target){
        double err=headingError(target,yaw);
        if(std::abs(err)>5.0){
            cmd.angular.z=std::copysign(
                std::max(0.5,std::abs(err*0.02)),err);

            RCLCPP_INFO_THROTTLE(this->get_logger(),*this->get_clock(),500,
                "[SEARCH][TURN] target=%.2f yaw=%.2f err=%.2f ang=%.2f",
                target,yaw,err,cmd.angular.z);
            return false;
        }

        RCLCPP_INFO(this->get_logger(),
            "[SEARCH][TURN][DONE] target=%.2f yaw=%.2f",
            target,yaw);
        return true;
    };

    switch(FollowPattern)
    {
        case kTurnRight:
        {
            double tgt=normalize360(base_heading-90.0);
            if(!turnTo(tgt)) break;
            FollowPattern=kTurnLeft;
            RCLCPP_INFO(this->get_logger(),"[SEARCH][FSM] TURN_RIGHT → TURN_LEFT");
            break;
        }

        case kTurnLeft:
        {
            double tgt=normalize360(base_heading+90.0);
            if(!turnTo(tgt)) break;
            FollowPattern=kFaceForward;
            RCLCPP_INFO(this->get_logger(),"[SEARCH][FSM] TURN_LEFT → FACE_FORWARD");
            break;
        }

        case kFaceForward:
        {
            double tgt=normalize360(base_heading+offset_deg);
            if(!turnTo(tgt)) break;
            FollowPattern=kMoveForward;
            RCLCPP_INFO(this->get_logger(),
                "[SEARCH][FSM] FACE_FORWARD → MOVE_FORWARD | offset=%.2f",
                offset_deg);
            break;
        }

        case kMoveForward:
        {
            if(!timing){
                end_time = now + rclcpp::Duration::from_seconds(3.0);
                timing = true;
                RCLCPP_INFO(this->get_logger(),
                    "[SEARCH][MOVE] start forward window (3.0s)");
            }

            if(now < end_time){
                cmd.linear.x = kMaxLinearVel;
                RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 500,
                    "[SEARCH][MOVE] forward | v=%.2f", cmd.linear.x);
            }
            else{
                timing = false;
                double prev = offset_deg;
                offset_deg += (offset_deg == 0.0 ? 15.0 : 10.0);
                offset_deg = std::clamp(offset_deg, -90.0, 90.0);

                FollowPattern = (search_skew == kLeftSkew) ? kTurnRight : kTurnLeft;

                RCLCPP_WARN(this->get_logger(),
                    "[SEARCH][EXPAND] offset %.2f → %.2f | next=%s",
                    prev, offset_deg,
                    (FollowPattern == kTurnRight ? "TURN_RIGHT" : "TURN_LEFT"));
            }

            publishVel(cmd);
            break;
        }

        default:
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

    cone_goal_reached = false;
    gps_goal_reached  = false;
    delivery_start_time_ = rclcpp::Time(0);

    CurrState = kManualState;

    RCLCPP_WARN(this->get_logger(), "[MISSION] Delivery complete → MANUAL");
}

// Helpers : 

void SensorCallback::publishVel(const geometry_msgs::msg::Twist& msg)
{
    geometry_msgs::msg::Twist cmd = msg;
    cmd.linear.x  = std::clamp(cmd.linear.x, 0.0, kMaxLinearVel);
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
            if (std::abs(point.x - cone_x) < 0.3 &&
                std::abs(point.y - cone_y) < 0.3)
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
    static rclcpp::Time close_since;
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
    search_skew = (skew == kLeftSkew) ? kLeftSkew : kRightSkew;
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
