#!/bin/bash
# This is a demo to illustrate a detached tmux session is capable of running multiple ros nodes. This script starts two tmux windows in a detached tmux session named "ros_brain". One of the windows starts a ROS turtlesim node while the other starts a publisher to control that turtlesim node. To bring up the detached tmux session, simply type "tmux attach" in the terminal. 

source ~/catkin_ws/devel/setup.bash

start_tmux()
{
    if ! tmux has-session -t ros_brain; then
	tmux new-session -d -s ros_brain -n ros_brain
	echo "Starting ros turtle node"
	tmux send-keys -t ros_brain:0 'rosrun turtlesim turtlesim_node' Enter
ince I don't have any experience with CMake files, I don't know of a more efficient way to do it
	echo "Starting ros control node"
	tmux new-window -t ros_brain:1 -n ros_control
	tmux send-keys -t ros_brain:1 'rosrun turtlesim draw_square' Enter
    else
	echo "Session ros_brain exists!"
    fi
}


if [ `rosnode list | grep -o rosout` ]; then
    start_tmux
else
    echo `rosnode list`
fi


exit 0
