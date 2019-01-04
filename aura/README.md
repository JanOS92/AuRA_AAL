# AuRA (Autonomous Robot Architecture)

## Description
This project includes a modern implementation of the *AuRA* within the *ROS* framework. The *AuRA* was defined by Ronald C. Arkin and Tucker Balch in 1997 and represent the first hybrid robot architecture. For further information please take a look at the following paper:

*AuRA: Principles and Practice in Review, Ronald C. Arkin and Tucker Balch, College of Computing Georgia Institute of Technology Atlanta, Georgia 30332*

## How To
### Setup
Please have a look at the project README.

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
ToDo

[![TWB](https://img.youtube.com/vi/U0d7B_Q4kNs/0.jpg)](https://www.youtube.com/watch?v=U0d7B_Q4kNs&t=2s)

### Add motor schemas
ToDo

### Add perceptual schemas
ToDo
