#include "brushless_controller.h"
#include "encoder.h"
#include "system_clock.h"
#include "common_message_types.h"
#include "brushless_controller_msgs.h"
#include <stdint.h>
#include "math_helper.h"
#include <math.h>

#if (defined(MOTOR_TIMER) || (defined(SYNC_TIMER_LOW) && defined(SYNC_TIMER_HIGH))) && defined(ENCODER_QEP_TIMER) 

#if defined(MOTOR_TIMER)
  #define SetBrushlessPWM SetMotorPWM
#else
  #define SetBrushlessPWM SetSyncPWM
#endif //defined(MOTOR_TIMER)

#define BRUSHLESS_CONTROLLER_BASE_KEY 0x0100
const uint16_t key_motor_encoder_calibration_angle  = BRUSHLESS_CONTROLLER_BASE_KEY + 0;
const uint16_t key_motor_electrical_lead_rate       = BRUSHLESS_CONTROLLER_BASE_KEY + 1;
const uint16_t key_motor_pole_pairs                 = BRUSHLESS_CONTROLLER_BASE_KEY + 2;
// const uint16_t key_motor_Kte              = BRUSHLESS_CONTROLLER_BASE_KEY + 3;
// const uint16_t key_motor_R                = BRUSHLESS_CONTROLLER_BASE_KEY + 4;
// const uint16_t key_motor_Vo               = BRUSHLESS_CONTROLLER_BASE_KEY + 5;
// const uint16_t key_motor_Io               = BRUSHLESS_CONTROLLER_BASE_KEY + 6;
const uint16_t key_speed_ctrl_pid_Kp      = BRUSHLESS_CONTROLLER_BASE_KEY + 11;
const uint16_t key_speed_ctrl_pid_Ki      = BRUSHLESS_CONTROLLER_BASE_KEY + 12;
const uint16_t key_speed_ctrl_pid_Kd      = BRUSHLESS_CONTROLLER_BASE_KEY + 13;
const uint16_t key_pos_ctrl_pid_Kp        = BRUSHLESS_CONTROLLER_BASE_KEY + 14;
const uint16_t key_pos_ctrl_pid_Ki        = BRUSHLESS_CONTROLLER_BASE_KEY + 15;
const uint16_t key_pos_ctrl_pid_Kd        = BRUSHLESS_CONTROLLER_BASE_KEY + 16;
const uint16_t key_permute_wires          = BRUSHLESS_CONTROLLER_BASE_KEY + 17;
const uint16_t key_motor_bemf_shape       = BRUSHLESS_CONTROLLER_BASE_KEY + 18;
const uint16_t key_motor_commutation_hz   = BRUSHLESS_CONTROLLER_BASE_KEY + 23;
const uint16_t key_speed_filter_alpha     = BRUSHLESS_CONTROLLER_BASE_KEY + 25;

void BrushlessController::InitBrushlessController(void)
{
  // Commands
  pwm_duty_cmd_ = 0.0;
  voltage_cmd_ = 0.0;
  set_commutation_hz(20000);
  speed_filter_alpha_ = 1;
  control_mode_ = kNoCtrl; // repeating initialization for clarity and emphisis 

  // Calculated Output
  pwm_duty_final_ = 0;
  
  // Observed state
  voltage_supply_ = 11.1f;
  voltage_supply_inv_ = 1.0f/11.1f;
  prev_control_time_.sec = 0;
  prev_control_time_.nsec = 0;
  
  // Calculated state
  current_motor_speed_ = 0.0; // The speed the motor is presently spinning
  motor_lead_elec_angle_ = 0.0; // radians

  // Properties
  motor_electrical_lead_rate_ = 0.0; // radians per (radians/second)
  motor_encoder_calibration_angle_ = 0.0;
  motor_pole_pairs_ = 7; // Number of motor pole pairs (# magnets/2)
  // motor_Kte_ = 0; // Torque constant of the motor
  // motor_R_ = 0;
  // motor_V0_ = 0;
  // motor_I0_ = 0;
  motor_bemf_shape_ = kTrapezoidal;
  permute_wires_ = 1;
  
  // Controls
  // current_ctrl_pid_ = Pid();
  speed_ctrl_pid_ = Pid();
  pos_ctrl_pid_ = Pid();

  CommuteCallback = DefaultCommuteCallback;
}

BrushlessController::BrushlessController():
control_mode_(kNoCtrl)
{
  InitBrushlessController();
}

