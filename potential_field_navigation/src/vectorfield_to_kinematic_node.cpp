// ============================================================================
// Name        : vectorfield_to_kinematic_node.cpp
// Author      : Daniel Rudolph <drudolph@techfak.uni-bielefeld.de>
//               Timo Korthals <tkorthals@cit-ec.uni-bielefeld.de>
// Description : Receive the merged vectorfield and publish the steering vector
// ============================================================================

// ROS
#include <ros/ros.h>
#include <tf/tf.h>
#include <sensor_msgs/Image.h>
#include <nav_msgs/Odometry.h>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <tf/transform_listener.h>

// ROS - OpenCV_ Bridge
#include <cv_bridge/cv_bridge.h>
#include <image_transport/subscriber_filter.h>

#include "potential_field_utils.hpp"
#include <mutex>

using namespace std;

ros::Publisher pub;

static double meterPerPixel;
static int syncTopics;
static cv::Mat vectorfield;
static bool dataArrived = false;
//static float velocityScale_meterPerSecond = 0.1;
static float velocityScale_meterPerSecond;
//static float angularScale_radPerSecond = 0.1;
static float angularScale_radPerSecond;
static bool pixelMode;
static bool twistMode;
static double pixelScale;

tf::TransformListener *listener;

void process(const cv::Mat &vectorfield, const nav_msgs::OdometryConstPtr odom) {

    cv::Point2i pose2d_i;
    cv::Point2i pose2d_o;

    if (pixelMode) {

        pose2d_i = cv::Point2i((int) (odom->pose.pose.position.x * pixelScale),
                               (int) (odom->pose.pose.position.y * pixelScale));

    } else {

        tf::StampedTransform transform; //Transformation between the robot and the vector field (JO)

        try {

            listener->lookupTransform("potential_field", "/amiro1/base_link", ros::Time(0), transform);

        } catch (tf::TransformException ex) {

            ROS_ERROR("%s", ex.what());
            ros::Duration(1.0).sleep();

        }

        geometry_msgs::Pose odomPose_tf;

        odomPose_tf.position.x = transform.getOrigin().x();
        odomPose_tf.position.y = transform.getOrigin().y();

//        ROS_INFO("odomPose_tf.position.x = %f, odomPose_tf.position.y = %f", odomPose_tf.position.x, odomPose_tf.position.y);

//        pose2d_i = cv::Point2i(pose2pixel(odom->pose.pose, vectorfield.cols, vectorfield.rows, meterPerPixel));
        pose2d_i = cv::Point2i(pose2pixel(odomPose_tf, vectorfield.cols, vectorfield.rows, meterPerPixel)); // JO

        // ToDo: Find out why this works out fine without any transformation?!
        pose2d_o.x = pose2d_i.x;
        pose2d_o.y = pose2d_i.y;

    }

    if (pose2d_i.x < 0 || pose2d_i.x >= vectorfield.cols || pose2d_i.y < 0 || pose2d_i.y >= vectorfield.rows) {

        ROS_WARN("[%s] Current AMiRo pose2d_i %d %d is not in image.", ros::this_node::getName().c_str(), pose2d_i.x,
                 pose2d_i.y);
        return;

    }

    // IMPORTANT: We assume the orientation olf the robot resides in the world frame
    // Get the vector in the vectorfield at robot position
//    cv::Point2f vector(vectorfield.at<cv::Vec2f>(pose2d_i.y, pose2d_i.x)[0],
//                       vectorfield.at<cv::Vec2f>(pose2d_i.y, pose2d_i.x)[1]);
    cv::Point2f vector(vectorfield.at<cv::Vec2f>(pose2d_o.y, pose2d_o.x)[0],
                       vectorfield.at<cv::Vec2f>(pose2d_o.y, pose2d_o.x)[1]);

    // rotate the vector by 90 degree (ccw) (JO)
    float xBuffer;
    xBuffer = vector.x;
    vector.x = -1.0 * vector.y;
    vector.y = 1.0 * xBuffer;

    const float vectorAbs = cv::norm(vector);
    const double vectorAngle = atan2(vector.y, vector.x);

    // Get the robot information (invert the robot angle, because it is given in the camera frame with z-axis pointing down)
//    const double robotAngle = (-1) * tf::getYaw(odom->pose.pose.orientation);
    const double robotAngle = tf::getYaw(odom->pose.pose.orientation); // JO
    const float angleDiff = getAngleDiff(robotAngle, vectorAngle);

    const float vectorAbs_max = 2.0; // JO

    if (twistMode) {

        ROS_INFO("vectorAbs = %f, angleDiff = %f", vectorAbs, angleDiff); // JO

        // Calculate the steering vector
        geometry_msgs::Twist twist;

        twist.linear.x = velocityScale_meterPerSecond * vectorAbs;
        twist.angular.z = angularScale_radPerSecond * angleDiff;

        if (vectorAbs > vectorAbs_max) { // JO

            twist.linear.x = velocityScale_meterPerSecond * vectorAbs_max;

        } else {

            twist.linear.x = velocityScale_meterPerSecond * vectorAbs;

        }

//        ROS_DEBUG_STREAM(
//                ros::this_node::getName() << " VectorAbs: " << vectorAbs << ", robotAngle: " << robotAngle * 180 / M_PI
//                                          << ", vectorAngle: " << vectorAngle * 180 / M_PI << ", diff: "
//                                          << angleDiff * 180 / M_PI);
        pub.publish(twist);

    } else {

        geometry_msgs::Vector3 vec3;
        // The steering vector only has to point in y direction if the angleDiff equals zero
        vec3.x = cos(angleDiff + M_PI / 2) * vectorAbs;
        vec3.y = sin(angleDiff + M_PI / 2) * vectorAbs;
        pub.publish(vec3);

    }

}

