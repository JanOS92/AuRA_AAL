/**
 * ==============================================================================
 * \file map_to_textured_quads_node.cpp
 * \brief Provides a C++ rviz_textured_quads interface
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

// ROS
#include "ros/ros.h"
#include <sensor_msgs/Image.h>

// ROS - OpenCV_ Bridge
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Types and classes
#include "aal_potential_field_navigation/Types.h"
#include "aal_potential_field_navigation/utils/utils.h"
#include "aal_potential_field_navigation/Constants.h"
#include "aal_potential_field_navigation/utils/potential_field_utils.hpp"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

cv::Mat mapImage_mat(IMAGE_SIZE_sz, CV_8UC3, cv::Scalar(uchar(0)));
static image_transport::Publisher imagePublisher_c;

/// LOCAL FUNCTIONS

/// getImagePath_st
/// \param imageName_st
/// \return
string getImagePath_st(string imageName_st) {

    string final_image_path_st = IMAGE_DIRECTORY_PATH_st;
    final_image_path_st += "/" + imageName_st;
    final_image_path_st += IMAGE_TYPE_st;

    return final_image_path_st;

}

/// MAIN

/// main
/// \param argc_i
/// \param argv_pch
/// \return 0
int main(int argc_i, char *argv_pch[]) {

    // init ROS
    ros::init(argc_i, argv_pch, ros::this_node::getName());
    ros::NodeHandle node_c("~");
    ROS_INFO("Start: %s", ros::this_node::getName().c_str());
    ros::Rate loopRate_c(LOOP_RATE_i);

    // load map image as grayscale
    mapImage_mat = cv::imread(getImagePath_st(MAP_IMAGE_NAME_st), CV_LOAD_IMAGE_COLOR);
    cv::flip(mapImage_mat, mapImage_mat, 0); // flip around the x-axis

    // prepare image transmission
    cv_bridge::CvImage cvImage_c;
    image_transport::ImageTransport imageTransport_c(node_c);
    imagePublisher_c = imageTransport_c.advertise(MAP_IMAGE_TOPIC_st, 1, true);
    cvImage_c.encoding = sensor_msgs::image_encodings::BGR8;
    cvImage_c.image = mapImage_mat;

    while (ros::ok()) {

        imagePublisher_c.publish(cvImage_c.toImageMsg());

        loopRate_c.sleep();

    }

    return 0;

}