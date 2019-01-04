cmake_minimum_required(VERSION 2.8.3)
project(aura)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

# Source: tik0-GitHub/amiro_potential_field_navigation
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# System dependencies are found with CMake's conventions
find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
        amiro_description
        amiro_gazebo
        amiro_msgs
        roscpp
        rospy
        std_msgs
        message_generation
        sensor_msgs
        cv_bridge
        nav_msgs
        sensor_mockup
        image_transport
        rviz_textured_quads
        grid_map_core
        grid_map_msgs
        grid_map_cv
        grid_map_ros
        genmsg
        actionlib_msgs
        tf
        actionlib)

# Source: tik0-GitHub/amiro_potential_field_navigation
if (OPENMP_FOUND)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-deprecated-declarations -O3 -Wno-write-strings -std=c++11")

## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend tag for "message_generation"
##   * add a build_depend and a run_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependency has been pulled in
##     but can be declared for certainty nonetheless:
##     * add a run_depend tag for "message_runtime"
## * In this file (CMakeLists.txt):
##   * add "message_generation" and every package in MSG_DEP_SET to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * add "message_runtime" and every package in MSG_DEP_SET to
##     catkin_package(CATKIN_DEPENDS ...)
##   * uncomment the add_*_files sections below as needed
##     and list every .msg/.srv/.action file to be processed
##   * uncomment the generate_messages entry below
##   * add every package in MSG_DEP_SET to generate_messages(DEPENDENCIES ...)

# Generate messages in the 'msg' folder
add_message_files(
        FILES
        feedbackMessage.msg
        motionVectorMessage.msg)

# Generate services in the 'srv' folder
add_service_files(
        FILES
        navigation/check_rfid.srv
        navigation/get_rfid_type.srv
        schema_backend/initiate_image_routine.srv
        schema_backend/get_field_by_key.srv)

# Generate actions in the 'action' folder
add_action_files(
        DIRECTORY action FILES
        # tracing
        trace.action
        # motor schemas
        move_to_target.action
        stay_on_path.action
        wait_for_obstacle.action
        avoid_obstacle.action
        move_robot.action)

# Generate added messages and services with any dependencies listed here
generate_messages(
        DEPENDENCIES
        amiro_msgs std_msgs sensor_msgs actionlib_msgs)

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

## To declare and build dynamic reconfigure parameters within this
## package, follow these steps:
## * In the file package.xml:
##   * add a build_depend and a run_depend tag for "dynamic_reconfigure"
## * In this file (CMakeLists.txt):
##   * add "dynamic_reconfigure" to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * uncomment the "generate_dynamic_reconfigure_options" section below
##     and list every .cfg file to be processed

## Generate dynamic reconfigure parameters in the 'cfg' folder
# generate_dynamic_reconfigure_options(
#   cfg/DynReconf1.cfg
#   cfg/DynReconf2.cfg
# )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
        INCLUDE_DIRS include
        LIBRARIES aal_potential_field_navigation
        CATKIN_DEPENDS message_runtime amiro_msgs roscpp rospy std_msgs
        DEPENDS system_lib
)

###########
## Build ##
###########

