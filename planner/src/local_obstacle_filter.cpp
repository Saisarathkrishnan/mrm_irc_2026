#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>

#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <pcl/filters/passthrough.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/segmentation/sac_segmentation.h>

class ZedCloudFilter : public rclcpp::Node
{
public:
    ZedCloudFilter()
    : Node("zed_cloud_filter")
    {
        declare_parameter("input_cloud", "/zed/zed_node/point_cloud/cloud_registered");
        declare_parameter("output_cloud", "/obstacles");

        // ROI (cone-safe)
        declare_parameter("x_min", 0.3);
        declare_parameter("x_max", 3.0);
        declare_parameter("y_min", -1.2);
        declare_parameter("y_max",  1.2);
        declare_parameter("z_min", -0.25);
        declare_parameter("z_max",  1.2);

        // Filtering
        declare_parameter("voxel_size", 0.08);
        declare_parameter("ransac_dist_thresh", 0.12);
        declare_parameter("ransac_angle_deg", 20.0);

        sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            get_parameter("input_cloud").as_string(), 10,
            std::bind(&ZedCloudFilter::cloudCallback, this, std::placeholders::_1));

        pub_ = create_publisher<sensor_msgs::msg::PointCloud2>(
            get_parameter("output_cloud").as_string(), 10);

        RCLCPP_INFO(get_logger(), "ZED PCL filter (slope + cone safe) started");
    }

private:
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr sub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_;

    void cloudCallback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
        pcl::fromROSMsg(*msg, *cloud);
        if (cloud->empty()) return;

        // ---------------- ROI CROP ----------------
        pass(cloud, "x", p("x_min"), p("x_max"));
        pass(cloud, "y", p("y_min"), p("y_max"));
        pass(cloud, "z", p("z_min"), p("z_max"));
        if (cloud->empty()) return;

        // ---------------- VOXEL GRID ----------------
        pcl::VoxelGrid<pcl::PointXYZ> vg;
        vg.setLeafSize(p("voxel_size"), p("voxel_size"), p("voxel_size"));
        vg.setInputCloud(cloud);
        vg.filter(*cloud);
        if (cloud->empty()) return;

        // ---------------- RANSAC (GROUND ONLY) ----------------
        pcl::SACSegmentation<pcl::PointXYZ> seg;
        seg.setOptimizeCoefficients(true);
        seg.setModelType(pcl::SACMODEL_PERPENDICULAR_PLANE);
        seg.setMethodType(pcl::SAC_RANSAC);

        // Z-up axis (gravity approx)
        seg.setAxis(Eigen::Vector3f(0.0, 0.0, 1.0));
        seg.setEpsAngle(p("ransac_angle_deg") * M_PI / 180.0);
        seg.setDistanceThreshold(p("ransac_dist_thresh"));

        pcl::PointIndices::Ptr inliers(new pcl::PointIndices);
        pcl::ModelCoefficients::Ptr coeffs(new pcl::ModelCoefficients);

        seg.setInputCloud(cloud);
        seg.segment(*inliers, *coeffs);

        if (!inliers->indices.empty())
        {
            pcl::ExtractIndices<pcl::PointXYZ> extract;
            extract.setInputCloud(cloud);
            extract.setIndices(inliers);
            extract.setNegative(true);   // remove ground
            extract.filter(*cloud);
        }

        if (cloud->empty()) return;

        // ---------------- PUBLISH ----------------
        sensor_msgs::msg::PointCloud2 out;
        pcl::toROSMsg(*cloud, out);
        out.header = msg->header;
        pub_->publish(out);
    }

    void pass(pcl::PointCloud<pcl::PointXYZ>::Ptr& cloud,
              const std::string& field, double min, double max)
    {
        pcl::PassThrough<pcl::PointXYZ> p;
        p.setInputCloud(cloud);
        p.setFilterFieldName(field);
        p.setFilterLimits(min, max);
        p.filter(*cloud);
    }

    inline double p(const std::string& name)
    {
        return get_parameter(name).as_double();
    }
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ZedCloudFilter>());
    rclcpp::shutdown();
    return 0;
}
