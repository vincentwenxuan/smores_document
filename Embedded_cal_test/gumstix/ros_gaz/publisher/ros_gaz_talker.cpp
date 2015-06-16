#include "ros/ros.h"
#include "std_msgs/String.h"
#include "commands.h"
#include <sstream>

//This number defines the number of commands which would be published by this node.
//Since we are calling the initialize_demo_cmd function right now, increasing this
//number would just increase the number of times the module moves back and forth
#define NUM_OF_CMD 8

//This function gets called when the a message has been sent to the subscribed topic
void chatterCallback(const std_msgs::String::ConstPtr& recv_msg)
{
  ROS_INFO("I heard: [%s]", recv_msg->data.c_str());
}

int main(int argc, char **argv)
{
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line. For programmatic
   * remappings you can use a different version of init() which takes remappings
   * directly, but for most command-line programs, passing argc and argv is the easiest
   * way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "talker");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle p, s;

  //This sets the rate at which this node would publish the command
  ros::Rate loop_rate(10);

  /**
   * The advertise() function is how you tell ROS that you want to
   * publish on a given topic name. This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing. After this advertise() call is made, the master
   * node will notify anyone who is trying to subscribe to this topic name,
   * and they will in turn negotiate a peer-to-peer connection with this
   * node.  advertise() returns a Publisher object which allows you to
   * publish messages on that topic through a call to publish().  Once
   * all copies of the returned Publisher object are destroyed, the topic
   * will be automatically unadvertised.
   *
   * The second parameter to advertise() is the size of the message queue
   * used for publishing messages.  If messages are published more quickly
   * than we can send them, the number here specifies how many messages to
   * buffer up before throwing some away.
   */
  ros::Publisher chatter_pub = p.advertise<gaz_ros_world_msg::Cmd>("chatter", 1000);
  //The following function allows this node to subscribe as well
  ros::Subscriber sub = s.subscribe("chatter2", 1000, chatterCallback);

  //This counter is used to keep track of the commands that we publish
  int counter=1;
  //Creates an instance of CmdWrapper, it contains the cmd objects which we send to gazebo
  CmdWrapper demo_cmd(NUM_OF_CMD);
  //Initializes the cmd objects
  demo_cmd.initialize_demo_cmd2();

  //This is important to wait for a subscriber to be present in the system so that the messages would not be lost 
  ros::Rate poll_rate(100);
  while(chatter_pub.getNumSubscribers() == 0)
    poll_rate.sleep();

  /**
   * A count of how many messages we have sent. This is used to create
   * a unique string for each message.
   */
  while (ros::ok())
  {
    //Transmit only the commands we have in the system and just subscribe after that
    if(counter<=NUM_OF_CMD){
    /**
     * This is a message object. You stuff it with data, and then publish it.
     */
    
	ROS_INFO("Publishing Command %d for [%s]", counter, demo_cmd.getNameByCmdNumber(counter).c_str());

    /**
     * The publish() function is how you send messages. The parameter
     * is the message object. The type of this object must agree with the type
     * given as a template parameter to the advertise<>() call, as was done
     * in the constructor above.
     */
    	chatter_pub.publish(demo_cmd.getCmdByNumber(counter));
    }
    //executes any callback functions waiting in the node
    ros::spinOnce();
    //limit the speed which we publish
    loop_rate.sleep();
    counter++;
  }


  return 0;
}
