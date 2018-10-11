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
//#include <opencv-3.3.1/opencv2/imgproc.hpp>
#include <omp.h>

// Utils
#include "potential_field_utils.hpp"
#include <algorithm>

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
float dyeValue_half = 128.0;
float dyeValue_max = 255.0;

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
    cv::cvtColor(image,image_gray, cv::COLOR_RGB2GRAY); // convert the rgb image into grayscale image

    cv::Mat image_binary(image_gray.size(), image_gray.type(),cv::Scalar(0.0f));

#pragma omp parallel for
    for(int idy = 0; idy < image_gray.rows; ++idy) { // get a white map

        for (int idx = 0; idx < image_gray.cols; ++idx) {

            uchar &value = image_gray.at<uchar>(idy,idx);

            if(value != dyeValue_max) {

                image_binary.at<uchar>(idy,idx) = dyeValue_max;

            }

        }

    }

    /**
     * Closeness detection
     */
    bool closedConture = false;
    std::vector< std::vector <cv::Point> > contours; // Vector for storing contour
    std::vector< cv::Vec4i > hierarchy;
    findContours( image_binary, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

    ROS_INFO("contours.size() = %i", contours.size());
    ROS_INFO("hierarchy.size() = %i", hierarchy.size());

    for (int i = 0; i < hierarchy.size(); i = hierarchy[i][0]) {

        if ( hierarchy[i][2] < 0 ) { //check if there is a child contour (magic line)

            ROS_INFO("closedConture = false");
            closedConture = false;

        } else {

            ROS_INFO("closedConture = true");
            closedConture = true;

        }

    }

    /**
     * Corner detection
     */
    cv::Mat dst, dst_norm, dst_norm_scaled;
    dst = cv::Mat::zeros( image_binary.size(), CV_32FC1 );
    std::vector<cv::Point> cornerList;

    if(!closedConture) {

        // detector parameters
        int blockSize = 2;
        int apertureSize = 3;
        double k = 0.04;
//        int thresh = 200;
        int thresh = 225;

        // detecting corners
        cv::cornerHarris(image_binary, dst, blockSize, apertureSize, k, cv::BORDER_DEFAULT);

        // normalizing
        cv::normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());

        // store corner coordinates
        for (int idy = 0; idy < dst_norm.rows; idy++) {

            for (int idx = 0; idx < dst_norm.cols; idx++) {

                if ((int) dst_norm.at<float>(idx, idy) > thresh) { // caution: idx -> idy, idy -> idx

//                    ROS_INFO("dst_norm.at<float>(%i, %i) = %i", idx, idy, (int) dst_norm.at<float>(idx, idy));
                    cornerList.emplace_back(cv::Point(idy, idx));

                }
            }
        }
    }

//    ROS_INFO("cornerList.size() = %i",cornerList.size());

    /**
     * Edge extraction
     */
    cv::Mat edgeMap(image_binary.rows, image_binary.cols, CV_32FC2, cv::Scalar(0.0f));

    // detector parameters
    int lowThreshold = 0;
    int ratio = 3;
    int kernel_size = 3;

    cv::blur(image_binary, edgeMap, cv::Size(kernel_size, kernel_size)); // reduce noise with a kernel 3x3
    cv::Canny(edgeMap, edgeMap, lowThreshold, lowThreshold * ratio, kernel_size); // canny detector

    /**
     * edgeMap postprocessing
     */
    std::vector<cv::Point> processedPoints;
    if(!closedConture && cornerList.size() > 0) {

        for(auto it1 = cornerList.begin(); it1 < cornerList.end(); it1++) {

            cv::Point nearestNeighbour;
            cv::Point initialPoint;
            double minDist = std::numeric_limits<float>::max();

            if(processedPoints.size() > 0) { // skip point if processed already

                if(std::find(processedPoints.begin(), processedPoints.end(), *it1) != processedPoints.end()) {

                    continue;

                }

            }

            for(auto it2 = cornerList.begin(); it2 < cornerList.end(); it2++) { // find the nearest corner

                if(*it1 == *it2) {

                    continue;

                }

                double abs = sqrt((it1->x - it2->x) * (it1->x - it2->x) + (it1->y -  it2->y) * (it1->y -  it2->y));

                if(abs < minDist) {

                    minDist = abs;
                    nearestNeighbour = *it2;

                }

            }

            initialPoint = *it1;

            // store the processed point pair
            processedPoints.emplace_back(nearestNeighbour);
            processedPoints.emplace_back(initialPoint);

//            ROS_INFO("initialPoint->x = %i, initialPoint->y = %i", initialPoint.x, initialPoint.y);
//            ROS_INFO("nearestNeighbour->x = %i, nearestNeighbour->y = %i", nearestNeighbour.x, nearestNeighbour.y);

            cv::LineIterator lineIterator(edgeMap, initialPoint, nearestNeighbour, 8);

            int sideLength = 2;

            for(auto i = 0; i < lineIterator.count; i++ , lineIterator++) { // move a kernel along the line

                cv::Point p1 = cv::Point(lineIterator.pos().x - sideLength, lineIterator.pos().y - sideLength);
                cv::Point p2 = cv::Point(lineIterator.pos().x + sideLength + 1, lineIterator.pos().y + sideLength + 1);
                cv::Mat kernel = edgeMap(cv::Rect(p1, p2));

                for(int idy = 0; idy < kernel.rows; idy++) { // dye area

                    for(int idx = 0; idx < kernel.cols; idx++) {

                        kernel.at<uchar>(idy,idx) = 0;

                    }

                }

            }

        }

    }

    /**
     * Edge clustering
     */
    cv::Mat labelsEdgeMap;
    int cntLabelsEdgeMap = cv::connectedComponents(edgeMap, labelsEdgeMap); // find connected components in the edgeMap

    /**
    * Edge dyeing
    */
    cv::Mat dyedBGR(image.rows, image.cols, CV_8UC3, cv::Scalar(dyeValue_half, dyeValue_half, dyeValue_half));
    cv::Vec3b gray = cv::Vec3b( dyeValue_half, dyeValue_half, dyeValue_half );
    cv::Vec3b black = cv::Vec3b( 0.0, 0.0, 0.0 );

    ROS_INFO("nLabels = %i", cntLabelsEdgeMap);

    // dye the edges
#pragma omp parallel for
    for(int idy = 0; idy < dyedBGR.rows; ++idy){

        for(int idx = 0; idx < dyedBGR.cols; ++idx) {

            int labelIdxEdgeMap = labelsEdgeMap.at<int>(idy, idx);
            
            cv::Vec3b &pixel = dyedBGR.at<cv::Vec3b>(idy, idx);

            if(labelIdxEdgeMap == 1) { // leave the background (labelIdxEdgeMap == 0)

//                pixel = cv::Vec3b( dyeValue_max, 0.0, 0.0 ); // blue (cw)
                pixel = cv::Vec3b( 0.0, 0.0, dyeValue_max ); // red (ccw)

            } else if (labelIdxEdgeMap == 2) {

//                pixel = cv::Vec3b( 0.0, 0.0, dyeValue_max ); // red (cw)
                pixel = cv::Vec3b( dyeValue_max, 0.0, 0.0 ); // blue (ccw)

            } else {

                if(image_binary.at<uchar>(idy,idx) == dyeValue_max) { // burn the whiteMap in

                    pixel = cv::Vec3b(dyeValue_max,dyeValue_max,dyeValue_max); // white

                }

            }

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