#ifdef USING_PERSISTENT_MEMORY
BrushlessController::BrushlessController(PersistentMemory& memory):
control_mode_(kNoCtrl)
{
  mem_ = &memory;

  InitBrushlessController();
  
  //Add memory values
  mem_->AddKey(key_motor_encoder_calibration_angle);
  mem_->AddKey(key_motor_electrical_lead_rate);
  mem_->AddKey(key_motor_pole_pairs);
  // mem_->AddKey(key_motor_Kte);
  // mem_->AddKey(key_motor_R);
  // mem_->AddKey(key_motor_Io);
  // mem_->AddKey(key_motor_Vo);
  // mem_->AddKey(key_current_ctrl_pid_Kp);
  // mem_->AddKey(key_current_ctrl_pid_Ki);
  // mem_->AddKey(key_current_ctrl_pid_Kd);
  mem_->AddKey(key_speed_ctrl_pid_Kp);
  mem_->AddKey(key_speed_ctrl_pid_Ki);
  mem_->AddKey(key_speed_ctrl_pid_Kd);
  mem_->AddKey(key_pos_ctrl_pid_Kp);
  mem_->AddKey(key_pos_ctrl_pid_Ki);
  mem_->AddKey(key_pos_ctrl_pid_Kd);
  mem_->AddKey(key_permute_wires);
  mem_->AddKey(key_motor_bemf_shape);
  mem_->AddKey(key_motor_commutation_hz);
  mem_->AddKey(key_speed_filter_alpha);

  ReadFlash(255);
}
#endif

float DefaultCommuteCallback(float)
{
  return 0.0f;
}

