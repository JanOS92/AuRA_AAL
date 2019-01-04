/**
 * ==============================================================================
 * \file map_representation_node.cpp
 * \brief Receive all vectorfields and merge them.
 * \author Daniel Rudolph <drudolph@techfak.uni-bielefeld.de>
 * \author Timo Korthals <tkorthals@cit-ec.uni-bielefeld.de>
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

// ROS
#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

// ROS - OpenCV_ Bridge
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <image_transport/subscriber_filter.h>

// OpenCV
#include <opencv2/highgui/highgui.hpp>

// Utils
#include "aal_potential_field_navigation/utils/potential_field_utils.hpp"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

static image_transport::Publisher imagePublisher_c;

cv::Mat field1_mat, field2_mat;
static bool dataArrived1_b, dataArrived2_b;

/// LOCAL FUNCTIONS

void process1_v(const sensor_msgs::ImageConstPtr &msg_c) { // current

    dataArrived1_b = true;

    cv_bridge::toCvShare(msg_c, msg_c->encoding)->image.copyTo(field1_mat);

}

void process2_v(const sensor_msgs::ImageConstPtr &msg_c) { // charge

    dataArrived2_b = true;

    cv_bridge::toCvShare(msg_c, msg_c->encoding)->image.copyTo(field2_mat);

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

    // Initialize
    cv_bridge::CvImage cvImage_c;
    cvImage_c.encoding = sensor_msgs::image_encodings::TYPE_32FC2;
    cv::Mat vectorfieldMerged_mat;

    {
        cv::Mat dummy_mat = cv::Mat(cv::Size(DESIRED_VECTORFIELD_WIDTH_i, DESIRED_VECTORFIELD_HEIGTH_i), CV_32FC2, cv::Scalar(0.0f));
        dummy_mat.copyTo(field1_mat);
        dummy_mat.copyTo(field2_mat);
        dummy_mat.copyTo(vectorfieldMerged_mat);
    }

    image_transport::ImageTransport fusedTransport_c(node_c);
    image_transport::ImageTransport field1Transport_c(node_c);
    image_transport::ImageTransport field2Transport_c(node_c);

    imagePublisher_c = fusedTransport_c.advertise(VEC_FIELD_FINAL_TOPIC_st, 1, true);

    image_transport::Subscriber field1Sub_c = field1Transport_c.subscribe(VEC_FIELD_DIR_TOPIC_st, 1, &process1_v);
    image_transport::Subscriber field2Sub_c = field2Transport_c.subscribe(VEC_FIELD_ALIGN_TOPIC_st, 1, &process2_v);

    bool burnIn_b = true;
    ros::Rate r_c(LOOP_RATE_i);

    while (ros::ok()) {

        ros::spinOnce();

        if (!burnIn_b) {

            if (dataArrived1_b || dataArrived2_b) {

                dataArrived1_b = false;
                dataArrived2_b = false;
                ROS_INFO_STREAM(ros::this_node::getName() << " STATE: Do fusion");

                // Fuse
                if (field1_mat.cols != DESIRED_VECTORFIELD_WIDTH_i || field1_mat.rows != DESIRED_VECTORFIELD_HEIGTH_i ||
                    field2_mat.cols != DESIRED_VECTORFIELD_WIDTH_i || field2_mat.rows != DESIRED_VECTORFIELD_HEIGTH_i) {

                    ROS_WARN_STREAM(ros::this_node::getName()
                                            << " insufficient field size: (field1_mat.cols, field1_mat.rows, field2_mat.cols, field2_mat.rows, DESIRED_VECTORFIELD_WIDTH_i, DESIRED_VECTORFIELD_HEIGTH_i)=("
                                            <<
                                            field1_mat.cols << ", " << field1_mat.rows << ", " << field2_mat.cols << ", "
                                            << field2_mat.rows << ", " << DESIRED_VECTORFIELD_WIDTH_i << ", " << DESIRED_VECTORFIELD_HEIGTH_i << ")");
                    continue;

                }

                vectorfieldMerged_mat = field1_mat + field2_mat;

                // Publish
                cvImage_c.image = vectorfieldMerged_mat;

                imagePublisher_c.publish(cvImage_c.toImageMsg());

                // Show image
//                cv::Mat img_mat(vectorfieldMerged_mat.size(), CV_8UC3, cv::Scalar(DYE_VALUE_MAX_i, DYE_VALUE_MAX_i, DYE_VALUE_MAX_i)); // Debug only!
//                drawGradientOnImg_v(vectorfieldMerged_mat, img_mat); // Debug only!
//                cv::namedWindow("Vectorfield pixel", cv::WINDOW_AUTOSIZE); // Debug only!
//                cv::imshow("Vectorfield pixel", img_mat); // Debug only!
//                cv::waitKey(0); // Debug only!

            } else {


            }

        } else {

            if (dataArrived1_b && dataArrived2_b) {

                burnIn_b = false;

            }

        }

        r_c.sleep();

    }

    return 0;

}