void processSynced(const sensor_msgs::ImageConstPtr &image, const nav_msgs::OdometryConstPtr odom) {

    cv::Mat vectorfield = cv_bridge::toCvShare(image, image->encoding)->image;
    process(vectorfield, odom);

}

void processVectorfield(const sensor_msgs::ImageConstPtr &msg) {

    // TODO Normalize vectorfield (?)
    vectorfield = cv_bridge::toCvCopy(msg, msg->encoding)->image;
    dataArrived = true;

}

void processOdom(const nav_msgs::OdometryConstPtr &odom) {

    if (!dataArrived) {

        ROS_INFO_STREAM_ONCE(ros::this_node::getName() << " Waiting on vectorfield to arrive");
        return;

    } else {

        ROS_INFO_STREAM_ONCE(ros::this_node::getName() << " Vectorfield arrived");

    }

    process(vectorfield, odom);
}


int main(int argc, char *argv[]) {

    ROS_INFO("Start: %s", ros::this_node::getName().c_str());

    // Init ROS
    ros::init(argc, argv, ros::this_node::getName());
    ros::NodeHandle node("~");

    listener = new tf::TransformListener;

    // ROS Topics
    string twistPublisherTopic, vectorPublisherTopic;
    string vectorfield_listener_topic;
    string amiroOdomListenerTopic;
    node.param<string>("vectorfield_listener_topic", vectorfield_listener_topic, "/image/vectorfield");
    node.param<string>("amiro_odom_listener_topic", amiroOdomListenerTopic, "/amiro1/odom");
    node.param<string>("twist_publisher_topic", twistPublisherTopic, "/amiro1/cmd_vel");
    node.param<string>("vector_publisher_topic", vectorPublisherTopic, "/amiro1/vector");
    node.param<double>("meter_per_pixel", meterPerPixel, 0.003);
    node.param<float>("velocityScale_meterPerSecond", velocityScale_meterPerSecond, 0.1);
    node.param<float>("angularScale_radPerSecond", angularScale_radPerSecond, 0.1);
    node.param<int>("syncTopics", syncTopics, 0);
//    node.param<bool>("pixel_mode", pixelMode, false);
    node.param<bool>("pixel_mode", pixelMode, true);
    node.param<bool>("twist_mode", twistMode, false);
    node.param<double>("pixel_scale", pixelScale, 1.0);

    ROS_INFO("[%s] vectorfield_listener_topic: %s", ros::this_node::getName().c_str(),
             vectorfield_listener_topic.c_str());
    ROS_INFO("[%s] amiro_odom_listener_topic: %s", ros::this_node::getName().c_str(), amiroOdomListenerTopic.c_str());
    ROS_INFO("[%s] twist_publisher_topic: %s", ros::this_node::getName().c_str(), twistPublisherTopic.c_str());
    ROS_INFO("[%s] vector_publisher_topic: %s", ros::this_node::getName().c_str(), vectorPublisherTopic.c_str());
    ROS_INFO("[%s] meter_per_pixel: %f", ros::this_node::getName().c_str(), meterPerPixel);
    ROS_INFO("[%s] pixel_mode: %i", ros::this_node::getName().c_str(), pixelMode);
    ROS_INFO("[%s] twist_mode: %i", ros::this_node::getName().c_str(), twistMode);
    ROS_INFO("[%s] pixel_scale: %f", ros::this_node::getName().c_str(), pixelScale);

    if (twistMode) {

        // The twist publisher
        pub = node.advertise<geometry_msgs::Twist>(twistPublisherTopic, 1);

    } else {

        // The Vector3 publisher
        pub = node.advertise<geometry_msgs::Vector3>(vectorPublisherTopic, 1);

    }

    // The subscriber
    image_transport::ImageTransport imageTransport(node);
    image_transport::SubscriberFilter merged_potentialfield_sub;
    message_filters::Subscriber<nav_msgs::Odometry> amiro_odom_sub;
    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, nav_msgs::Odometry> MySyncPolicy;
    message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), merged_potentialfield_sub, amiro_odom_sub);
    image_transport::Subscriber vectorfield_sub;
    ros::Subscriber odom_sub;

    if (syncTopics) {

        merged_potentialfield_sub.subscribe(imageTransport, vectorfield_listener_topic, 1);
        amiro_odom_sub.subscribe(node, amiroOdomListenerTopic, 1);
        sync.registerCallback(boost::bind(&processSynced, _1, _2));

    } else {

        vectorfield_sub = imageTransport.subscribe(vectorfield_listener_topic, 1, &processVectorfield);
        odom_sub = node.subscribe(amiroOdomListenerTopic, 1, &processOdom);

    }

    ros::spin();
    return 0;
}