void BrushlessController::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length)
{
  uint8_t type = rx_data[0];

  if(type == kTypeRequestMessage) {
    MsgRequestMessage *data_in = (MsgRequestMessage*) &rx_data[1];
    uint8_t msg_type = data_in->msg_type;

    if(msg_type == kTypeBrushlessControllerState) {
      SendState(com);
    }
  }

  else if(type==kTypeBrushlessController)
  {
    uint8_t subtype = rx_data[1];

    switch(subtype)
    {
      case kSubPwmCmd:
        set_pwm_cmd(PunToFloat(&rx_data[2]));
        break;
      case kSubVoltageCmd:
         set_voltage_cmd(PunToFloat(&rx_data[2]));
         break;
      // case kSubCurrentCmd:
      //   set_current_cmd(PunToFloat(&rx_data[2]),rx_data[6]);
      //   break;
      // case kSubTorqueCmd:
      //   set_torque_cmd(PunToFloat(&rx_data[2]));
      //   break;
      case kSubSpeedCmd:
        set_speed_cmd(PunToFloat(&rx_data[2]));
        break;
      case kSubSpeedPosCmd:
        set_speed_position_cmd(PunToFloat(&rx_data[2]),PunToFloat(&rx_data[6]));
        break;
      case kSubPosCmd:
        set_position_cmd(PunToFloat(&rx_data[2]));
        break;
      case kSubCalibrateCmd:
        CalibrateMotorAngle(PunToFloat(&rx_data[2]), PunToUint32(&rx_data[6]));
        break;
      case kSubElecAngleCmd:
      {
        float pwm   = PunToFloat(&rx_data[2]);
        float angle = PunToFloat(&rx_data[6]);
        set_elec_angle_cmd(pwm, angle);
      } break;
    #ifdef USING_PERSISTENT_MEMORY
      case kSubWriteToFlashCmd:
        WriteFlash(rx_data[2]);
        break;
    #endif
      case kSubCalibrationAngleMsg:
        if(rx_data[2] == SET_CMD) //SET_CMD
          set_motor_encoder_calibration_angle(PunToFloat(&rx_data[3]));
        else if(rx_data[2] == GET_CMD) //get
        {
          MsgBrushlessControllerCalibration msg_out;
          msg_out.cmd = RESPONSE_CMD;
          msg_out.calibration_angle = motor_encoder_calibration_angle_;
          ReplyByte(com, (uint8_t)kSubCalibrationAngleMsg, (uint8_t*)&msg_out, sizeof(MsgBrushlessControllerCalibration));
        }
        #ifdef USING_PERSISTENT_MEMORY
        else if(rx_data[2] == SAVE_CMD) //SAVE_CMD
        {
          WriteFlash(rx_data[1]);
        }
        #endif
        break;
      case kSubLeadPerVelocityMsg:
        if(rx_data[2] == SET_CMD) //SET_CMD
          set_motor_electrical_lead_rate(PunToFloat(&rx_data[3]));
        else if(rx_data[2] == GET_CMD) //get
        {
          MsgBrushlessControllerLead msg_out;
          msg_out.cmd = RESPONSE_CMD;
          msg_out.lead_rate = motor_electrical_lead_rate_;
          ReplyByte(com, (uint8_t)kSubLeadPerVelocityMsg, (uint8_t*)&msg_out, sizeof(MsgBrushlessControllerLead));
        }
        #ifdef USING_PERSISTENT_MEMORY
        else if(rx_data[2] == SAVE_CMD) //SAVE_CMD
        {
          WriteFlash(rx_data[1]);
        }
        #endif
        break;
      case kSubMotorDataMsg:
        if(rx_data[2] == SET_CMD) //SET_CMD
        {
          set_motor_data(PunToUint16(&rx_data[3]), PunToFloat(&rx_data[5]), PunToFloat(&rx_data[9]),PunToFloat(&rx_data[13]),PunToFloat(&rx_data[17]),rx_data[21]);
        }
        else if(rx_data[2] == GET_CMD) //get
        {
          MsgBrushlessControllerMotor msg_out;
          msg_out.cmd = RESPONSE_CMD;
          msg_out.pole_pairs = motor_pole_pairs_;
          msg_out.Kv = NAN; // not used
          msg_out.R  = NAN; // not used
          msg_out.I0 = NAN; // not used
          msg_out.V0 = NAN; // not used
          msg_out.bemf_shape = motor_bemf_shape_;
          ReplyByte(com, (uint8_t)kSubMotorDataMsg, (uint8_t*)&msg_out, sizeof(MsgBrushlessControllerMotor));
        }
        #ifdef USING_PERSISTENT_MEMORY
        else if(rx_data[2] == SAVE_CMD) //SAVE_CMD
        {
          WriteFlash(rx_data[1]);
        }
        #endif
        break;
      // case kSubCurrentCtrlMsg:
      //   if(rx_data[2] == SET_CMD) //SET_CMD
      //     set_current_ctrl(PunToFloat(&rx_data[3]),PunToFloat(&rx_data[7]),PunToFloat(&rx_data[11]));
      //   else if(rx_data[2] == GET_CMD) //get
      //   {
      //     MsgBrushlessControllerCurrentControl msg_out;
      //     msg_out.cmd = RESPONSE_CMD;
      //     msg_out.Kp = current_ctrl_pid_.get_Kp();
      //     msg_out.Ki = current_ctrl_pid_.get_Ki();
      //     msg_out.Kd = current_ctrl_pid_.get_Kd();
      //     ReplyByte(com, (uint8_t)kSubCurrentCtrlMsg, (uint8_t*)&msg_out, sizeof(MsgBrushlessControllerCurrentControl));
      //   }
      //   else if(rx_data[2] == SAVE_CMD) //SAVE_CMD
      //   {
      //     WriteFlash(rx_data[1]);
      //   }
      //   break;
      case kSubSpeedCtrlMsg:
        if(rx_data[2] == SET_CMD) //SET_CMD
          set_speed_ctrl(PunToFloat(&rx_data[3]),PunToFloat(&rx_data[7]),PunToFloat(&rx_data[11]));
        else if(rx_data[2] == GET_CMD) //get
        {
          MsgBrushlessControllerSpeedControl msg_out;
          msg_out.cmd = RESPONSE_CMD;
          msg_out.Kp = speed_ctrl_pid_.get_Kp();
          msg_out.Ki = speed_ctrl_pid_.get_Ki();
          msg_out.Kd = speed_ctrl_pid_.get_Kd();
          ReplyByte(com, (uint8_t)kSubSpeedCtrlMsg, (uint8_t*)&msg_out, sizeof(MsgBrushlessControllerSpeedControl));
        }
        #ifdef USING_PERSISTENT_MEMORY
        else if(rx_data[2] == SAVE_CMD) //SAVE_CMD
        {
          WriteFlash(rx_data[1]);
        }
        #endif
        break;
      case kSubPosCtrlMsg:
        if(rx_data[2] == SET_CMD) //SET_CMD
          set_pos_ctrl(PunToFloat(&rx_data[3]),PunToFloat(&rx_data[7]),PunToFloat(&rx_data[11]));
        else if(rx_data[2] == GET_CMD) //get
        {
          MsgBrushlessControllerPositionControl msg_out;
          msg_out.cmd = RESPONSE_CMD;
          msg_out.Kp = pos_ctrl_pid_.get_Kp();
          msg_out.Ki = pos_ctrl_pid_.get_Ki();
          msg_out.Kd = pos_ctrl_pid_.get_Kd();
          ReplyByte(com, (uint8_t)kSubPosCtrlMsg, (uint8_t*)&msg_out, sizeof(MsgBrushlessControllerPositionControl));
        }
        #ifdef USING_PERSISTENT_MEMORY
        else if(rx_data[2] == SAVE_CMD) //SAVE_CMD
        {
          WriteFlash(rx_data[1]);
        }
        #endif
        break;
      case kSubPermuteMsg:
        if(rx_data[2] == SET_CMD) //SET_CMD
          set_permute_wires(rx_data[3]);
        else if(rx_data[2] == GET_CMD) //get
        {
          MsgBrushlessControllerPermute msg_out;
          msg_out.cmd = RESPONSE_CMD;
          msg_out.permute = permute_wires_;
          ReplyByte(com, (uint8_t)kSubPermuteMsg, (uint8_t*)&msg_out, sizeof(MsgBrushlessControllerPermute));
        }
        #ifdef USING_PERSISTENT_MEMORY
        else if(rx_data[2] == SAVE_CMD) //SAVE_CMD
        {
          WriteFlash(rx_data[1]);
        }
        #endif
        break;
      case kSubStateMsg:
        // read only
        if(rx_data[2] == GET_CMD) //get
        {
          SendState(com);
        }
        break;

      case kSubSpeedKpMsg:
        {
        uint8_t direction = rx_data[2];
        float ftemp;
        if(direction == GET_CMD) {
          uint8_t tx_msg[6];
          tx_msg[0] = subtype;
          tx_msg[1] = 3;
          ftemp = speed_ctrl_pid_.get_Kp();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBrushlessController, tx_msg, 6);
        }
        else if(direction == SET_CMD) {
          ftemp = PunToFloat(&rx_data[3]);
          speed_ctrl_pid_.set_Kp(ftemp);
        }
        #ifdef USING_PERSISTENT_MEMORY
        else if(direction == SAVE_CMD) {
          ftemp = speed_ctrl_pid_.get_Kp();
          mem_->Write(key_speed_ctrl_pid_Kp, PunToUint32(ftemp));
        }
        #endif
        }
        break;

      case kSubSpeedKiMsg:
        {
        uint8_t direction = rx_data[2];
        float ftemp;
        if(direction == GET_CMD) {
          uint8_t tx_msg[6];
          tx_msg[0] = subtype;
          tx_msg[1] = 3;
          ftemp = speed_ctrl_pid_.get_Ki();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBrushlessController, tx_msg, 6);
        }
        else if(direction == SET_CMD) {
          ftemp = PunToFloat(&rx_data[3]);
          speed_ctrl_pid_.set_Ki(ftemp);
        }
        #ifdef USING_PERSISTENT_MEMORY
        else if(direction == SAVE_CMD) {
          ftemp = speed_ctrl_pid_.get_Ki();
          mem_->Write(key_speed_ctrl_pid_Ki, PunToUint32(ftemp));
        }
        #endif
        } 
        break;

      case kSubSpeedKdMsg:
        {
        uint8_t direction = rx_data[2];
        float ftemp;
        if(direction == GET_CMD) {
          uint8_t tx_msg[6];
          tx_msg[0] = subtype;
          tx_msg[1] = 3;
          ftemp = speed_ctrl_pid_.get_Kd();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBrushlessController, tx_msg, 6);
        }
        else if(direction == SET_CMD) {
          ftemp = PunToFloat(&rx_data[3]);
          speed_ctrl_pid_.set_Kd(ftemp);
        }
        #ifdef USING_PERSISTENT_MEMORY
        else if(direction == SAVE_CMD) {
          ftemp = speed_ctrl_pid_.get_Kd();
          mem_->Write(key_speed_ctrl_pid_Kd, PunToUint32(ftemp));
        }
        #endif
        }
        break;
      
      case kSubCommutationHzMsg:
        if(rx_data[2] == SET_CMD) //SET_CMD
          set_commutation_hz(PunToUint32(&rx_data[3]));
        else if(rx_data[2] == GET_CMD) //get
        {
          MsgBrushlessControllerCommutationHz msg_out;
          msg_out.cmd = RESPONSE_CMD;
          msg_out.commutation_hz = get_commutation_hz();
          ReplyByte(com, (uint8_t)kSubCommutationHzMsg, (uint8_t*)&msg_out, sizeof(MsgBrushlessControllerCommutationHz));
        }
        #ifdef USING_PERSISTENT_MEMORY
        else if(rx_data[2] == SAVE_CMD) //SAVE_CMD
        {
          WriteFlash(rx_data[1]);
        }
        #endif
        break;
      case kSubCmdVoltsMsg:
        {
        uint8_t direction = rx_data[2];
        float ftemp;
        if(direction == GET_CMD) {
          uint8_t tx_msg[6];
          tx_msg[0] = subtype;
          tx_msg[1] = 3;
          ftemp = get_voltage_cmd();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBrushlessController, tx_msg, 6);
        }
        else if(direction == SET_CMD) {
          ftemp = PunToFloat(&rx_data[3]);
          set_voltage_cmd(ftemp);
        }
        }
        break;

      case kSubSpeedFilterAlphaMsg:
        {
        uint8_t direction = rx_data[2];
        if(direction == GET_CMD) {
          uint8_t tx_msg[6];
          tx_msg[0] = subtype;
          tx_msg[1] = 3;
          memcpy(&tx_msg[2], &speed_filter_alpha_, 4);
          com.SendPacket(kTypeBrushlessController, tx_msg, 6);
        }
        else if(direction == SET_CMD) {
          speed_filter_alpha_ = PunToFloat(&rx_data[3]);
        }
        #ifdef USING_PERSISTENT_MEMORY
        else if(direction == SAVE_CMD) {
          mem_->Write(key_speed_filter_alpha, PunToUint32(speed_filter_alpha_));
        }
        #endif
        }
        break;
    }
  }  
}

