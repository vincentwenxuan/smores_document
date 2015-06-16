#include "anticogging.h"
#include "anticogging_interface.h"
#include "project_message_types.h"
#include "math_helper.h"

#ifdef USING_ANTICOGGING

static int8_t demo_mode;
static int8_t anticogging_valid;
static int8_t anticogging_enabled;
float voltage_supply;
float voltage_supply_inv;

static float AnticoggingCallback(float mech_angle)
{
  if(anticogging_valid && anticogging_enabled)
  {
    mGreenON;
    return GetAnticoggingFloat(mech_angle)*voltage_supply_inv;
  }
  mGreenOFF;
  return 0.0;
}

void AnticoggingInterface::set_voltage_supply(float voltage)
{
  voltage_supply = voltage;
  voltage_supply_inv = 1.0f/voltage;
}

AnticoggingInterface::AnticoggingInterface()
{
  anticogging_enabled = 0;
  demo_mode = 0;
}

AnticoggingInterface::AnticoggingInterface(BrushlessController& controller)
{
  anticogging_valid = DataValid();
  controller.CommuteCallback = &AnticoggingCallback;
  anticogging_enabled = 0;
  demo_mode = 0;
}

void AnticoggingInterface::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length)
{
  uint8_t type = rx_data[0];
  if(type == kTypeAnticogData) 
  {
    MsgAnticogData* msg = (MsgAnticogData*)&rx_data[1];
    if(msg->cmd == GET_CMD) // get
    {
      MsgAnticogData msg_out;
      msg_out.cmd = RESPONSE_CMD;
      msg_out.index = msg->index;
      msg_out.value = GetAnticogging(msg->index);
      com.SendPacket(type, (uint8_t*)&msg_out, sizeof(MsgAnticogData));
    }
    else if(msg->cmd == SET_CMD) // set
    {
      if (SetAnticogging(msg->index,msg->value) == ERROR)
        mErrorON;
      else // check to see if the data is valid now
        anticogging_valid = DataValid();
    }
  }
  else if(type == kTypeAnticog)
  {
    MsgAnticog* msg = (MsgAnticog*)&rx_data[1];
    if(msg->cmd == 0) // erase
    {
      anticogging_valid = 0;
      EraseAnticogging();
    }
    else if(msg->cmd == 1) // enable
    {
      anticogging_enabled = 1;
      demo_mode = 0;
    }
    else if(msg->cmd == 2) // disable
    {
      anticogging_enabled = 0;
      demo_mode = 0;
    }
    else if(msg->cmd == 3) // demo mode
    {
      anticogging_enabled = 1;
      demo_mode = 1;
    }
  }
}

int8_t AnticoggingInterface::DataValid()
{
  float test_value;
  for(uint16_t i = 0; i < ENCODER_COUNTS; i++)
  {
    test_value = GetAnticogging(i);
    if(PunToUint32(&test_value) == 0xFFFFFFFF)
      return 0;
  }
  return 1;
}

void AnticoggingInterface::DemoToggle()
{
  if(demo_mode == 1)
  {
    if(anticogging_enabled == 1)
      anticogging_enabled = 0;
    else
      anticogging_enabled = 1;
  }
}

void AnticoggingInterface::DemoEnable()
{
  demo_mode = 1;
}

void AnticoggingInterface::DemoDisable()
{
  demo_mode = 0;
}

#endif // USING_ANTICOGGING