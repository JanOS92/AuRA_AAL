// ============================================================================
// Name        : edge_separation_node.cpp
// Author      : Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
// Description : Separates image edges into different clusters.
// ============================================================================

// ROS
#include <ros/ros.h>
#include <sensor_msgs/Image.h>

// ROS - OpenCV_ Bridge
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv-3.3.1/opencv2/imgproc.hpp>

// Utils
#include "potential_field_utils.hpp"


using namespace std;

// Ros Topics
string rosListenerTopicImg;
string rosPublisherTopicImg;

// ROS publisher
image_transport::Publisher imagePublisherImg;
static image_transport::Publisher imagePublisher;

// Process variables
static int image_flip_code;

void edge_dyeing(const sensor_msgs::ImageConstPtr &msg) {

    /**
     * Setup
     */
    cv::Mat image;
    cv::Mat image_gray;
    cv::Mat image_binary;

    /**
     * Initial image routine
     */
    image = cv_bridge::toCvShare(msg, msg->encoding)->image; // get the image from the msg pointer
    cv::cvtColor(image,image_gray, cv::COLOR_RGB2GRAY); // convert the rgb image into grayscale image

    /**
     * Edge extraction
     */
    cv::Mat edgeMap(image_gray.rows, image_gray.cols, CV_32FC2, cv::Scalar(0.0f));

    int lowThreshold = 0;
    int ratio = 3;
    int kernel_size = 3;

    cv::blur(image_gray, edgeMap, cv::Size(kernel_size, kernel_size)); // reduce noise with a kernel 3x3
    cv::Canny(edgeMap, edgeMap, lowThreshold, lowThreshold * ratio, kernel_size); // canny detector

    /**
     * Edge clustering
     */
    cv::Mat labels;
    int nLabels = cv::connectedComponents(edgeMap, labels); // find connected components in the edgeMap

    /**
    * Edge dyeing
    */
    cv::Mat dyedBGR(image.rows, image.cols, CV_8UC3, cv::Scalar(0,0,255));

    std::vector<cv::Vec3b> colors(nLabels);
    colors[0] = cv::Vec3b(0, 0, 0); // set background value

    // set the dyeing vectors
    float dyeValue = 255.0;
    for(int label = 1; label <= nLabels; ++label) {

        if (label < nLabels - 1) {

            colors[label] = cv::Vec3b( dyeValue, 0.0, 0.0 ); // blue

        } else {

            colors[label] = cv::Vec3b( 0.0, 0.0, dyeValue ); // red

        }

    }

    // dye the edges
#pragma omp parallel for
    for(int idy = 0; idy < dyedBGR.rows; ++idy){

        for(int idx = 0; idx < dyedBGR.cols; ++idx){

            int labelIdx = labels.at<int>(idy, idx);
            cv::Vec3b &pixel = dyedBGR.at<cv::Vec3b>(idy, idx);
            pixel = colors[labelIdx];

        }

    }
    
    /**
    * Publish dyedBGR
    */
    cv_bridge::CvImage cvImage;
    cvImage.encoding = sensor_msgs::image_encodings::BGR8;
    cvImage.image = dyedBGR;
    cvImage.header.frame_id = "current"; // send always as current

    ROS_INFO("[%s] Send as current", ros::this_node::getName().c_str());
    imagePublisher.publish(cvImage.toImageMsg());

//    /**
//     * Debug only
//     */
//    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE); // create a window for display.
//    cv::imshow("Display window", dyedBGR); // show image
//    cv::waitKey(0); // wait for a keystroke in the window

}

int main(int argc, char *argv[]) {

    // Init ROS
    ros::init(argc, argv, ros::this_node::getName());
    ros::NodeHandle node("~");
    ROS_INFO("Start: %s", ros::this_node::getName().c_str());

    // ROS Topics
    node.param<string>("image_listener_topic", rosListenerTopicImg, "/image");
    node.param<string>("dyed_image_publisher_topic", rosPublisherTopicImg, "/image/image_dyed");

    // image transport setup
    image_transport::ImageTransport imageTransport(node);

    // ROS subscriber
    image_transport::Subscriber sub = imageTransport.subscribe(rosListenerTopicImg, 1, edge_dyeing);

    // ROS publisher
    imagePublisher = imageTransport.advertise(rosPublisherTopicImg, 1, true);

    ros::spin();
    return 0;

}