void BrushlessController::BrushlessControllerOffCommute()
{
  SetBrushlessPWM(max_pwm_, 0, 0, 0); // Pulls all lines low
}

void BrushlessController::BrushlessControllerEncoderCommute(float pwm_duty_cmd)
{
  float mech_angle = encoderReadRad();
  
  BrushlessControllerElectricalAngleCommute(pwm_duty_cmd + CommuteCallback(mech_angle), MechToElecAngle(mech_angle + motor_encoder_calibration_angle_, motor_pole_pairs_) + motor_lead_elec_angle_);
}

void BrushlessController::BrushlessControllerElectricalAngleCommute(float pwm_duty_final, float electrical_angle_cmd)
{
  float current_pwms[3];
  pwm_duty_final_ = SatFloat(pwm_duty_final, -1.0f, 1.0f);
  CalcSpaceVector(electrical_angle_cmd, pwm_duty_final_, current_pwms, motor_bemf_shape_);

  if(permute_wires_) {
    SetBrushlessPWM(max_pwm_, (uint16_t)(current_pwms[1]*max_pwm_float_), (uint16_t)(current_pwms[0]*max_pwm_float_), (uint16_t)(current_pwms[2]*max_pwm_float_));
  }
  else {
    SetBrushlessPWM(max_pwm_, (uint16_t)(current_pwms[0]*max_pwm_float_), (uint16_t)(current_pwms[1]*max_pwm_float_), (uint16_t)(current_pwms[2]*max_pwm_float_)); 
  }
}

