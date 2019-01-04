/**
 * ==============================================================================
 * \file map_representation_node.cpp
 * \brief Recieve a vectorfield and creates a grid_map
 * \author Daniel Rudolph <drudolph@techfak.uni-bielefeld.de>
 * \author Timo Korthals <tkorthals@cit-ec.uni-bielefeld.de>
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

// ROS
#include <ros/ros.h>
#include <sensor_msgs/Image.h>

// ROS - OpenCV_ Bridge
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>

// Grid Map
#include <grid_map_msgs/GridMap.h>
#include <grid_map_core/grid_map_core.hpp>
#include <grid_map_cv/GridMapCvConverter.hpp>
#include <grid_map_ros/GridMapRosConverter.hpp>

//OpenCV
#include <opencv2/highgui/highgui.hpp>

// Utils and Types
#include "aal_potential_field_navigation/Types.h"
#include "aal_potential_field_navigation/utils/utils.h"
#include "aal_potential_field_navigation/Constants.h"
#include "aal_potential_field_navigation/utils/potential_field_utils.hpp"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

image_transport::Publisher rgbImagePublisher_c;
ros::Publisher gridMapPublisher_c;

/// LOCAL FUNCTIONS

/// getMapPos_c
/// \param listener_c
/// \return center position of the potential field
/// \brief Return the center position of the potential field
grid_map::Position getMapPos_c(tf::TransformListener &listener_c) {

    geometry_msgs::Pose odomPose_tf;

    tf::StampedTransform transform_c; // transformation between the robot and the vector field

    try {

        listener_c.waitForTransform(FRAME_MAP_st, FRAME_POT_FIELD_st, ros::Time(0),
                                          TIME_TO_WAIT_OF_STP_c); // wait for transformation

        listener_c.lookupTransform(FRAME_MAP_st, FRAME_POT_FIELD_st, ros::Time(0),
                                         transform_c); // apply transformation between the tfs

    } catch (tf::TransformException ex) {

        ROS_ERROR("[%s] %s", ros::this_node::getName().c_str(), ex.what());

        return grid_map::Position(0.0, 0.0);

    }

    return grid_map::Position(transform_c.getOrigin().x(), transform_c.getOrigin().y()); // return the new x and y coordinate inside the vectorfield frame

}

/// displayHSV_v
/// \param hsv_mat
/// \param showHue_b
/// \param showSat_b
/// \param showVal_b
/// \brief Display the hue, saturation and value of a hsv
void displayHSV_v(const cv::Mat hsv_mat, bool showHue_b = false, bool showSat_b = false, bool showVal_b = false) {

    cv::Mat hsv_amat[3];
    cv::Mat hue_mat;
    cv::Mat sat_mat;
    cv::Mat val_mat;

    // split hue, saturation and value of hsv
    cv::split(hsv_mat, hsv_amat);

    hue_mat = hsv_amat[0];
    sat_mat = hsv_amat[1];
    val_mat = hsv_amat[2];

    if(showHue_b) {

        cv::namedWindow( "hue", cv::WINDOW_AUTOSIZE); // Debug only!
        cv::imshow( "hue", hue_mat ); // Debug only!
        cv::waitKey(0); // Debug only!

    }

    if(showSat_b) {

        cv::namedWindow( "sat", cv::WINDOW_AUTOSIZE); // Debug only!
        cv::imshow( "sat", sat_mat ); // Debug only!
        cv::waitKey(0); // Debug only!

    }

    if(showVal_b) {

        cv::namedWindow( "val", cv::WINDOW_AUTOSIZE); // Debug only!
        cv::imshow( "val", val_mat ); // Debug only!
        cv::waitKey(0); // Debug only!

    }

}

void process_v(const sensor_msgs::ImageConstPtr &msg_c) {

    tf::TransformListener listener_c;

    cv::Mat vectorField_mat = cv_bridge::toCvShare(msg_c, msg_c->encoding)->image;

    rot90(vectorField_mat, RotateFlags::ROTATE_90_COUNTERCLOCKWISE); // rotate the vectorfield (~ image)

    flipVectors_v(vectorField_mat); // transform all vectors into the world frame

    cv::Mat hsv_mat = vectorfield_to_hsv(vectorField_mat, ros::this_node::getName());
    cv::Mat rgb_mat = hsv_to_rgb(hsv_mat);

//    cv::namedWindow( "rgb_mat", cv::WINDOW_AUTOSIZE); // Debug only!
//    cv::imshow( "rgb_mat", rgb_mat ); // Debug only!
//    cv::waitKey(0); // Debug only!

    // Get the color
    cv_bridge::CvImage cvImage_c;
    cvImage_c.header = msg_c->header;
    cvImage_c.encoding = sensor_msgs::image_encodings::RGB8;
    cvImage_c.image = rgb_mat;
    sensor_msgs::ImagePtr rgbImage_c = cvImage_c.toImageMsg();
    rgbImagePublisher_c.publish(rgbImage_c);

    // Get the height
    cv::Mat mono_mat = hsv_to_gray(hsv_mat);
    cv_bridge::CvImage cvImageMono_c;
    cvImageMono_c.header = msg_c->header;
    cvImageMono_c.encoding = sensor_msgs::image_encodings::MONO8;
    cvImageMono_c.image = mono_mat;
    sensor_msgs::ImagePtr monoImage_c = cvImageMono_c.toImageMsg();

    // Format the grid map
    grid_map::GridMap gridmap_c;
    grid_map::GridMapRosConverter::initializeFromImage(*rgbImage_c, (METER_PER_PIXEL_WIDTH_d + METER_PER_PIXEL_HEIGHT_d) / 2.0, gridmap_c);

    const grid_map::Position pos_c = getMapPos_c(listener_c); // get the center position
    gridmap_c.setPosition(pos_c); // set the gridmap position

    grid_map::GridMapRosConverter::addLayerFromImage(*monoImage_c, ELEVATION_str, gridmap_c);
    grid_map::GridMapRosConverter::addColorLayerFromImage(*rgbImage_c, COLOR_str, gridmap_c);

    grid_map_msgs::GridMap gridmapMsg_c;
    grid_map::GridMapRosConverter::toMessage(gridmap_c, gridmapMsg_c);

    // Publish the grid map
    ROS_INFO("[%s] publishing gridmap ....", ros::this_node::getName().c_str());
    gridmapMsg_c.info.header.frame_id = FRAME_MAP_st;
    gridMapPublisher_c.publish(gridmapMsg_c);

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
    ros::Rate loopRate_(LOOP_RATE_i);

    // ROS (image transport) subscriber
    image_transport::ImageTransport imageTransport(node_c);
    image_transport::Subscriber imageSub_c = imageTransport.subscribe(VEC_FIELD_FINAL_TOPIC_st, 1, &process_v);

    // ROS publisher
    gridMapPublisher_c = node_c.advertise<grid_map_msgs::GridMap>(GRIDMAP_TOPIC_st, 1, true);
    rgbImagePublisher_c = imageTransport.advertise(RGB_TOPIC_st, 1, true);

    ros::spin(); // keep this node alive

    return 0;

}