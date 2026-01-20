#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>

#include <pcl_conversions/pcl_conversions.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/extract_indices.h>

#include <Eigen/Dense>

using pcl::PointXYZ;

class DitchDetector : public rclcpp::Node
{
public:
    DitchDetector() : Node("ditch_detector")
    {
        cloud_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>(
            "/zed/zed_node/point_cloud/cloud_registered", rclcpp::SensorDataQoS(),
            std::bind(&DitchDetector::cloudCb, this, std::placeholders::_1));

        obstacle_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>(
            "/obstacles", 10);

        RCLCPP_INFO(get_logger(), "Ditch detector started (ditches + >40deg slopes)");
    }

private:
    void cloudCb(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        pcl::PointCloud<PointXYZ>::Ptr cloud(new pcl::PointCloud<PointXYZ>);
        pcl::fromROSMsg(*msg, *cloud);

        pcl::PassThrough<PointXYZ> pass;
        pass.setInputCloud(cloud);

        pass.setFilterFieldName("x");
        pass.setFilterLimits(0.4, 4.5);
        pass.filter(*cloud);

        pass.setFilterFieldName("y");
        pass.setFilterLimits(-1.2, 1.2);
        pass.filter(*cloud);

        pass.setFilterFieldName("z");
        pass.setFilterLimits(-1.0, 0.2);
        pass.filter(*cloud);

        if (cloud->size() < 150)
            return;

        pcl::VoxelGrid<PointXYZ> voxel;
        voxel.setInputCloud(cloud);
        voxel.setLeafSize(0.05f, 0.05f, 0.05f);
        voxel.filter(*cloud);

        pcl::SACSegmentation<PointXYZ> seg;
        pcl::PointIndices::Ptr inliers(new pcl::PointIndices);
        pcl::ModelCoefficients::Ptr coeff(new pcl::ModelCoefficients);

        seg.setOptimizeCoefficients(true);
        seg.setModelType(pcl::SACMODEL_PLANE);
        seg.setMethodType(pcl::SAC_RANSAC);
        seg.setDistanceThreshold(0.025);
        seg.setMaxIterations(120);
        seg.setInputCloud(cloud);
        seg.segment(*inliers, *coeff);

        if (inliers->indices.size() < 0.25 * cloud->size())
            return;

        Eigen::Vector3f normal(
            coeff->values[0],
            coeff->values[1],
            coeff->values[2]);
        normal.normalize();

        double slope_deg =
            std::acos(std::abs(normal.dot(Eigen::Vector3f::UnitZ()))) * 180.0 / M_PI;

        pcl::PointCloud<PointXYZ>::Ptr obstacle_cloud(new pcl::PointCloud<PointXYZ>);
        obstacle_cloud->header = cloud->header;

        if (slope_deg > 40.0)
        {
            *obstacle_cloud = *cloud;
            RCLCPP_WARN_THROTTLE(
                get_logger(), *get_clock(), 500,
                "Steep surface detected: %.1f deg -> obstacle", slope_deg);
        }
        else
        {
            pcl::ExtractIndices<PointXYZ> extract;
            pcl::PointCloud<PointXYZ>::Ptr nonground(new pcl::PointCloud<PointXYZ>);
            extract.setInputCloud(cloud);
            extract.setIndices(inliers);
            extract.setNegative(true);
            extract.filter(*nonground);

            for (const auto &pt : nonground->points)
            {
                double z_expected =
                    -(coeff->values[0] * pt.x +
                      coeff->values[1] * pt.y +
                      coeff->values[3]) /
                    coeff->values[2];

                if (z_expected < -0.15 || z_expected > 0.05)
                    continue;

                double dz = z_expected - pt.z;

                if (dz > 0.15 && pt.x < 3.5)
                {
                    obstacle_cloud->points.emplace_back(
                        pt.x, pt.y, z_expected);
                }
            }

            if (!obstacle_cloud->points.empty())
            {
                RCLCPP_WARN_THROTTLE(
                    get_logger(), *get_clock(), 500,
                    "Ditch detected -> obstacle wall (%zu pts)",
                    obstacle_cloud->points.size());
            }
        }

        if (!obstacle_cloud->points.empty())
        {
            sensor_msgs::msg::PointCloud2 out;
            pcl::toROSMsg(*obstacle_cloud, out);
            out.header = msg->header;
            obstacle_pub_->publish(out);
        }
    }

    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr cloud_sub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr obstacle_pub_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DitchDetector>());
    rclcpp::shutdown();
    return 0;
}