void BrushlessController::BrushlessControllerCommute()
{
  // Function pointers determine what to do
  if ((int8_t)control_mode_ > 0) // These use the encoder
    BrushlessControllerEncoderCommute(pwm_duty_cmd_);
  else if(control_mode_ == kOffCtrl)
    BrushlessControllerOffCommute();
  //else if (control_mode_ == kElecAngCtrl (-2) or control_mode_ == kCalibrationCtrl (-1) )
    // don't do anything
  //else
    // don't do anything
}

void BrushlessController::BrushlessControllerControl()
{
  // get period and frequency of control updates
  struct Time control_time = GetTime();
  float dt = ToSeconds(SubtractTime(control_time, prev_control_time_)); // seconds
  float freq = 1.0f / dt; // frequency, Hz
  prev_control_time_ = control_time;

  // update speed estimate
  current_motor_speed_ = compute_speed_fr(freq, speed_filter_alpha_);
  motor_lead_elec_angle_ = motor_electrical_lead_rate_ * current_motor_speed_ * motor_pole_pairs_;

  // calculate pwm duty based on active controller
  switch(control_mode_)
  {
     case kVoltageCtrl:
       pwm_duty_cmd_ = voltage_cmd_*voltage_supply_inv_;
       break;
    // case kCurrentCtrl:
    //   //TODO:: CURRENT CONTROL!!!!!!
    //   break;
    // case kTorqueCtrl:
    //   //TODO:: TORQUE CONTROL!!!!!!!!!!
    //   break;
    case kSpeedCtrl:
      //TODO:: FEEDFORWARD
      pwm_duty_cmd_ = speed_ctrl_pid_.PidCompute(current_motor_speed_);
      break;
    case kSpeedPosCtrl: // Fall into position control (position setpoint dot is set elsewhere)
    case kPosCtrl:
      pwm_duty_cmd_ = pos_ctrl_pid_.PidComputeRadMod(encoderReadRad(),current_motor_speed_);
      break;
    case kCalibrationCtrl:
      if (GreaterThan(GetTime(),calibration_end_time_)) // Check for timeout
      {
        motor_encoder_calibration_angle_ = -encoderReadRad(); // Grab calibration angle
        control_mode_ = kOffCtrl;  // Command to known safe values
      }
      break;
    default:
      // if control_mode_ = kPwmCtrl, do nothing
      // if control_mode_ = kElecAngleCtrl, do nothing
      break;
  }
}

void BrushlessController::CalibrateMotorAngle(float calibration_pwm_duty, uint32_t calibration_time_ms)
{
  // Make sure calibration time is int32 able since calibration time comes in as uint32
  if (calibration_time_ms < 0x80000000)
  {
    calibration_end_time_ = AddTime(GetTime(),FromMilliseconds(calibration_time_ms)); // Set the end time   
    control_mode_ = kCalibrationCtrl; // Changing to calibration control mode before setting is safe because calibration control mode does nothing in commute. Worst case, last commute is left for an extra control cycle.
    BrushlessControllerElectricalAngleCommute(calibration_pwm_duty,4.71238898f);
  }
}

float BrushlessController::get_mechanical_angle() {
  return encoderReadRad();
}

void BrushlessController::set_pwm_cmd(float pwm_in)
{
  pwm_duty_cmd_ = pwm_in;
  control_mode_ = kPwmCtrl;
}
    
