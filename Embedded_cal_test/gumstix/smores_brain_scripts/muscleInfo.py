#This class contains fields that are used to store information for each muscle module in the system.
class MuscleInfo:
    position={'left':0.0,'right':0.0,'front':0.0,'cbj':0.0}
    speed={'left':0.0,'right':0.0,'front':0.0,'cbj':0.0}
    torque={'left':0.0,'right':0.0,'front':0.0,'cbj':0.0}
    dock_id={'left':0,'right':0,'front':0,'back':0}
    magnet_state={'left':0,'right':0,'front':0,'back':0}
    ip=""

    def __init__ (self, module_ip):
        self.ip=module_ip
  
    def print_info(self):    
	print 'My ID is '+self.ip
    
    def change_ip(self,new_ip):
	self.ip=new_ip
