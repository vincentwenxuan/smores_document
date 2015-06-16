/** 
** This template will guide you in making a ST M4 project.  
** All locations with double comments require your attention (** or ///)
** Please perform the following tasks:
**
** Edit peripherals.h to enable the desired peripherals
** Edit peripherals.c to enable the desired peripherals
** Include custom headers
** Write your code in the specified areas
** Comment your code
** Delete this comment and put a description of your project and the used hardware
**/

#include "stm32f37x.h"
#include "mGeneral.h"
#include "common_peripherals.h"

/// Include your custom headers here
#include "vector_control.h"
#include "project_message_types.h"
#include "usb_interface.h"
#include "zigbee_interface.h"
#include "invensense_imu.h"

/// Initialize functions here
void calibrate_motor_angle(float calibration_pwm_duty, uint32_t calibrate_time_ms);

/// Initialize variables here
typedef enum {NO_DATA=0, ZIGBEE_DATA, USB_DATA} datasource_t;
datasource_t datasource = NO_DATA;
float pwm_duty = 0.0;
uint16_t max_pwm = 1000;
float current_motor_speed = 0.0;
float motor_speed_smoothing_factor = 0.5;
float motor_lead_elec_factor = .0003*7; // radians per (radians/second)
float motor_lead_elec_angle = 0.0; // radians
float motor_encoder_calibration_angle = 0.0;
float yaw_rate = 0.0;
float yaw_angle = 0.0;
float yaw_angle_setpoint = 0.0;
float beacon = 0.0;
float pulse_phase = 0.0;
float pulse_amplitude = 0.0;
uint32_t period_previous = 0xFFFFFFFF;
uint32_t period_current = 0;
uint16_t motor_pole_pairs = 7;
uint32_t com_ms_timeout = 250;
uint32_t com_ms_since_last = 0;
uint16_t eeprom_addresses[] = {EEPROM_MAX_PWM, EEPROM_MOTOR_SPEED_SMOOTHING_FACTOR, EEPROM_MOTOR_LEAD_ELEC_FACTOR, \
              EEPROM_MOTOR_ENCODER_CALIBRATION_ANGLE, EEPROM_POLE_PAIRS, \
              kEepromAccelOffsetX,  kEepromAccelOffsetY,  kEepromAccelOffsetZ, \
              kEepromAccelScaleX,   kEepromAccelScaleY,   kEepromAccelScaleZ, \
              kEepromGyroOffsetX,   kEepromGyroOffsetY,   kEepromGyroOffsetZ, \
              kEepromGyroScaleX,    kEepromGyroScaleY,    kEepromGyroScaleZ};
              
int16_t imu_raw[6];
float imu_metric[6];
float imu_time;

const uint8_t kImuCommandDefaultOffset = 0;
const uint8_t kImuCommandDefaultScale = 1;
const uint8_t kImuCommandCalibrateGyroOffset = 2;

uint8_t module_ID = 3;