void BrushlessController::set_voltage_cmd(float voltage)
{
  voltage_cmd_ = voltage;
  control_mode_ = kVoltageCtrl;
}

//void BrushlessController::set_current_cmd(float current, uint8_t regen){/*TODO*/}
    
//void BrushlessController::set_torque_cmd(float torque){/*TODO*/}

void BrushlessController::set_speed_cmd(float speed)
{
  speed_ctrl_pid_.set_set_point(speed);
  control_mode_ = kSpeedCtrl;
}

void BrushlessController::set_speed_position_cmd(float speed, float position)
{
    pos_ctrl_pid_.set_set_point_dot(speed);
    pos_ctrl_pid_.set_set_point(WrapFloat(position, 2.0f*PI,0.0f));
    control_mode_ = kSpeedPosCtrl;
}
    
void BrushlessController::set_position_cmd(float position)
{
  pos_ctrl_pid_.set_set_point(WrapFloat(position, 2.0f*PI,0.0f));
  control_mode_ = kPosCtrl;
}

void BrushlessController::set_elec_angle_cmd(float pwm, float angle)
{
  BrushlessControllerElectricalAngleCommute(pwm, angle);
  control_mode_ = kElecAngleCtrl;
}

void BrushlessController::set_voltage_supply(float voltage)
{
  voltage_supply_ = voltage;
  voltage_supply_inv_ = 1.0f/voltage;
}

void BrushlessController::set_motor_encoder_calibration_angle(float angle)
{
  if (angle < 2.0f*PI && angle > -2.0f*PI)
    motor_encoder_calibration_angle_ = angle;
}

void BrushlessController::set_commutation_hz(uint32_t commutation_hz)
{
  if (commutation_hz < SystemCoreClock)
  {
    max_pwm_ = SystemCoreClock/commutation_hz/2;
    max_pwm_float_ = (float)max_pwm_;
  }
}

uint32_t BrushlessController::get_commutation_hz()
{
  return SystemCoreClock/max_pwm_/2;
}

void BrushlessController::set_motor_data(uint16_t motor_pole_pairs_in, float motor_Kv_in, float motor_R_in, float motor_I0_in, float motor_V0_in, uint8_t motor_bemf_shape_in)
{
  motor_pole_pairs_ = motor_pole_pairs_in;
  // if (motor_Kv_in > 0.0)
  //   motor_Kte_ = 1/(motor_Kv_in*(2.0*PI))*60.0;
  // if (motor_R_in > 0.0)
  //   motor_R_ = motor_R_in;
  // if (motor_I0_in > 0.0)
  //   motor_I0_ = motor_I0_in;
  // if (motor_V0_in > 0.0)
  //   motor_V0_ = motor_V0_in;
  motor_bemf_shape_ = (Shape)motor_bemf_shape_in;
}

void BrushlessController::set_permute_wires(uint8_t permute)
{
  permute_wires_ = permute;
}

#ifdef USING_PERSISTENT_MEMORY
void BrushlessController::WriteFlash(uint8_t variable_number)
{
  float temp_float;
  if(variable_number == 255 || variable_number == kSubCalibrationAngleMsg) // write calibration
  {
    mem_->Write(key_motor_encoder_calibration_angle, PunToUint32(&motor_encoder_calibration_angle_));
  }
  if(variable_number == 255 || variable_number == kSubLeadPerVelocityMsg) // write lead
  {
    mem_->Write(key_motor_electrical_lead_rate, PunToUint32(&motor_electrical_lead_rate_));
  }
  if(variable_number == 255 || variable_number == kSubMotorDataMsg) // write motor settings
  {
    mem_->Write(key_motor_pole_pairs, motor_pole_pairs_);
    // mem_->Write(key_motor_Kte, PunToUint32(&motor_Kte_));
    // mem_->Write(key_motor_R, PunToUint32(&motor_R_));
    // mem_->Write(key_motor_Vo, PunToUint32(&motor_V0_));
    // mem_->Write(key_motor_Io, PunToUint32(&motor_I0_));
    mem_->Write(key_motor_bemf_shape, motor_bemf_shape_);
  }

  // if(variable_number == 255 || variable_number == kSubCurrentCtrlMsg) // write current control params
  // {
  //   temp_float = current_ctrl_pid_.get_Kp();
  //   mem_->Write(key_current_ctrl_pid_Kp, PunToUint32(&(temp_float)));
  //   temp_float = current_ctrl_pid_.get_Ki();
  //   mem_->Write(key_current_ctrl_pid_Ki, PunToUint32(&(temp_float)));
  //   temp_float = current_ctrl_pid_.get_Kd();
  //   mem_->Write(key_current_ctrl_pid_Kd, PunToUint32(&(temp_float)));
  // }
  if(variable_number == 255 || variable_number == kSubSpeedCtrlMsg) // write speed control params
  {
    temp_float = speed_ctrl_pid_.get_Kp();
    mem_->Write(key_speed_ctrl_pid_Kp, PunToUint32(&(temp_float)));
    temp_float = speed_ctrl_pid_.get_Ki();
    mem_->Write(key_speed_ctrl_pid_Ki, PunToUint32(&(temp_float)));
    temp_float = speed_ctrl_pid_.get_Kd();
    mem_->Write(key_speed_ctrl_pid_Kd, PunToUint32(&(temp_float)));
  }
  if(variable_number == 255 || variable_number == kSubPosCtrlMsg) // write pos control params
  {
    temp_float = pos_ctrl_pid_.get_Kp();
    mem_->Write(key_pos_ctrl_pid_Kp, PunToUint32(&(temp_float)));
    temp_float = pos_ctrl_pid_.get_Ki();
    mem_->Write(key_pos_ctrl_pid_Ki, PunToUint32(&(temp_float)));
    temp_float = pos_ctrl_pid_.get_Kd();
    mem_->Write(key_pos_ctrl_pid_Kd, PunToUint32(&(temp_float)));
  }
  if(variable_number == 255 || variable_number == kSubPermuteMsg)
  {
    mem_->Write(key_permute_wires, (uint32_t)permute_wires_);
  }
  if(variable_number == 255 || variable_number == kSubCommutationHzMsg)
  {
    mem_->Write(key_motor_commutation_hz, get_commutation_hz());
  }
}
#endif

