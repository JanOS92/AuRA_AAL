# AuRA (Autonomous Robot Architecture)

## Description
This project includes a modern implementation of the *AuRA* within the *ROS* framework. The *AuRA* was defined by Ronald C. Arkin and Tucker Balch in 1997 and represent the first hybrid robot architecture. For further information please take a look at the following paper:

*AuRA: Principles and Practice in Review, Ronald C. Arkin and Tucker Balch, College of Computing Georgia Institute of Technology Atlanta, Georgia 30332*

The overall application of the described implementation is called *AMiRo Assembly Line* or in short *AAL*. The *AMiRo* shall act as a transporation vehicle on a robotic assembly line. The assembly line depict a route network like this one shown in the picture or videos below.

..picture..

## How To
### Setup
Please have a look at the project README.md.

### Build
#### Gazebo
* open ./include/aal_potential_field_navigation/Constants.h
* change the TWB flag: 
```cpp
#define TWB 0
```
* go into your catkin_ws: `cd ~/{path}`
* build: `catkin_make -DCMAKE_BUILD_TYPE=Release`

#### TWB
* open ./include/aal_potential_field_navigation/Constants.h
* change the TWB flag: 
```cpp
#define TWB 1
```
* go into your workspace: `cd ~/{path}`
* build: `catkin_make -DCMAKE_BUILD_TYPE=Release`

### Launch
#### Gazebo
1. go into your catkin_ws: `cd ~/{path to your workspace}`
2. source the *ROS* environment: `source devel/setup.bash`
3. launch a *AMiRo* world: `roslaunch aura amiro_aal_gazebo` or `roslaunch aura amiro_automatica_gazebo`
4. open a new terminal and repeat steps 1. and 2.
5. launch a helper node: `roslaunch aura helper_launcher_gazebo`
6. open a new terminal and repeat steps 1. and 2.
7. launch a main node: `roslaunch aura apfn_launcher_gazebo`
6. open a new terminal and go into the scripts folder: `cd ~/{path to your workspace}/src/AuRA_AAL/aura/scripts`
7. start the execution: `python ./AMiRoA_fsm.py`

[![Gazebo](https://img.youtube.com/vi/XzC6PFxVVj8/0.jpg)](https://www.youtube.com/watch?v=XzC6PFxVVj8&t=2s)

#### TWB
1. prepare the *AMiRo(s)* for remote control (TWB), have a look at the project README.md.
2. go into your catkin_ws: `cd ~/{path to your workspace}`
3. source the *ROS* environment: `source devel/setup.bash`
4. launch a helper node: `roslaunch aura helper_launcher_TWB`
5. open a new terminal and repeat steps 2. and 3.
6. launch a main node: `roslaunch aura apfn_launcher_TWB`
7. open a new terminal and go into the scripts folder: `cd ~/{path to your workspace}/src/AuRA_AAL/aura/scripts`
8. start the execution: `python ./AMiRoA_fsm.py`

[![TWB](https://img.youtube.com/vi/U0d7B_Q4kNs/0.jpg)](https://www.youtube.com/watch?v=U0d7B_Q4kNs&t=2s)

### Add motor schemas
This *HowTo* describes only how a new motor schema has to be integrated into the architecture environment. The creation of a new motor schema class can easily oriented at the examples. Please keep in mind that you've to add new classes to the custom library in the CMakeLists.txt.

#### Schema Controller
1. go into the action folder: `cd ~/{path to your workspace}/src/AuRA_AAL/aura/action`
2. create a new action file for your motor schema by defining goal (high level information), result and feedback parameters: `touch {name}.action && echo "{content} >> {name}.action"`
3. register the action file in the CMakeLists.txt:
```xml
# Generate actions in the 'action' folder
add_action_files(
        DIRECTORY action FILES
        ...
        # motor schemas
        {name}.action
)
```
4. go into your catkin_ws: `cd ~/{path}`
5. build: `catkin_make -DCMAKE_BUILD_TYPE=Release`
6. go into your src folder: `cd ~/{path to your workspace}/src/AuRA_AAL/aura/src`
7. create a new node cpp file: `touch {name}.cpp`
8. open the new node file and use the following template:
```cpp

...

/// INCLUDES

// ROS
#include "ros/ros.h"

// ROS action
#include "../../../../install/include/aal_potential_field_navigation/{name}Action.h"
#include <actionlib/server/simple_action_server.h>

...

/// LOCAL VARIABLES

typedef actionlib::SimpleActionServer<aal_potential_field_navigation::{name}Action> {Name}ActionServer_c;
aal_potential_field_navigation::{name}Result {Name}Result_c;
boost::shared_ptr<const typename actionlib::SimpleActionServer<aal_potential_field_navigation::{name}Action>::Goal> newGoalPtr_pc;

...

/// ROS ACTION

/// {name}Action_v
/// \param goal_c
/// \param actionServer_c
void {name}Action_v(const aal_potential_field_navigation::{name}GoalConstPtr &goal_c,
{Name}ActionServer_c *actionServer_c) {

 while (ros::ok()) { // execution loop
 
  // motor schema execution
 
 }
 
 {Name}Result_c.errorCode_i = NO_ERROR; // quit the job without causing an error handling
 actionServer_c->setSucceeded({Name}Result_c); // set the server state to "Succeeded"

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

    // ROS action (server)
    {Name}ActionServer_c actionServer_c(node_c, {Name}, boost::bind(&{name}Action_v, _1, &actionServer_c), false);
    actionServer_c.start();

    ROS_INFO("[%s] {Name} action server is online", ros::this_node::getName().c_str());

    ros::spin(); // keep this node alive

    return 0;

}
```
9. build: `catkin_make -DCMAKE_BUILD_TYPE=Release`

#### Plan Sequencer


### Add perceptual schemas
ToDo
