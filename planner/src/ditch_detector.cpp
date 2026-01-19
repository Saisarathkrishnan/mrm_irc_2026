#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

class SafetyDitchCloudFused : public rclcpp::Node {
public:
  SafetyDitchCloudFused() : Node("safety_ditch_cloud_fused") {

    depth_sub_ = create_subscription<sensor_msgs::msg::Image>(
      "/zed/zed_node/depth/depth_registered",10,
      std::bind(&SafetyDitchCloudFused::depthCb,this,std::placeholders::_1));

    rtab_cloud_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
      "/local_grid_obstacle",10,
      std::bind(&SafetyDitchCloudFused::rtabCb,this,std::placeholders::_1));

    pub_ = create_publisher<sensor_msgs::msg::PointCloud2>(
      "/local_grid_safe",10);
  }

private:
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr depth_sub_;
  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr rtab_cloud_sub_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_;

  pcl::PointCloud<pcl::PointXYZ>::Ptr rtab_cloud_{new pcl::PointCloud<pcl::PointXYZ>};

  void rtabCb(const sensor_msgs::msg::PointCloud2::SharedPtr msg){
    pcl::fromROSMsg(*msg,*rtab_cloud_);
  }

  void depthCb(const sensor_msgs::msg::Image::SharedPtr msg){
    if(rtab_cloud_->empty()) return;

    auto img=cv_bridge::toCvCopy(msg)->image;
    int cx=img.cols/2;
    int cy=img.rows/2+40;

    bool ditch=false;

    for(int dx=-60;dx<=60;dx+=4){
      float dn=img.at<float>(cy,cx+dx);
      float df=img.at<float>(cy+8,cx+dx);

      if(!std::isfinite(df)) { ditch=true; break; }
      if(std::isfinite(dn) && (df-dn>0.6 || (dn<1.0 && df>2.2))){
        ditch=true; break;
      }
    }

    pcl::PointCloud<pcl::PointXYZ> out=*rtab_cloud_;

    if(ditch){
      for(float x=0.7;x<=1.4;x+=0.05)
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
  rclcpp::spin(std::make_shared<SafetyDitchCloudFused>());
  rclcpp::shutdown();
  return 0;
}