#ifdef USING_PERSISTENT_MEMORY
void BrushlessController::WriteAllFlash(){
  WriteFlash(255);
}
#endif

#ifdef USING_PERSISTENT_MEMORY
void BrushlessController::ReadFlash(uint8_t variable_number)
{
  uint32_t uint32_value;
  if(variable_number == 255 || variable_number == kSubCalibrationAngleMsg) // read calibration
  {
    if(mem_->Read(key_motor_encoder_calibration_angle, (uint32_t*)&uint32_value) == 0)
      motor_encoder_calibration_angle_ = PunToFloat(&uint32_value);
  }
  if(variable_number == 255 || variable_number == kSubLeadPerVelocityMsg) // read lead
  {
    if(mem_->Read(key_motor_electrical_lead_rate, (uint32_t*)&uint32_value) == 0)
      motor_electrical_lead_rate_ = PunToFloat(&uint32_value);
  }
  if(variable_number == 255 || variable_number == kSubMotorDataMsg) // read motor settings
  {
    if(mem_->Read(key_motor_pole_pairs, (uint32_t*)&uint32_value) == 0)
      motor_pole_pairs_ = uint32_value;
    // if(mem_->Read(key_motor_Kte, (uint32_t*)&uint32_value) == 0)
    //   motor_Kte_ = PunToFloat(&uint32_value);
    // if(mem_->Read(key_motor_R, (uint32_t*)&uint32_value) == 0)
    //   motor_R_ = PunToFloat(&uint32_value);
    // if(mem_->Read(key_motor_Vo, (uint32_t*)&uint32_value) == 0)
    //   motor_V0_ = PunToFloat(&uint32_value);
    // if(mem_->Read(key_motor_Io, (uint32_t*)&uint32_value) == 0)
    //   motor_I0_ = PunToFloat(&uint32_value);
    if(mem_->Read(key_motor_bemf_shape, (uint32_t*)&uint32_value) == 0)
      motor_bemf_shape_ = (Shape)uint32_value;
  }

  // if(variable_number == 255 || variable_number == kSubCurrentCtrlMsg) // read current control params
  // {
  //   if(mem_->Read(key_current_ctrl_pid_Kp, (uint32_t*)&uint32_value) == 0)
  //     current_ctrl_pid_.set_Kp(PunToFloat(&uint32_value));
  //   if(mem_->Read(key_current_ctrl_pid_Ki, (uint32_t*)&uint32_value) == 0)
  //     current_ctrl_pid_.set_Ki(PunToFloat(&uint32_value));
  //   if(mem_->Read(key_current_ctrl_pid_Kd, (uint32_t*)&uint32_value) == 0)
  //     current_ctrl_pid_.set_Kd(PunToFloat(&uint32_value));
  // }
  if(variable_number == 255 || variable_number == kSubSpeedCtrlMsg) // read speed control params
  {
    if(mem_->Read(key_speed_ctrl_pid_Kp, (uint32_t*)&uint32_value) == 0)
      speed_ctrl_pid_.set_Kp(PunToFloat(&uint32_value));
    if(mem_->Read(key_speed_ctrl_pid_Ki, (uint32_t*)&uint32_value) == 0)
      speed_ctrl_pid_.set_Ki(PunToFloat(&uint32_value));
    if(mem_->Read(key_speed_ctrl_pid_Kd, (uint32_t*)&uint32_value) == 0)
      speed_ctrl_pid_.set_Kd(PunToFloat(&uint32_value));
  }
  if(variable_number == 255 || variable_number == kSubPosCtrlMsg) // read pos control params
  {
    if(mem_->Read(key_pos_ctrl_pid_Kp, (uint32_t*)&uint32_value) == 0) //If successfully loaded param
      pos_ctrl_pid_.set_Kp(PunToFloat(&uint32_value));
    if(mem_->Read(key_pos_ctrl_pid_Ki, (uint32_t*)&uint32_value) == 0) //If successfully loaded param
      pos_ctrl_pid_.set_Ki(PunToFloat(&uint32_value));
    if(mem_->Read(key_pos_ctrl_pid_Kd, (uint32_t*)&uint32_value) == 0) //If successfully loaded param
      pos_ctrl_pid_.set_Kd(PunToFloat(&uint32_value));
  }
  if(variable_number == 255 || variable_number == kSubPermuteMsg) // read permute_wires_ param
  {
    if(mem_->Read(key_permute_wires, (uint32_t*)&uint32_value) == 0) //If successfully loaded param
      permute_wires_ = (uint8_t)uint32_value;
  }
  if(variable_number == 255 || variable_number == kSubCalibrationAngleMsg) // read calibration
  {
    if(mem_->Read(key_motor_encoder_calibration_angle, (uint32_t*)&uint32_value) == 0)
      motor_encoder_calibration_angle_ = PunToFloat(&uint32_value);
  }
  if(variable_number == 255 || variable_number == kSubCommutationHzMsg) // read commutation hz
  {
    if(mem_->Read(key_motor_commutation_hz, (uint32_t*)&uint32_value) == 0)
      set_commutation_hz(uint32_value);
  }
  if(variable_number == 255 || variable_number == kSubSpeedFilterAlphaMsg)
  {
    if(mem_->Read(key_speed_filter_alpha, (uint32_t*)&uint32_value) == 0)
      speed_filter_alpha_ = PunToFloat(&uint32_value);
  }
}
#endif

