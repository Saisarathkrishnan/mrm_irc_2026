#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <cv_bridge/cv_bridge.h>

#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

class SafetyNegativeObstacleFused : public rclcpp::Node {
public:
  SafetyNegativeObstacleFused() : Node("safety_negative_obstacle_fused") {

    depth_sub_ = create_subscription<sensor_msgs::msg::Image>(
      "/zed/zed_node/depth/depth_registered",10,
      std::bind(&SafetyNegativeObstacleFused::depthCb,this,std::placeholders::_1));

    cloud_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
      "/local_grid_obstacle",10,
      std::bind(&SafetyNegativeObstacleFused::cloudCb,this,std::placeholders::_1));

    pub_ = create_publisher<sensor_msgs::msg::PointCloud2>(
      "/local_grid_safe",10);
  }

private:
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr depth_sub_;
  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_sub_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_;

  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_{new pcl::PointCloud<pcl::PointXYZ>};

  void cloudCb(const sensor_msgs::msg::PointCloud2::SharedPtr msg){
    pcl::fromROSMsg(*msg,*cloud_);
  }

  void depthCb(const sensor_msgs::msg::Image::SharedPtr msg){
    if(cloud_->empty()) return;

    auto img=cv_bridge::toCvCopy(msg)->image;
    int rows=img.rows, cols=img.cols;
    int cx=cols/2;

    int y_start=rows*0.60;
    int y_end  =rows*0.90;

    constexpr float DROP_THRESH=0.15;     // 15 cm
    constexpr float MAX_RANGE  =3.0;      // meters

    bool hazard=false;

    for(int dx=-160;dx<=160;dx+=4){
      int x=cx+dx;
      if(x<0||x>=cols) continue;

      float ground=std::numeric_limits<float>::infinity();

      for(int y=y_start;y<=y_end;y+=2){
        float d=img.at<float>(y,x);
        if(std::isfinite(d) && d<ground) ground=d;
      }

      if(!std::isfinite(ground)){ hazard=true; break; }
      if(ground>MAX_RANGE) continue;

      for(int y=y_start;y<=y_end;y+=2){
        float d=img.at<float>(y,x);
        if(!std::isfinite(d)){ hazard=true; break; }
        if(d-ground>DROP_THRESH){ hazard=true; break; }
      }

      if(hazard) break;
    }

    pcl::PointCloud<pcl::PointXYZ> out=*cloud_;

    if(hazard){
      for(float x=0.6;x<=2.0;x+=0.05)
        for(float y=-0.6;y<=0.6;y+=0.05)
          for(float z=0.0;z<=0.6;z+=0.05)
            out.points.emplace_back(x,y,z);
    }

    out.width=out.points.size();
    out.height=1;
    out.is_dense=true;

    sensor_msgs::msg::PointCloud2 ros_out;
    pcl::toROSMsg(out,ros_out);
    ros_out.header.frame_id="base_link";
    ros_out.header.stamp=now();
    pub_->publish(ros_out);
  }
};

int main(int argc,char** argv){
  rclcpp::init(argc,argv);
  rclcpp::spin(std::make_shared<SafetyNegativeObstacleFused>());
  rclcpp::shutdown();
  return 0;
}
