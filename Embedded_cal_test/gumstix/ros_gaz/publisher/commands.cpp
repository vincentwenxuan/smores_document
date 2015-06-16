/* This class is a wrapper class for our customized message. Any accessing or 
 * mutating actions on the cmd object (which is the class of the customized 
 * message) has to be done via this class. Of course we can also declare the cmd object 
 * in the main function, but this class serves the purpose of making the codes neat. 
 * Any changes to the existing or new commands can be edited in the commands.cpp file.
 *
 *
 * Warning: This class depends on the Cmd.h, any changes to that file can have an impact
 * on the functionality of that file as well.
*/

#include "commands.h"

//constructor that takes the number of commands (to be sent) as the argument
CmdWrapper::CmdWrapper(int n)
{
  num_of_cmd = n;
  cmd_vector.resize(num_of_cmd);
}

//A function to initialize the commands, but this is just a demo version, all it does is
//drive Module_0 forward and backward
void CmdWrapper::initialize_demo_cmd()
{
    int iter=0;
    while(iter<num_of_cmd)
    {
	cmd_vector[iter].module_name = "Module_0";
	cmd_vector[iter].flag[0] = 0;
	cmd_vector[iter].flag[1] = 3;
	cmd_vector[iter].flag[2] = 3;
	cmd_vector[iter].flag[3] = 3;
	cmd_vector[iter].gait_value[0] = 0;
	cmd_vector[iter].time_stamp = 0;
	cmd_vector[iter].condition_str = "a";
        if(iter%2==0){
	    cmd_vector[iter].gait_value[1] = 22.5663706144;
	    cmd_vector[iter].gait_value[2] = 22.5663706144;
	    cmd_vector[iter].gait_value[3] = 1.57079632679;
	}
	else{
	    cmd_vector[iter].gait_value[1] = 0;
	    cmd_vector[iter].gait_value[2] = 0;
	    cmd_vector[iter].gait_value[3] = 0;
	}
	iter++;
    }
}

void CmdWrapper::initialize_demo_cmd2()
{

    cmd_vector[0].module_name = "Module_0";
    cmd_vector[0].flag[0] = 0;
    cmd_vector[0].flag[1] = 3;
    cmd_vector[0].flag[2] = 3;
    cmd_vector[0].flag[3] = 3;
    cmd_vector[0].gait_value[0] = 0;
    cmd_vector[0].gait_value[1] = 22.5663706144;
    cmd_vector[0].gait_value[2] = 22.5663706144;
    cmd_vector[0].gait_value[3] = 1.57079632679;
    cmd_vector[0].time_stamp = 0;
    cmd_vector[0].condition_str = "a";

    cmd_vector[1].module_name = "Module_4";
    cmd_vector[1].flag[0] = 0;
    cmd_vector[1].flag[1] = 3;
    cmd_vector[1].flag[2] = 3;
    cmd_vector[1].flag[3] = 3;
    cmd_vector[1].gait_value[0] = 0;
    cmd_vector[1].gait_value[1] = 22.5663706144;
    cmd_vector[1].gait_value[2] = 22.5663706144;
    cmd_vector[1].gait_value[3] = 1.57079632679;
    cmd_vector[1].time_stamp = 0;
    cmd_vector[1].condition_str = "a";

    cmd_vector[2].module_name = "Module_6";
    cmd_vector[2].flag[0] = 0;
    cmd_vector[2].flag[1] = 3;
    cmd_vector[2].flag[2] = 3;
    cmd_vector[2].flag[3] = 3;
    cmd_vector[2].gait_value[0] = 0;
    cmd_vector[2].gait_value[1] = 22.5663706144;
    cmd_vector[2].gait_value[2] = 22.5663706144;
    cmd_vector[2].gait_value[3] = 1.57079632679;
    cmd_vector[2].time_stamp = 0;
    cmd_vector[2].condition_str = "a";

    cmd_vector[3].module_name = "Module_8";
    cmd_vector[3].flag[0] = 0;
    cmd_vector[3].flag[1] = 3;
    cmd_vector[3].flag[2] = 3;
    cmd_vector[3].flag[3] = 3;
    cmd_vector[3].gait_value[0] = 0;
    cmd_vector[3].gait_value[1] = 22.5663706144;
    cmd_vector[3].gait_value[2] = 22.5663706144;
    cmd_vector[3].gait_value[3] = 1.57079632679;
    cmd_vector[3].time_stamp = 0;
    cmd_vector[3].condition_str = "a";

    cmd_vector[4].module_name = "Module_11";
    cmd_vector[4].flag[0] = 0;
    cmd_vector[4].flag[1] = 3;
    cmd_vector[4].flag[2] = 3;
    cmd_vector[4].flag[3] = 3;
    cmd_vector[4].gait_value[0] = 0;
    cmd_vector[4].gait_value[1] = 22.5663706144;
    cmd_vector[4].gait_value[2] = 22.5663706144;
    cmd_vector[4].gait_value[3] = 1.57079632679;
    cmd_vector[4].time_stamp = 0;
    cmd_vector[4].condition_str = "a";

    cmd_vector[5].module_name = "Module_14";
    cmd_vector[5].flag[0] = 0;
    cmd_vector[5].flag[1] = 3;
    cmd_vector[5].flag[2] = 3;
    cmd_vector[5].flag[3] = 3;
    cmd_vector[5].gait_value[0] = 0;
    cmd_vector[5].gait_value[1] = 22.5663706144;
    cmd_vector[5].gait_value[2] = 22.5663706144;
    cmd_vector[5].gait_value[3] = 1.57079632679;
    cmd_vector[5].time_stamp = 0;
    cmd_vector[5].condition_str = "a";

    cmd_vector[6].module_name = "Module_16";
    cmd_vector[6].flag[0] = 0;
    cmd_vector[6].flag[1] = 3;
    cmd_vector[6].flag[2] = 3;
    cmd_vector[6].flag[3] = 3;
    cmd_vector[6].gait_value[0] = 0;
    cmd_vector[6].gait_value[1] = 22.5663706144;
    cmd_vector[6].gait_value[2] = 22.5663706144;
    cmd_vector[6].gait_value[3] = 1.57079632679;
    cmd_vector[6].time_stamp = 0;
    cmd_vector[6].condition_str = "a";

    cmd_vector[7].module_name = "Module_18";
    cmd_vector[7].flag[0] = 0;
    cmd_vector[7].flag[1] = 3;
    cmd_vector[7].flag[2] = 3;
    cmd_vector[7].flag[3] = 3;
    cmd_vector[7].gait_value[0] = 0;
    cmd_vector[7].gait_value[1] = 22.5663706144;
    cmd_vector[7].gait_value[2] = 22.5663706144;
    cmd_vector[7].gait_value[3] = 1.57079632679;
    cmd_vector[7].time_stamp = 0;
    cmd_vector[7].condition_str = "a";
}
//This function returns the name of the module set in the cmd object and it is based on
// the command number which it takes as the parameter
std::string CmdWrapper::getNameByCmdNumber(int n)
{
if(n<=num_of_cmd){
    return cmd_vector[n-1].module_name;
}
else
    return NULL;
}



//This function returns a single cmd object and it is based on the command number which 
//it takes as the parameter
project_name::Cmd CmdWrapper::getCmdByNumber(int n)
{
return cmd_vector[n-1];
}
    
