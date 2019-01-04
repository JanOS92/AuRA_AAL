/**
 * ==============================================================================
 * \file rfid_mockup_node.cpp
 * \brief Provides a mockup of a RFID-Tag reader
 * \author Jan O'Sullivan <josullivan@techfak.uni-bielefeld.de>
 * ==============================================================================  */

/// INCLUDES

// ROS
#include "ros/ros.h"
#include <tuple>
#include "xmlrpcpp/XmlRpc.h"
#include <boost/algorithm/string.hpp>
#include "nav_msgs/Odometry.h"
#include "std_msgs/UInt32MultiArray.h"

// Types and classes
#include "aal_potential_field_navigation/perceptual_schemas/GetPosition.h"
#include "aal_potential_field_navigation/Constants.h"
#include "aal_potential_field_navigation/Types.h"

/// NAMESPACES

using namespace std;

/// LOCAL VARIABLES

ros::Publisher pub;

static std::vector<std::tuple<double, double, double, uint32_t>> tag_xyzi;

GetPosition *getPosition_c; // perceptual schema

static double max_tag_distance_m;

enum tag {

    x = 0,
    y = 1,
    z = 2,
    id = 3

};

/// LOCAL FUNCTIONS

/// getTags_v
/// \param n
/// \brief Get the RFID-Tags from the yaml-File
void getTags_v(ros::NodeHandle &n) {

//    ROS_INFO_STREAM("Start tag parsing"); // Debug only!

    std::string parsePrefix("tag_");
    std::vector<std::string> keys;
    n.getParamNames(keys);

    for (auto key = keys.begin(); key < keys.end(); ++key) {

        // continue if the parameter is not in our namespace
        try {

            if (key->compare(0, n.getNamespace().size(), n.getNamespace()) != 0) {

                ROS_DEBUG_STREAM(*key << " is not a parameter in the " << n.getNamespace() << " namespace => continue");

                continue;

            }

        } catch (...) {

            continue;

        }

        // Get rid of the namespace and check for the correct prefix
        std::vector<std::string> strs;
        boost::split(strs, *key, boost::is_any_of("/"));

        if (strs.back().size() < parsePrefix.size()) {

            ROS_DEBUG_STREAM(*key << " to short");

            continue;

        } else {

            ROS_DEBUG_STREAM(*key << " matches in length");
        }

        // parse the parameters
        if (strs.back().compare(0, parsePrefix.size(), parsePrefix) == 0) {

            try {

                ROS_DEBUG_STREAM(*key << " matches");

                XmlRpc::XmlRpcValue parsedList;

                n.getParam(*key, parsedList);

                ROS_ASSERT(parsedList.getType() == XmlRpc::XmlRpcValue::TypeArray);
                ROS_ASSERT(parsedList.size() == 4);  // s.t. 4 is the number of possible entries [x, y, z, id]
                ROS_ASSERT(parsedList[0].getType() == XmlRpc::XmlRpcValue::TypeDouble);
                ROS_ASSERT(parsedList[1].getType() == XmlRpc::XmlRpcValue::TypeDouble);
                ROS_ASSERT(parsedList[2].getType() == XmlRpc::XmlRpcValue::TypeDouble);
                ROS_ASSERT(parsedList[3].getType() == XmlRpc::XmlRpcValue::TypeInt);

                tag_xyzi.push_back(std::make_tuple<double, double, double, uint32_t>(
                        static_cast<double>(parsedList[0]),
                        static_cast<double>(parsedList[1]),
                        static_cast<double>(parsedList[2]),
                        static_cast<int>(parsedList[3])));

//                ROS_INFO_STREAM("Load " << *key << "with xyzi: " <<
//                                        std::get<tag::x>(*(tag_xyzi.begin())) << " " <<
//                                        std::get<tag::x>(*(tag_xyzi.begin())) << " " <<
//                                        std::get<tag::y>(*(tag_xyzi.begin())) << " " <<
//                                        std::get<tag::z>(*(tag_xyzi.begin())) << " "); // Debug only!

            } catch (XmlRpc::XmlRpcException a) {

                std::cerr << "XmlRpc exception: " << a.getMessage() << std::endl;

            }

        } else {

            ROS_DEBUG_STREAM(*key << " does not match");

        }

    }

    ROS_INFO_STREAM("End parsing");

}

/// callback_v
/// \param msg
/// \brief Odometry callback function
void callback_v(const nav_msgs::Odometry msg) {

    // iterate through all given tags
    std_msgs::UInt32MultiArray tags;

    for (auto it = tag_xyzi.begin(); it != tag_xyzi.end(); ++it) {

        Pose pose_c = getPosition_c->getPositionByFrame_c(FRAME_MAP_st);

//    const double x = msg.pose.pose.position.x - std::get<tag::x>(*it);
//    const double y = msg.pose.pose.position.y - std::get<tag::y>(*it);
//    const double z = msg.pose.pose.position.z - std::get<tag::z>(*it);
        const double x = pose_c.x - std::get<tag::x>(*it);
        const double y = pose_c.y - std::get<tag::y>(*it);

//        ROS_INFO("x = %f, y = %f", x , y); // Debug only!

//        if (sqrt(x * x + y * y + z * z) <= max_tag_distance_m) { // check distance in cartesian space
        if (sqrt(x * x + y * y) <= max_tag_distance_m) { // check distance only in xy-plane

//            ROS_INFO("abs = %f, tag = %i", sqrt(x * x + y * y), std::get<tag::id>(*it)); // Debug only!

            tags.data.push_back(std::get<tag::id>(*it));

        }
    }

    pub.publish(tags);

}

/// MAIN

/// main
/// \param argc_i
/// \param argv_pch
/// \return 0
int main(int argc_i, char *argv_pch[]) {

    delete (getPosition_c);

    // init ROS
    ros::init(argc_i, argv_pch, ros::this_node::getName());
    ros::NodeHandle node_c("~");
    ROS_INFO("Start: %s", ros::this_node::getName().c_str());
    ros::Rate loopRate_(LOOP_RATE_i);

    // parameter
    std::string topic_out, topic_in;
    node_c.param<std::string>("topic_in", topic_in, ODOM_AMIRO_TOPIC_st);
    node_c.param<std::string>("topic_out", topic_out, RFID_TAG_TOPIC_st);
    node_c.param<double>("max_tag_distance_m", max_tag_distance_m, 0.1);

    getPosition_c = new GetPosition(topic_in);

    getTags_v(node_c);
    ros::Subscriber sub = node_c.subscribe(topic_in, 1, callback_v);
    pub = node_c.advertise<std_msgs::UInt32MultiArray>(topic_out, 1);

    ros::spin(); // keep this node alive!

    return 0;

}
