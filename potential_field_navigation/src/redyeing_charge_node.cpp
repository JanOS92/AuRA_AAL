// ============================================================================
// Name        : redyeing_charge_node.cpp
// Author      : Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
// Description : Prepare an input image for the charge routine.
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

// Process constants
float dyeValue_max = 255;
float dyeValue_half = 128;
cv::Vec3b blue = cv::Vec3b(dyeValue_max, 0, 0);
cv::Vec3b red = cv::Vec3b(0, 0, dyeValue_max);
cv::Vec3b gray = cv::Vec3b(dyeValue_half, dyeValue_half, dyeValue_half);
cv::Vec3b white = cv::Vec3b(dyeValue_max, dyeValue_max, dyeValue_max);

void dyeing_charge(const sensor_msgs::ImageConstPtr &msg) {

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

    cv::Mat image_binary(image_gray.size(), image_gray.type(),cv::Scalar(0.0f));

#pragma omp parallel for
    for(int idy = 0; idy < image_gray.rows; ++idy) { // get a white map

        for (int idx = 0; idx < image_gray.cols; ++idx) {

            uchar value = image_gray.at<uchar>(idy,idx);

            if(value != dyeValue_max) {

                image_binary.at<uchar>(idy,idx) = dyeValue_max;

            }

        }

    }

    /**
     * ---------------------
     * Stay on line schema
     * ---------------------
     */
    int kernelSizeX = image.cols / 40;
    int kernelSizeY = image.rows / 40;

    cv::Mat whiteMap(image_gray.size(), image_gray.type(), cv::Scalar(0.0f, 0.0f));

#pragma omp parallel for
    for (int idy = 0; idy < whiteMap.rows; ++idy) {  // broaden the line

        for (int idx = 0; idx < whiteMap.cols; ++idx) {

            uchar value = image_binary.at<uchar>(idy, idx);

            if (value == dyeValue_max) { // case white

                for (int idy_kernel = 0; idy_kernel < kernelSizeY; ++idy_kernel) {

                    int y_dye = (idy - kernelSizeY / 2) + idy_kernel;

                    for (int idx_kernel = 0; idx_kernel < kernelSizeX; ++idx_kernel) {

                        int x_dye = (idx - kernelSizeX / 2) + idx_kernel;

                        uchar &valueWhiteMap = whiteMap.at<uchar>(y_dye, x_dye);

                        valueWhiteMap = 255;

                    }

                }

            }

        }

    }

    /**
    * Redyeing
    */
    cv::Mat dyedBGR(image.rows, image.cols, CV_8UC3, cv::Scalar(dyeValue_half, dyeValue_half, dyeValue_half));

#pragma omp parallel for
    for (int idy = 0; idy < dyedBGR.rows; ++idy) {  // redye the image for charge

        for (int idx = 0; idx < dyedBGR.cols; ++idx) {

            cv::Vec3b &pixel = dyedBGR.at<cv::Vec3b>(idy, idx);

            if (image_binary.at<uchar>(idy,idx) == dyeValue_max) {

                pixel = blue;

            } else if(whiteMap.at<uchar>(idy,idx) == dyeValue_max) {

                pixel = white;

            } else {

                pixel = gray;

            }

        }

    }

    /**
     * dyedBGR postprocessing
     */
    cv::Mat inv_sharpend;
//    cv::GaussianBlur(dyedBGR, inv_sharpend, cv::Size(0, 0), 2); // cv::Size(0, 0): kernel size depends on sigmaX
    cv::GaussianBlur(dyedBGR, inv_sharpend, cv::Size(7, 7), 0); // magic line

#pragma omp parallel for
    for (int idy = 0; idy < dyedBGR.rows; ++idy) {  // redye the image for charge

        for (int idx = 0; idx < dyedBGR.cols; ++idx) {

            cv::Vec3b &pixelOrig = dyedBGR.at<cv::Vec3b>(idy, idx);
            cv::Vec3b pixelBlur = inv_sharpend.at<cv::Vec3b>(idy, idx);

            if(pixelOrig == blue && pixelBlur.val[2] > 0) { // magic condition

                pixelOrig = white;

            }

        }

    }

    /**
    * Publish dyedBGR
    */
    cv_bridge::CvImage cvImage;
    cvImage.encoding = sensor_msgs::image_encodings::BGR8;
    cvImage.image = dyedBGR;
    cvImage.header.frame_id = "charge"; // send always as charge

    ROS_INFO("[%s] Send as charge", ros::this_node::getName().c_str());
    imagePublisher.publish(cvImage.toImageMsg());

    /**
    * Debug only
    */
    cv::namedWindow("Charge", cv::WINDOW_NORMAL); // create a window for display.
    cv::imshow("Charge", dyedBGR); // show image
    cv::waitKey(0); // wait for a keystroke in the window

}

int main(int argc, char *argv[]) {

    // Init ROS
    ros::init(argc, argv, ros::this_node::getName());
    ros::NodeHandle node("~");
    ROS_INFO("Start: %s", ros::this_node::getName().c_str());

    // ROS Topics
    node.param<string>("image_listener_topic", rosListenerTopicImg, "/image");
    node.param<string>("dyed_image_publisher_topic", rosPublisherTopicImg, "/image/image_charge");

    // image transport setup
    image_transport::ImageTransport imageTransport(node);

    // ROS subscriber
    image_transport::Subscriber sub = imageTransport.subscribe(rosListenerTopicImg, 1, dyeing_charge);

    // ROS publisher
    imagePublisher = imageTransport.advertise(rosPublisherTopicImg, 1, true);

    ros::spin();
    return 0;

}