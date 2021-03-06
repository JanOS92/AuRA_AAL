cmake_minimum_required(VERSION 2.8.3)
project(potential_field_navigation)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread)
find_package(catkin REQUIRED COMPONENTS
  amiro_description
  amiro_gazebo
  amiro_msgs
  grid_map_core
  grid_map_msgs
  grid_map_cv
  grid_map_ros
  nav_msgs
  roscpp
  rospy
  rviz_textured_quads
  sensor_mockup
  sensor_msgs
  std_msgs
  cv_bridge
  image_transport
  )

catkin_package(
)


if (OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif ()
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-deprecated-declarations -O3 -Wno-write-strings -std=c++11")

set(grid_map_INCLUDE_DIRS "~/catkin_ws2/install/include")

include_directories(
  ${catkin_INCLUDE_DIRS}
  include/
  ${QT_USE_FILE}
  ${OPENCV_INCLUDE_DIR}
  ${grid_map_INCLUDE_DIRS}
)

#image_to_vectorfield_current_node
add_executable(image_to_vectorfield_current_node
        src/image_to_vectorfield_current_node.cpp)

target_link_libraries(image_to_vectorfield_current_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS})

#image_to_vectorfield_charge_node
add_executable(image_to_vectorfield_charge_node
        src/image_to_vectorfield_charge_node.cpp)

target_link_libraries(image_to_vectorfield_charge_node
        ${catkin_LIBRARIES}
        ${OpenCV_LIBS})

#vectorfield_to_gridmap_node
add_executable(vectorfield_to_gridmap_node
  src/vectorfield_to_gridmap_node.cpp)

target_link_libraries(vectorfield_to_gridmap_node
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS})

#image_selecter_node
add_executable(image_selecter_node
  src/image_selecter_node.cpp)

target_link_libraries(image_selecter_node
  ${catkin_LIBRARIES}
  Qt5::Widgets
  ${Boost_LIBRARIES})

#edge_separation_node
add_executable(edge_separation_node
        src/edge_separation_node.cpp)

target_link_libraries(edge_separation_node
        ${catkin_LIBRARIES}
        ${OpenCV_LIBS})

#redyeing_charge_node
add_executable(redyeing_charge_node
        src/redyeing_charge_node.cpp)

target_link_libraries(redyeing_charge_node
        ${catkin_LIBRARIES}
        ${OpenCV_LIBS})

#odom_to_vectorfield_node
add_executable(odom_to_vectorfield_node
  src/odom_to_vectorfield_node.cpp)

target_link_libraries(odom_to_vectorfield_node
  ${catkin_LIBRARIES})

#vectorfield_merger_node
add_executable(vectorfield_merger_node
  src/vectorfield_merger_node.cpp)

target_link_libraries(vectorfield_merger_node
  ${catkin_LIBRARIES})

#vectorfield_to_kinematic_node
add_executable(vectorfield_to_kinematic_node
  src/vectorfield_to_kinematic_node.cpp)

target_link_libraries(vectorfield_to_kinematic_node
  ${catkin_LIBRARIES})

#pixel_odom_to_pseudo_metric_odom_node
add_executable(pixel_odom_to_pseudo_metric_odom_node
  src/pixel_odom_to_pseudo_metric_odom_node.cpp)

target_link_libraries(pixel_odom_to_pseudo_metric_odom_node
  ${catkin_LIBRARIES})

#image_viewer_node
add_executable(image_viewer_node
  src/image_viewer_node.cpp)

target_link_libraries(image_viewer_node
  Qt5::Widgets
  ${catkin_LIBRARIES}
  ${Boost_LIBRARIES})

file(COPY "patter" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

