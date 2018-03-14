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

    /**
     * Initial image routine
     */
    image = cv_bridge::toCvShare(msg, msg->encoding)->image; // get the image from the msg pointer
    cv::cvtColor(image, image_gray, cv::COLOR_RGB2GRAY); // convert the rgb image into grayscale image

    /**
     * Edge direction
     */
    cv::Mat image_filled_line(image_gray.size(), image_gray.type());
#pragma omp parallel for
    for (int idy = 0; idy < image_gray.rows; ++idy) {

        for (int idx = 0; idx < image_gray.cols; ++idx) {

            uchar value = image_gray.at<uchar>(idy, idx);

            if (value > 128) {

                image_filled_line.at<uchar>(idy, idx) = 0;

            } else if (value == 128) {

                image_filled_line.at<uchar>(idy, idx) = 255;

            }

        }

    }

    cv::Mat inv_image_filled_line(image_gray.size(), image_gray.type());
    cv::bitwise_not(image_filled_line, inv_image_filled_line);

//    /**
//    * Debug only
//    */
//    cv::namedWindow("inv_image_filled_line", cv::WINDOW_NORMAL); // create a window for display.
//    cv::imshow("inv_image_filled_line", inv_image_filled_line); // show image
//    cv::waitKey(0); // wait for a keystroke in the window

    cv::Mat vectorFieldX(image.size(), CV_32FC2, cv::Scalar(0.0f, 0.0f));
    cv::Mat vectorFieldY(image.size(), CV_32FC2, cv::Scalar(0.0f, 0.0f));
    cv::Mat vectorField(image.size(), CV_32FC2, cv::Scalar(0.0f, 0.0f));

    cv::Scharr(inv_image_filled_line, vectorFieldX, CV_32FC2, 1, 0, 1, 0, cv::BORDER_REPLICATE);
    cv::Scharr(inv_image_filled_line, vectorFieldY, CV_32FC2, 0, 1, 1, 0, cv::BORDER_REPLICATE);

    cv::Mat vectorFieldChannels[2] = {vectorFieldX, vectorFieldY};
    cv::merge(vectorFieldChannels, 2, vectorField);

    /**
    * Edge extraction simple
    */
    cv::Mat inv;
    cv::Mat image_binary(image_gray.size(), image_gray.type());
    cv::bitwise_not(image_gray, inv); // invert the colors
    cv::threshold(inv, image_binary, 129, 255, cv::THRESH_BINARY);

    cv::Mat inv_sharpend;
    cv::GaussianBlur(image_binary, inv_sharpend, cv::Size(0, 0), 1); // cv::Size(0, 0): kernel size depends on sigmaX

    /**
     * Edge clustering
     */
    cv::Mat labels;
    int nLabels = cv::connectedComponents(image_binary, labels); // find connected components in the edgeMap

    /**
    * Edge dyeing
    */
    cv::Mat dyedBGR(image.rows, image.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    float dyeValue_max = 255.0;
    float dyeValue_half = 128.0;
    cv::Vec3b white = cv::Vec3b(dyeValue_max, dyeValue_max, dyeValue_max);
    cv::Vec3b gray = cv::Vec3b(dyeValue_half, dyeValue_half, dyeValue_half);
    cv::Vec3b black = cv::Vec3b(0.0, 0.0, 0.0);

    // stride
    float stride = sqrt((image.rows / 40) * (image.rows / 40) +
                        (image.cols / 40) * (image.cols / 40)); //image.rows/20*1/2 and image.cols/20*1/2
    stride = 2; //Debug only

    // dye the edges
    image.copyTo(dyedBGR); // copy the image to the dyedBGR
#pragma omp parallel for
    for (int idy = 0; idy < dyedBGR.rows; ++idy) {

        for (int idx = 0; idx < dyedBGR.cols; ++idx) {

            int labelIdx = labels.at<int>(idy, idx);
            cv::Vec3b &pixel = dyedBGR.at<cv::Vec3b>(idy, idx);

            float x = -1.0 * vectorField.at<cv::Vec2f>(idy, idx)[0];
            float y = -1.0 * vectorField.at<cv::Vec2f>(idy, idx)[1];
            float abs = cv::norm(vectorField.at<cv::Vec2f>(idy, idx));

            if (abs > 0.0) {

                float dir = atan2(y, x);

                float x_it = 0.0;
                float y_it = 0.0;

                if (isnan(dir)) {

                } else {

                    y_it = stride * sin(dir);
                    x_it = stride * cos(dir);

                }

                cv::Point start(idy, idx);
                cv::Point target(idy + (int) y_it, idx + (int) x_it);

//                cv::Vec3b &pixel_target = dyedBGR.at<cv::Vec3b>(target.y,target.x);

                ROS_INFO("(%i,%i) x = %f, y = %f", idy, idx, x, y);
                ROS_INFO("(%i,%i) x_it = %f, y_it = %f, dir = %f", idy, idx, x_it, y_it, dir);
                ROS_INFO("start = (%i,%i), target = (%i,%i), labelIdx = %i",idy,idx,idy + (int)y_it,idx + (int)x_it,labelIdx);

                if (labelIdx == 1) { // leave the background

                    dyedBGR.at<cv::Vec3b>(idy + (int)y_it,idx + (int)x_it) = cv::Vec3b(dyeValue_max, 0.0, 0.0); // blue

                    cv::LineIterator it(image_gray, start, target);

//                    for (int i = 0; i < it.count; i++, ++it) {
//
//                        // ToDo: Find out why it has the wrong direction?
//                        dyedBGR.at<cv::Vec3b>(it.pos().y,it.pos().x) = white;
//
//                    }

                } else if (labelIdx == 2) {

                    dyedBGR.at<cv::Vec3b>(idy + (int)y_it,idx + (int)x_it) = cv::Vec3b(0.0, 0.0, dyeValue_max); // red

                    cv::LineIterator it(image_gray, start, target);

//                    for (int i = 0; i < it.count; i++, ++it) {
//
//                        // ToDo: Find out why it has the wrong direction?
//                        dyedBGR.at<cv::Vec3b>(it.pos().y,it.pos().x) = white;
//
//                    }

                } else {

                }

            }


        }

    }

//    /**
//    * Publish dyedBGR
//    */
//    cv_bridge::CvImage cvImage;
//    cvImage.encoding = sensor_msgs::image_encodings::BGR8;
//    cvImage.image = dyedBGR;
//    cvImage.header.frame_id = "current"; // send always as current
//
//    ROS_INFO("[%s] Send as current", ros::this_node::getName().c_str());
//    imagePublisher.publish(cvImage.toImageMsg());

    /**
     * Debug only
     */
    cv::namedWindow("Current", cv::WINDOW_NORMAL); // create a window for display.
    cv::imshow("Current", dyedBGR); // show image
    cv::waitKey(0); // wait for a keystroke in the window

}

int main(int argc, char *argv[]) {

    // Init ROS
    ros::init(argc, argv, ros::this_node::getName());
    ros::NodeHandle node("~");
    ROS_INFO("Start: %s", ros::this_node::getName().c_str());

    // ROS Topics
    node.param<string>("image_listener_topic", rosListenerTopicImg, "/image");
    node.param<string>("dyed_image_publisher_topic", rosPublisherTopicImg, "/image/image_current");

    // image transport setup
    image_transport::ImageTransport imageTransport(node);

    // ROS subscriber
    image_transport::Subscriber sub = imageTransport.subscribe(rosListenerTopicImg, 1, edge_dyeing);

    // ROS publisher
    imagePublisher = imageTransport.advertise(rosPublisherTopicImg, 1, true);

    ros::spin();
    return 0;

}