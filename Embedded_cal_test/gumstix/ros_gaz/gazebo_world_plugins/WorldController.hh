#ifndef _GAZEBO_WORLD_CONTROLLER_HH_
#define _GAZEBO_WORLD_CONTROLLER_HH_

#include "WorldServer.hh"
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "project_name/Cmd.h"

#define INTIALCONFIGURATION "InitialConfiguration"

namespace gazebo{

class WorldController : public WorldServer
{
 public:
  WorldController();
  ~WorldController();
  /// This function will perform extra initialization
  /// Need to read in initial configuration here
  virtual void ExtraInitializationInLoad(physics::WorldPtr _parent, 
      sdf::ElementPtr _sdf);
  /// Need to read in initial Gait table here
  virtual void ExtraWorkWhenModelInserted(CommandMessagePtr &msg);
  virtual void OnSystemRunningExtra(const common::UpdateInfo & _info);
  void chatterCallback(const project_name::Cmd& recv_cmd);
  
 private:
  event::ConnectionPtr updateConnection;
  ros::Publisher pub_node;
  ros::Subscriber sub;
 public:
  string module_name;
  bool flag[4];
  double gait_value[4];
//  project_name::Cmd cmd;
  // transport::SubscriberPtr configSub;
  // transport::PublisherPtr configPub;
};
} // namespace gazebo
#endif
