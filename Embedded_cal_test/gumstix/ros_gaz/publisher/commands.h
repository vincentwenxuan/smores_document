/* This class is a wrapper class for our customized message. Any accessing or 
 * mutating actions on the cmd object (which is the class of the customized 
 * message) has to be done via this class. Of course we can also declare the cmd object 
 * in the main function, but this class serves the purpose of making the code neat. 
 * Any changes to the existing or new commands can be edited in the commands.cpp file.
 *
 *
 * Warning: This class depends on the Cmd.h, any changes to that file can have an impact
 * on the functionality of that file as well.
*/
#ifndef COMMANDS_WRAPPER_H
#define COMMANDS_WRAPPER_H

#include <vector>
#include "project_name/Cmd.h"
#include <string>

class CmdWrapper 
{ 
  public:
     //constructor that takes the number of commands (to be sent) as the argument
     CmdWrapper(int n);

     //A function to initialize the commands, but this is just a demo version, all it does is
     //drive Module_0 forward and backward
     void initialize_demo_cmd();
     void initialize_demo_cmd2();    
     //This function returns the name of the module set in the cmd object and it is based on
     // the command number which it takes as the parameter
     std::string getNameByCmdNumber(int n);

     //This function returns a single cmd object and it is based on the command number which 
     //it takes as the parameter
     project_name::Cmd getCmdByNumber(int n);


     /*NOTE: Implement and use this function instead of initialize_demo_cmd function if you
      *wish to send other commands to the ROS node in gazebo. Refer to the 
      *initialize_demo_cmd function on how to initialize each field. Note that the number
      *of commands you initialize should correspond to the NUM_OF_CMD macro in talker2.cpp
     */
     void initialize_cmd();

  private:
     //This creates a vector of cmd objects
     std::vector<project_name::Cmd> cmd_vector;
     //This field keeps track of the number of commands initialized in the CmdWrapper object
     int num_of_cmd;

};

#endif //COMMANDS_WRAPPER_H