#set(grid_map_INCLUDE_DIRS "~/catkin_ws/install/include")
set(grid_map_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/../../../install/include")

# Specify additional locations of header files
# Your package locations should be listed before other locations
include_directories(
        ${catkin_INCLUDE_DIRS}
        include/
        ${OpenCV_INCLUDE_DIRS}
        ${grid_map_INCLUDE_DIRS})

# Specify a custom library
set(AURA_LIB
        # motor schemas
        ${PROJECT_SOURCE_DIR}/src/motor_schemas/MotorSchema.cpp
        ${PROJECT_SOURCE_DIR}/src/motor_schemas/MoveToTarget.cpp
        ${PROJECT_SOURCE_DIR}/src/motor_schemas/StayOnPath.cpp
        ${PROJECT_SOURCE_DIR}/src/motor_schemas/MoveRobot.cpp
        ${PROJECT_SOURCE_DIR}/src/motor_schemas/WaitForObstacle.cpp
        ${PROJECT_SOURCE_DIR}/src/motor_schemas/AvoidObstacle.cpp
        # perceptual schemas
        ${PROJECT_SOURCE_DIR}/src/perceptual_schemas/GetRFID.cpp
        ${PROJECT_SOURCE_DIR}/src/perceptual_schemas/GetDirection.cpp
        ${PROJECT_SOURCE_DIR}/src/perceptual_schemas/GetPosition.cpp
        ${PROJECT_SOURCE_DIR}/src/perceptual_schemas/GetMotionVector.cpp
        ${PROJECT_SOURCE_DIR}/src/perceptual_schemas/GetAlignment.cpp
        ${PROJECT_SOURCE_DIR}/src/perceptual_schemas/GetRingValues.cpp
        ${PROJECT_SOURCE_DIR}/src/perceptual_schemas/GetFloorValues.cpp
        ${PROJECT_SOURCE_DIR}/src/perceptual_schemas/GetFeedback.cpp
        # utils
        ${PROJECT_SOURCE_DIR}/src/utils/image_converting_utils.cpp
        ${PROJECT_SOURCE_DIR}/src/utils/potential_field_utils.cpp
        ${PROJECT_SOURCE_DIR}/src/navigation/navigation_utils.cpp)

## Declare a C++ library
add_library(${PROJECT_NAME}_AURA_LIB ${AURA_LIB})

## Interdependencies
#target_link_libraries(${PROJECT_NAME}_AURA_LIB ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})

## Add cmake target dependencies of the library
## as an example, code may need to be generated before libraries
## either from message generation or dynamic reconfigure
# add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Declare a C++ executable and link libraries against
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide

#####################
## Motor Schema nodes

# move_to_target
add_executable(${PROJECT_NAME}_move_to_target_node src/move_to_target_node.cpp)
target_link_libraries(${PROJECT_NAME}_move_to_target_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_move_to_target_node ${PROJECT_NAME}_gencpp)

# stay_on_path
add_executable(${PROJECT_NAME}_stay_on_path_node src/stay_on_path_node.cpp)
target_link_libraries(${PROJECT_NAME}_stay_on_path_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_stay_on_path_node ${PROJECT_NAME}_gencpp)

# wait_for_obstacle
add_executable(${PROJECT_NAME}_wait_for_obstacle_node src/wait_for_obstacle_node.cpp)
target_link_libraries(${PROJECT_NAME}_wait_for_obstacle_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_wait_for_obstacle_node ${PROJECT_NAME}_gencpp)

# avoid_obstacle
add_executable(${PROJECT_NAME}_avoid_obstacle_node src/avoid_obstacle_node.cpp)
target_link_libraries(${PROJECT_NAME}_avoid_obstacle_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_avoid_obstacle_node ${PROJECT_NAME}_gencpp)

# move_robot
add_executable(${PROJECT_NAME}_move_robot_node src/move_robot_node.cpp)
target_link_libraries(${PROJECT_NAME}_move_robot_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_move_robot_node ${PROJECT_NAME}_gencpp)

#####################
## Other nodes

# image_converter
add_executable(${PROJECT_NAME}_image_converting_node src/image_converting/image_converting_node.cpp)
target_link_libraries(${PROJECT_NAME}_image_converting_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_image_converting_node ${PROJECT_NAME}_gencpp)

# map_representation
add_executable(${PROJECT_NAME}_beacon_node src/navigation/beacon_node.cpp)
target_link_libraries(${PROJECT_NAME}_beacon_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_beacon_node ${PROJECT_NAME}_gencpp)

# transformation
add_executable(${PROJECT_NAME}_dynamic_tf_broadcaster_node src/utils/dynamic_tf_broadcaster_node.cpp)
target_link_libraries(${PROJECT_NAME}_dynamic_tf_broadcaster_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_dynamic_tf_broadcaster_node ${PROJECT_NAME}_gencpp)

add_executable(${PROJECT_NAME}_dynamic_tf_with_odom_node src/utils/dynamic_tf_with_odom_node.cpp)
target_link_libraries(${PROJECT_NAME}_dynamic_tf_with_odom_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_dynamic_tf_with_odom_node ${PROJECT_NAME}_gencpp)

# RFID sensor mockup
add_executable(${PROJECT_NAME}_rfid_mockup_node src/utils/rfid_mockup_node.cpp)
target_link_libraries(${PROJECT_NAME}_rfid_mockup_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_rfid_mockup_node ${PROJECT_NAME}_gencpp)

# visualization
add_executable(${PROJECT_NAME}_vectorfield_to_gridmap_node src/visualization/vectorfield_to_gridmap_node.cpp)
target_link_libraries(${PROJECT_NAME}_vectorfield_to_gridmap_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_vectorfield_to_gridmap_node ${PROJECT_NAME}_gencpp)

add_executable(${PROJECT_NAME}_vectorfield_merger_node src/visualization/vectorfield_merger_node.cpp)
target_link_libraries(${PROJECT_NAME}_vectorfield_merger_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_vectorfield_merger_node ${PROJECT_NAME}_gencpp)

add_executable(${PROJECT_NAME}_finalGradient_marker_node src/visualization/finalGradient_marker_node.cpp)
target_link_libraries(${PROJECT_NAME}_finalGradient_marker_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_finalGradient_marker_node ${PROJECT_NAME}_gencpp)

add_executable(${PROJECT_NAME}_map_to_textured_quads_node src/visualization/map_to_textured_quads_node.cpp)
target_link_libraries(${PROJECT_NAME}_map_to_textured_quads_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_map_to_textured_quads_node ${PROJECT_NAME}_gencpp)

add_executable(${PROJECT_NAME}_gradient_orientation_plot_node src/visualization/gradient_orientation_plot_node.cpp)
target_link_libraries(${PROJECT_NAME}_gradient_orientation_plot_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_gradient_orientation_plot_node ${PROJECT_NAME}_gencpp)

add_executable(${PROJECT_NAME}_trace_node src/visualization/trace_node.cpp)
target_link_libraries(${PROJECT_NAME}_trace_node ${catkin_LIBRARIES} ${PROJECT_NAME}_AURA_LIB ${OpenCV_LIBS} ${OpenCV_LIBRARIES} ${cv_bridge_LIBRARIES} ${image_transport_LIBRARIES})
add_dependencies(${PROJECT_NAME}_trace_node ${PROJECT_NAME}_gencpp)

## Rename C++ executable without prefix
## The above recommended prefix causes long target names, the following renames the
## target back to the shorter version for ease of user use
## e.g. "rosrun someones_pkg node" instead of "rosrun someones_pkg someones_pkg_node"
# set_target_properties(${PROJECT_NAME}_node PROPERTIES OUTPUT_NAME node PREFIX "")

## Add cmake target dependencies of the executable
## same as for the library above
#add_dependencies(${PROJECT_NAME}_planning_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# install(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

# Mark executables and/or libraries for installation
install(TARGETS ${PROJECT_NAME}_AURA_LIB
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})

# Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME}/
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
        #   FILES_MATCHING PATTERN "*.h"
        #   PATTERN ".svn" EXCLUDE
        )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_aal_potential_field_navigation.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