#ifdef USING_PERSISTENT_MEMORY
void BrushlessController::ReadAllFlash(){
  ReadFlash(255);
}
#endif

void BrushlessController::set_motor_electrical_lead_rate(float lead_rate)
{
    motor_electrical_lead_rate_ = lead_rate;
}

// void BrushlessController::set_current_ctrl(float kp, float ki, float kd)
// {
//   if(kp >= 0.0 && ki >= 0.0 && kd >= 0.0)
//   {
//     current_ctrl_pid_.set_Kp(kp);
//     current_ctrl_pid_.set_Ki(ki);
//     current_ctrl_pid_.set_Kd(kd);
//   }
// }

void BrushlessController::set_speed_ctrl(float kp, float ki, float kd)
{
  if(kp >= 0.0 && ki >= 0.0 && kd >= 0.0)
  {
    speed_ctrl_pid_.set_Kp(kp);
    speed_ctrl_pid_.set_Ki(ki);
    speed_ctrl_pid_.set_Kd(kd);
  }
}

void BrushlessController::set_pos_ctrl(float kp, float ki, float kd)
{
  if(kp >= 0.0 && ki >= 0.0 && kd >= 0.0)
  {
    pos_ctrl_pid_.set_Kp(kp);
    pos_ctrl_pid_.set_Ki(ki);
    pos_ctrl_pid_.set_Kd(kd);
  }
}

float BrushlessController::get_pwm_cmd() {return pwm_duty_cmd_;}

float BrushlessController::get_pwm()      {return pwm_duty_final_;}

float BrushlessController::get_voltage_cmd() {return voltage_cmd_;}

float BrushlessController::get_speed()    { return current_motor_speed_;}

float BrushlessController::get_voltage_supply_inverse() { return voltage_supply_inv_;}

void BrushlessController::SendState(CommunicationInterface& com) {
  MsgBrushlessControllerState tx_msg;
  tx_msg.cmd = (uint8_t)RESPONSE_CMD;
  tx_msg.t = GetSeconds();
  tx_msg.cmd_pwm = get_pwm_cmd();
  tx_msg.pwm = get_pwm();
  tx_msg.speed = get_speed();
  tx_msg.position = get_mechanical_angle();
  ReplyByte(com, (uint8_t)kSubStateMsg, (uint8_t*)&tx_msg, sizeof(MsgBrushlessControllerState));
}

void BrushlessController::ReplyByte(CommunicationInterface& com, uint8_t subtype, uint8_t* data_in, uint8_t length) 
{
  uint8_t type = kTypeBrushlessController;
  uint8_t data_out[255];
  data_out[0] = subtype;
  memcpy(&data_out[1],data_in,length);
  com.SendPacket(type, data_out, length + 1);
}

#endif // #if (defined(MOTOR_TIMER) || (defined(SYNC_TIMER_LOW) && defined(SYNC_TIMER_HIGH))) && defined(ENCODER_QEP_TIMER) 