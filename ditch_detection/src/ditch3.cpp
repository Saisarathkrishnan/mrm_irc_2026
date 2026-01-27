#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/float32.hpp>

#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <tf2_sensor_msgs/tf2_sensor_msgs.hpp>

#include <vector>
#include <queue>
#include <cmath>

class DitchDetectorDZ : public rclcpp::Node
{
public:
    DitchDetectorDZ()
    : Node("ditch_detector_dz"),
      tf_buffer_(get_clock()),
      tf_listener_(tf_buffer_)
    {
        cloud_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            "/zed/zed_node/point_cloud/cloud_registered",
            rclcpp::SensorDataQoS(),
            std::bind(&DitchDetectorDZ::cloudCallback,this,std::placeholders::_1));

        local_grid_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            "/local_grid_obstacle",
            rclcpp::SensorDataQoS(),
            std::bind(&DitchDetectorDZ::localGridCallback,this,std::placeholders::_1));

        wall_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>("/fake_wall_cloud",10);
        safe_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>("/local_grid_safe",10);
        ditch_pub_ = create_publisher<std_msgs::msg::Bool>("/ditch_detected",10);
        ditch_dist_pub_ = create_publisher<std_msgs::msg::Float32>("/ditch_distance",10);

        declare_parameter("target_frame","base_link");
        declare_parameter("xmin",0.6);
        declare_parameter("xmax",3.0);
        declare_parameter("x_bin",0.15);
        declare_parameter("y_bin",0.10);
        declare_parameter("corridor_width",2.0);
        declare_parameter("min_points_per_cell",4);
        declare_parameter("wall_height",1.0);
        declare_parameter("ground_z",-0.5);
        declare_parameter("min_region_cells",40);
        declare_parameter("release_frames",8);
    }

private:
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_sub_,local_grid_sub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr wall_pub_,safe_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr ditch_pub_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr ditch_dist_pub_;

    tf2_ros::Buffer tf_buffer_;
    tf2_ros::TransformListener tf_listener_;

    pcl::PointCloud<pcl::PointXYZ>::Ptr last_wall_{new pcl::PointCloud<pcl::PointXYZ>()};
    pcl::PointCloud<pcl::PointXYZ>::Ptr last_local_grid_{new pcl::PointCloud<pcl::PointXYZ>()};

    bool latched_{false};
    int release_count_{0};
    float latched_dist_{0.f};

    void cloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        std::string target;
        get_parameter("target_frame",target);

        sensor_msgs::msg::PointCloud2 tf_cloud;
        try{
            auto tf=tf_buffer_.lookupTransform(target,msg->header.frame_id,tf2::TimePointZero);
            tf2::doTransform(*msg,tf_cloud,tf);
        }catch(...){return;}

        pcl::PointCloud<pcl::PointXYZ> cloud;
        pcl::fromROSMsg(tf_cloud,cloud);

        double xmin,xmax,x_bin,y_bin,width,wall_h,ground_z;
        int min_pts,min_region,release_frames;
        get_parameter("xmin",xmin);
        get_parameter("xmax",xmax);
        get_parameter("x_bin",x_bin);
        get_parameter("y_bin",y_bin);
        get_parameter("corridor_width",width);
        get_parameter("min_points_per_cell",min_pts);
        get_parameter("wall_height",wall_h);
        get_parameter("ground_z",ground_z);
        get_parameter("min_region_cells",min_region);
        get_parameter("release_frames",release_frames);

        int x_bins=int((xmax-xmin)/x_bin);
        int y_bins=int(width/y_bin);

        std::vector<std::vector<int>> grid(x_bins,std::vector<int>(y_bins,0));

        for(const auto&p:cloud.points)
        {
            if(!std::isfinite(p.x)) continue;
            if(p.x<xmin||p.x>xmax) continue;
            if(std::abs(p.y)>width*0.5) continue;

            int xi=int((p.x-xmin)/x_bin);
            int yi=int((p.y+width*0.5)/y_bin);
            if(xi<0||xi>=x_bins||yi<0||yi>=y_bins) continue;

            grid[xi][yi]++;
        }

        std::vector<std::vector<bool>> empty(x_bins,std::vector<bool>(y_bins,false));
        for(int x=0;x<x_bins;x++)
            for(int y=0;y<y_bins;y++)
                empty[x][y]=(grid[x][y]<min_pts);

        std::vector<std::vector<bool>> visited(x_bins,std::vector<bool>(y_bins,false));

        bool detected=false;
        float best_dist=1e9;

        for(int x=0;x<x_bins;x++)
        for(int y=0;y<y_bins;y++)
        {
            if(!empty[x][y]||visited[x][y]) continue;

            std::queue<std::pair<int,int>> q;
            std::vector<std::pair<int,int>> region;
            q.push({x,y});
            visited[x][y]=true;

            while(!q.empty())
            {
                auto[cx,cy]=q.front(); q.pop();
                region.push_back({cx,cy});

                for(int dx=-1;dx<=1;dx++)
                for(int dy=-1;dy<=1;dy++)
                {
                    int nx=cx+dx, ny=cy+dy;
                    if(nx<0||ny<0||nx>=x_bins||ny>=y_bins) continue;
                    if(visited[nx][ny]||!empty[nx][ny]) continue;
                    visited[nx][ny]=true;
                    q.push({nx,ny});
                }
            }

            if((int)region.size()<min_region) continue;

            float mean_x=0;
            for(auto&p:region)
                mean_x+=xmin+p.first*x_bin;
            mean_x/=region.size();

            if(mean_x<0.8) continue;

            if(mean_x<best_dist)
            {
                best_dist=mean_x;
                detected=true;
            }
        }

        if(detected)
        {
            latched_=true;
            latched_dist_=best_dist-0.2f;
            release_count_=0;
        }
        else if(latched_)
        {
            if(++release_count_>=release_frames)
                latched_=false;
        }

        buildWall(latched_,latched_dist_,width,wall_h,ground_z,tf_cloud.header);
        fuseAndPublish(tf_cloud.header);
    }

    void localGridCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        pcl::fromROSMsg(*msg,*last_local_grid_);
    }

    void buildWall(bool active,float dist,double width,double wall_h,double ground_z,
                   const std_msgs::msg::Header&header)
    {
        last_wall_->clear();
        last_wall_->header.frame_id=header.frame_id;

        std_msgs::msg::Bool b; b.data=active; ditch_pub_->publish(b);
        if(!active) return;

        for(double y=-width;y<=width;y+=0.05)
            for(double z=ground_z;z<=ground_z+wall_h;z+=0.05)
                last_wall_->points.emplace_back(dist,y,z);

        sensor_msgs::msg::PointCloud2 out;
        pcl::toROSMsg(*last_wall_,out);
        out.header=header;
        wall_pub_->publish(out);

        std_msgs::msg::Float32 d; d.data=dist; ditch_dist_pub_->publish(d);
    }

    void fuseAndPublish(const std_msgs::msg::Header&header)
    {
        pcl::PointCloud<pcl::PointXYZ> fused=*last_local_grid_;
        fused+=*last_wall_;
        sensor_msgs::msg::PointCloud2 out;
        pcl::toROSMsg(fused,out);
        out.header=header;
        safe_pub_->publish(out);
    }
};

int main(int argc,char**argv)
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<DitchDetectorDZ>());
    rclcpp::shutdown();
    return 0;
}