// main function
int main(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h
  
  int16_t imu_offset[6];
  float imu_scale[6];
  ConfigureImu();
  InitializeImu(imu_offset, imu_scale);
  
  /* INDEX GPIO clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* INDEX GPIO config */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  mRedON;
  mWhiteON;
  mGreenON;
  mAmberON;
//  mCyanON;
  mOrangeON;
//  mBlueON;
  
  _delay_ms(500);
  _delay_ms(500);
  
  mRedOFF;
  mWhiteOFF;
  mGreenOFF;
  mAmberOFF;
//  mCyanOFF;
  mOrangeOFF;
//  mBlueOFF;

  /// Initialize code here
  InitUsbInterface();
  
  // initialize Zigbee interface object
  struct ZigbeeInterface zigbee_interface;
  InitZigbeeInterface(&zigbee_interface);
  
  // load eeprom values
  EE_ReadVariable(EEPROM_MOTOR_ENCODER_CALIBRATION_ANGLE, (uint32_t *)&motor_encoder_calibration_angle);
  EE_ReadVariable(EEPROM_MOTOR_LEAD_ELEC_FACTOR, (uint32_t *)&motor_lead_elec_factor);
  
  /*printf("I think cal_ang is: %f\n\r", motor_encoder_calibration_angle);
  
  for(int i = 0; i < 6*10; i = i+1)
  {
    printf("%u, %u\r",EEPROM_START_ADDRESS+i, *(__IO uint8_t *)(EEPROM_START_ADDRESS+i));
    _delay_ms(5);
  }*/
  ReadImuStart();
  
  // main loop
  while(1)
  {
    // uint8_t state = ZigBee_ReadState();
    // printf("%d\r\n", state);
    /// COMPUTE SPEED
    // TODO:: move from timerCallback2
    
    /// CHECK FOR DATA (Zigbee and USB)
    GetZigbeeBytes(&zigbee_interface);
    GetUsbBytes(); // poll for new USB bytes
    // if packets are available
    uint8_t *received_data;  // temporary pointer to received type+data bytes
    uint8_t received_length; // number of of received type+data bytes
    if(PeekZigbeePacket(&zigbee_interface, &received_data, &received_length))
    {
      datasource = ZIGBEE_DATA;
    }
    else if(PeekUsbPacket(&received_data, &received_length)) 
    {
      datasource = USB_DATA;
    }
    else 
      datasource = NO_DATA;
    
    //TODO:: this only does one packet per loop, should it be more or would that kill the main loop with big message???
    if(datasource != NO_DATA) // if there's data from somewhere
    {
      mWhiteON;
      com_ms_since_last = 0;
      // copy and store packet type
      uint8_t type = received_data[0];
      MsgPwm *pwm_in;
      MsgCal *cal_in;
      MsgLead *lead_in;
      MsgCalAng *cal_ang_in;
      MsgTAPY *tapy_in;
      MsgRotFbk rotations_out;
      if(type == PWM_CMD)
      {
        // copy and store packet data, using a helper struct as a template
        pwm_in = (MsgPwm*) &received_data[1];
        pwm_duty = pwm_in->pwm_duty;
        printf("pwm cmd: %f\n\r",pwm_duty);
      }
      else if(type == LEAD_FACT)
      {
        // copy and store packet data, using a helper struct as a template
        lead_in = (MsgLead*) &received_data[1];
        motor_lead_elec_factor = lead_in->lead_fact;
        printf("lead factor: %f\n\r",motor_lead_elec_factor);
        EE_WriteVariable(EEPROM_MOTOR_LEAD_ELEC_FACTOR, *(uint32_t *)&motor_lead_elec_factor);
      }
      else if(type == CAL_ANG)
      {
        // copy and store packet data, using a helper struct as a template
        cal_ang_in = (MsgCalAng*) &received_data[1];
        motor_encoder_calibration_angle = cal_ang_in->cal_ang;
        printf("cal ang: %f\n\r",motor_encoder_calibration_angle);
        EE_WriteVariable(EEPROM_MOTOR_ENCODER_CALIBRATION_ANGLE, *(uint32_t *)&motor_encoder_calibration_angle);
      }
      else if(type == CAL_CMD)
      {
        // copy and store packet data, using a helper struct as a template
        cal_in = (MsgCal*) &received_data[1];
        float cal_duty = cal_in->cal_duty;
        uint16_t cal_time_ms = cal_in->cal_time_ms;
        printf("Cal pwm %f, time: %u\n\r",cal_duty,cal_time_ms);
        calibrate_motor_angle(cal_duty,cal_time_ms);
        printf("cal ang: %f\n\r",motor_encoder_calibration_angle);
        EE_WriteVariable(EEPROM_MOTOR_ENCODER_CALIBRATION_ANGLE, *(uint32_t *)&motor_encoder_calibration_angle);
      }
      else if(type == TAPY_CMD)
      {
        tapy_in = (MsgTAPY*) &received_data[1];
        if(tapy_in->id == module_ID)
        {
          pwm_duty = tapy_in->thrust;
          pulse_amplitude = tapy_in->amplitude;
          pulse_phase = tapy_in->phase;
          yaw_angle_setpoint += tapy_in->yaw;
        }
      }
      else if(type == kTypeImuCommand)
      {
        MsgImuCommand *data_in = (MsgImuCommand*) &received_data[1];
        if(data_in->command == kImuCommandDefaultOffset) {
          DefaultImuOffset(imu_offset);
        }
        else if (data_in->command == kImuCommandDefaultScale) {
          DefaultImuScale(imu_scale);
        }
        else if (data_in->command == kImuCommandCalibrateGyroOffset) {
          mRedON;
          mWhiteON;
          mGreenON;
          mAmberON;
          CalibrateImuGyroOffset(&imu_offset[3]);
          ReadImuStart();                     // start normal measurements again
          mRedOFF;
          mWhiteOFF;
          mGreenOFF;
          mAmberOFF;
        }
      }
      mWhiteOFF;
      mAmberON;
      if(datasource == ZIGBEE_DATA)
      {
        
        DropZigbeePacket(&zigbee_interface); // now that the data is safe, drop the packet from the input buffer
        // send reply
        rotations_out.motor_ang_vel = current_motor_speed;
        rotations_out.body_ang_vel = yaw_rate; //imu_metric[5];
        SendZigbeePacket(&zigbee_interface,ROT_FBK, (uint8_t*)&rotations_out, sizeof(MsgRotFbk));
        
        SendZigbeeNow(&zigbee_interface); 
        
      }
      else if(datasource == USB_DATA)
      {
        
        DropUsbPacket(); // now that the data is safe, drop the packet from the input buffer
        // send reply
        rotations_out.motor_ang_vel = current_motor_speed;
        rotations_out.body_ang_vel = imu_metric[5];
        SendUsbPacket(ROT_FBK, (uint8_t*)&rotations_out, sizeof(MsgRotFbk));
        SendUsbNow();
      }
      mAmberOFF;

    } // if(datasource != NO_DATA
    if (com_ms_since_last > com_ms_timeout)
    {
      pwm_duty = 0;
      mRedON;
    }
    else
      mRedOFF;
    
    /// CHECK FOR IMU DATA
    if(ReadImuData(imu_raw, &imu_time)) { // if IMU data is ready
      mGreenTOGGLE;
      ScaleImu(imu_raw, imu_offset, imu_scale, imu_metric); // Convert data
      ReadImuStart(); // Ask IMU for more data
    }
    
    /// CHECK FOR ALTERNATE SENSOR DATA
    if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_13))
      mGreenON;
    else
      mGreenOFF;
    
    /// Output yaw colors
    if(yaw_angle < 0)
      yaw_angle += 2*PI;
    else if (yaw_angle > 2*PI)
      yaw_angle -= 2*PI;
      
    if(yaw_angle < -2*PI)
      yaw_angle = PI;
    else if(yaw_angle > 4*PI)
      yaw_angle = PI;
    
    if(yaw_angle < 1*PI/24 || yaw_angle > 47*PI/24) // facing directly forward
    {
      mOrangeON;
    }
    else
    {
      mOrangeOFF;
    }
  }
  
  return(0);
}

