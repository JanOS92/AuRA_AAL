/**
 * ==============================================================================
 * \file trace_node.cpp
 * \brief Trace the position of the AMiRo and print it to the map image
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

// ROS
#include <ros/ros.h>

// ROS - OpenCV_ Bridge
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <image_transport/subscriber_filter.h>

// OpenCV
#include <opencv2/highgui/highgui.hpp>

// ROS action
#include "../../../../install/include/aal_potential_field_navigation/avoid_obstacleAction.h"
#include "../../../../install/include/aal_potential_field_navigation/traceAction.h"
#include <actionlib/server/simple_action_server.h>

// Utils
#include "aal_potential_field_navigation/Constants.h"
#include "aal_potential_field_navigation/utils/potential_field_utils.hpp"
#include <ctime>
#include <fstream>

// Types
#include "aal_potential_field_navigation/perceptual_schemas/GetPosition.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

typedef actionlib::SimpleActionServer<aal_potential_field_navigation::traceAction> TraceActionServer_c;
aal_potential_field_navigation::traceResult TraceResult_c;
boost::shared_ptr<const typename actionlib::SimpleActionServer<aal_potential_field_navigation::traceAction>::Goal> newGoalPtr_pc;

cv::Mat mapImage_mat(IMAGE_SIZE_sz, CV_8UC3);

/// LOCAL FUNCTIONS

/// getImagePath_st
/// \param imageName_st
/// \return Full image path
/// \brief Combine path components to a full image path
string getImagePath_st(string imageName_st) {

    string final_image_path_st = IMAGE_DIRECTORY_PATH_st;
    final_image_path_st += "/" + imageName_st;
    final_image_path_st += IMAGE_TYPE_st;

    return final_image_path_st;

}

/// space2underscore_st
/// \param text_st
/// \return "text_st" with '_' instead of ' '
/// \brief Replace all whitespaces ' ' of a string with underscores '_'
string space2underscore_st(string text_st)
{
    for(int i = 0; i < text_st.length(); i++) {

        if (text_st[i] == ' ') {

            text_st[i] = '_';

        }

    }

    return text_st;
}

/// saveTraceData_v
/// \param pntBuffer_vec
/// \param filePath_st
/// \brief Save the content of the pntBuffer_vec to a trace file
void saveTraceData_v(std::vector<cv::Point2i> pntBuffer_vec, string filePath_st) {

    std::ofstream output_file((filePath_st + ".dat"));
    std::ostream_iterator<cv::Point2i> output_iterator(output_file, "\n");
    std::copy(pntBuffer_vec.begin(), pntBuffer_vec.end(), output_iterator);

}

/// avoidObstacleAction_v
/// \param goal_c
/// \param actionServer_c
void traceAction_v(const aal_potential_field_navigation::traceGoalConstPtr &goal_c, TraceActionServer_c *actionServer_c) {

    ros::Rate loopRate_c(LOOP_RATE_i);

    mapImage_mat = cv::imread(getImagePath_st(MAP_IMAGE_NAME_st), CV_LOAD_IMAGE_COLOR); // load map image as color image

    GetPosition getPosition_c(ODOM_AMIRO_TOPIC_st);

    geometry_msgs::Pose odomPose_c;

    std::vector<cv::Point2i> pntBuffer_vec;

    while (ros::ok()) { // execution loop

        ros::spinOnce();

        // get the position of the AMiRo inside the map image in pixel coordinates
        Pose pose_s = getPosition_c.getPositionByFrame_c(FRAME_POT_FIELD_st);
        odomPose_c.position.x = pose_s.x;
        odomPose_c.position.y = pose_s.y;

        pntBuffer_vec.push_back(cv::Point2i(pose2pixel(odomPose_c, IMAGE_SIZE_sz.width, IMAGE_SIZE_sz.height, METER_PER_PIXEL_WIDTH_d, METER_PER_PIXEL_HEIGHT_d))); // get the new pose inside the map image

        if (actionServer_c->isPreemptRequested()) { // check if the goal are canceled by the client

            const cv::Point *pts = (const cv::Point*) cv::Mat(pntBuffer_vec).data;
            int npts = cv::Mat(pntBuffer_vec).rows;

            // mark trace on image
            // HowToUse:
            // polylines(Mat& img, const Point* const* pts, const int* npts, int ncontours, bool isClosed, const Scalar& color, int thickness=1, int lineType=LINE_8, int shift=0 )
            cv::polylines(mapImage_mat, &pts, &npts, 1, false, RED_vec, 2, CV_AA, 0); // mark the travelled pixel position with red color

            // save trace as image an .dat file
            time_t now = time(0); // current date/time based on current system
            string dt = ctime(&now); // convert now to string form
#if TWB == 1
            std::string savePath_st = MEDIA_DIRECTORY_PATH_st + "traces/" + "TWB_";
#elif TWB == 0
            std::string savePath_st = MEDIA_DIRECTORY_PATH_st + "traces/" + "Gazebo_";
#endif
            savePath_st += space2underscore_st(dt);
            saveTraceData_v(pntBuffer_vec, savePath_st);
            savePath_st +=  ".jpg";
            cv::imwrite(savePath_st, mapImage_mat);

//            cv::namedWindow( "mapImage_mat", cv::WINDOW_AUTOSIZE); // Debug only!
//            cv::imshow( "mapImage_mat", mapImage_mat ); // Debug only!
//            cv::waitKey(0); // Debug only!

            delete(pts);

            break;

        }

        loopRate_c.sleep();

    }

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
    ros::Rate loopRate_c(1.0); // keep the trace vector smaller

    // ROS action (server)
    TraceActionServer_c actionServer_c(node_c, TRACE_st, boost::bind(&traceAction_v, _1, &actionServer_c), false);
    actionServer_c.start();

    ROS_INFO("[%s] Trace action server is online", ros::this_node::getName().c_str());

    ros::spin(); // keep this node alive

    return 0;

}
