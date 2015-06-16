#ifndef ANTICOGGING_INTERFACE_H
#define ANTICOGGING_INTERFACE_H

#include "communication_interface.h"
#include "brushless_controller.h"

#ifdef USING_ANTICOGGING

class AnticoggingInterface {

  private:
  
  public:
    /// Default Constructor
    AnticoggingInterface();
    AnticoggingInterface(BrushlessController& controller);
    
    /// Checks anticogging data to make sure all values have been written.
    /// Returns 1 for valid, 0 for invalid
    int8_t DataValid();
    
    void set_voltage_supply(float voltage);
    
    void DemoToggle();
    void DemoEnable();
    void DemoDisable();
    
    /*******************************************************************************
     * Parsing
     ******************************************************************************/
     
    /// Read a given message and act appropriately.
    void ReadMsg(CommunicationInterface& com, uint8_t* data, uint8_t length);

}; // class AnticoggingInterface

#endif // USING_ANTICOGGING
#endif // ANTICOGGING_INTERFACE_H