void calibrate_motor_angle(float calibration_pwm_duty, uint32_t calibrate_time_ms)
{
  float current_pwms[3];
  TIM_Cmd(MAKENAME(TIM,TIMEBASE_TIMER), DISABLE); // Don't let the timer interrupt the calibration process
  _delay_ms(1);
  CalcSpaceVector(4.71238898, calibration_pwm_duty, current_pwms, trapezoidal); // Command 270 degrees.  0 torque point at 270 degrees = max torque at 0 degrees
  SetPWM(max_pwm, (uint16_t)(current_pwms[1]*max_pwm), (uint16_t)(current_pwms[0]*max_pwm), (uint16_t)(current_pwms[2]*max_pwm));
  _delay_ms(calibrate_time_ms);
  motor_encoder_calibration_angle = -EncoderTicksToRadians(encoderRead(), ENCODER_COUNTS);
  SetPWM(max_pwm, max_pwm/2, max_pwm/2, max_pwm/2);
  TIM_Cmd(MAKENAME(TIM,TIMEBASE_TIMER), ENABLE);
}

// Vector control math
void timerCallback1(void) //10khz
{
  float mechanical_angle;
  float current_pwms[3];
  float electrical_angle;
  int32_t encoder_position = encoderRead(); //~200ns???
  mechanical_angle = EncoderTicksToRadiansFast((float)encoder_position, ENCODER_TICKS_PER_REV) + motor_encoder_calibration_angle;  //unmeasurably fast
  //Start 485ns
  if (mechanical_angle < 0.0f)
  {
    mechanical_angle = mechanical_angle + 2*PI;
  }
  else if (mechanical_angle >= 2*PI)
  {
    mechanical_angle = mechanical_angle - 2*PI;
  }
  //End 485ns
  
  electrical_angle = MechToElecAngle(mechanical_angle, motor_pole_pairs) + motor_lead_elec_angle; //FAST
  
  //Start 456ns
  if (electrical_angle < 0.0f)
  {
    electrical_angle = electrical_angle + 2*PI;
  }
  else if (electrical_angle >= 2*PI)
  {
    electrical_angle = electrical_angle - 2*PI;
  }
  //End 456ns
  
  //TODO::Ensure pwm_duty is between (-1,1)
  float pwm_duty_final = pwm_duty + arm_cos_f32(yaw_angle+pulse_phase)*pulse_amplitude;
  if(pwm_duty_final >= 1)
    pwm_duty_final = 0.99999;
  else if(pwm_duty_final <= -1)
    pwm_duty_final = -0.99999;
  
  CalcSpaceVector(electrical_angle, pwm_duty_final, current_pwms,trapezoidal); //6.7us
  
  SetPWM(max_pwm, (uint16_t)(current_pwms[1]*max_pwm), (uint16_t)(current_pwms[0]*max_pwm), (uint16_t)(current_pwms[2]*max_pwm)); //1.4us
}

// Speed calculations
void timerCallback2(void) //1000hz
{
  uint8_t beacon_current = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_13);
  float beacon_old = beacon;
  current_motor_speed = compute_speed_fr(TIMEBASE_INT2_FREQ, motor_speed_smoothing_factor);
  motor_lead_elec_angle = motor_lead_elec_factor*current_motor_speed;
  //yaw_angle = yaw_angle + imu_metric[5]*0.001f;
  yaw_angle = yaw_angle + yaw_rate*0.001f;
  period_current++;
  beacon = (beacon_old + beacon_current)*beacon_current;
  if (beacon >= 10 && beacon_old < 10)
  {
    yaw_angle = yaw_angle_setpoint;
    period_previous = period_current;
    period_current = 0;
    yaw_rate = 1.0/period_previous*1000.0*2*PI;
  }
  com_ms_since_last++;
}
