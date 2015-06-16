#include "WorldController.hh"


namespace gazebo{
	
	
  	void WorldController::chatterCallback(const project_name::Cmd& recv_cmd)
	{
	  ROS_INFO("Receiving info for Module: [%s]",recv_cmd.module_name.c_str());
	  this->module_name = recv_cmd.module_name;
	  for(int i=0; i<4; i++){
		if(recv_cmd.flag[i]==3)
		       this->flag[i]=true;
		this->gait_value[i] = (double) recv_cmd.gait_value[i];
	  }
	  SendGaitTable(GetModulePtrByName(this->module_name), this->flag, this->gait_value, 3);
	}
	WorldController::WorldController()
	    :WorldServer()
	{
	  int arg = 0;
	  ros::init( arg, NULL, "Gaz_ros_node");
	} // WorldController::WorldController

	WorldController::~WorldController(){} // WorldController::~WorldController

	void WorldController::ExtraInitializationInLoad(physics::WorldPtr _parent, sdf::ElementPtr _sdf)
	{
	  ros::NodeHandle n;
	  this->sub =n.subscribe("chatter", 1001, &WorldController::chatterCallback, this);
	  // Build initial configuration from file
	  BuildConfigurationFromXML(INTIALCONFIGURATION);

	  // Here is the test for dynamic shared libraries
	  // TODO: Need to be removed after testing
	} // WorldController::ExtraInitializationInLoad


	void WorldController::OnSystemRunningExtra(const common::UpdateInfo & _info)
	{

	   ros::spinOnce();
	}

	void WorldController::ExtraWorkWhenModelInserted(CommandMessagePtr &msg)
	{
	  if (GetInitialJointSequenceSize() == 1) {
	    // Confiuration connection initialized
	    BuildConnectionFromXML(INTIALCONFIGURATION);
	    cout<<"World: Build the connection"<<endl;
	    // ReadFileAndGenerateCommands("Commands");
		
	    cout<<"World: Command been sent"<<endl;
	  }
	} // WorldController::ExtraWorkWhenModelInserted
	// Register this plugin with the simulator
	GZ_REGISTER_WORLD_PLUGIN(WorldController)
} // namespace gazebo
