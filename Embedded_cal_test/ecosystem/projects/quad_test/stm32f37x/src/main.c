#include "stm32f37x.h"
#include "mGeneral.h"
#include "common_peripherals.h"

/// Include your custom headers here

#include "timer4_4pwm.h"
//#include "timer19_4pwm.h"
#include "types.h"

/// Initialize variables here

#define MIMU 0b11010000
#define MAX_THRUST (0.9f) // Max thrust in Newtons
#define ZB_RX_FRAME_OVERHEAD (6) // Message type + 2 CRC + LQI + ED + Status

static float normalize_angle(float angle);
static void calcControl(float des_thrust, float des_yaw, float des_pitch, float des_roll);

static const float pi = 3.14159265358979;
static float Kp = 700, Kd = 100;
static float Kp_yaw = 300, Kd_yaw = 150;
static uint8_t motors_on = 0;
static uint16_t pwm1, pwm2, pwm3, pwm4;
static struct IMU_DATA imu_data;
static uint8_t crash = 0;

// main function
int main(void)
{
  InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h

  /// Initialize code here

  TIM4_Config(); // TIM3 used for PWM
  //TIM19_Config(); // TIM19 used for PWM because of board mess up

  mRedON;


  /* TIM3 enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Prescaler = 7200-1; // 1 timer tick = 0.1 ms
  TIM_TimeBaseStructure.TIM_Period = 10000-1; // timer period = 1 sec
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  mGreenON;


  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_Cmd(TIM3, ENABLE);

  mBlueON;


  uint8_t mBus1_ret;
  mBus1_ret = mBusPeriphReady(MIMU,1);
  while(!mBus1_ret)
  {
    mBus1_ret = mBusPeriphReady(MIMU,1);
    _delay_ms(100);
  }

  mWhiteON;


  mBusWrite(MIMU, 0x6B, 0x01,1);
  _delay_ms(1);
  mBusWrite(MIMU, 0x1A, 0x02,1); // DLPF: Bandwidth ~100 Hz
  _delay_ms(1);
  mBusWrite(MIMU, 0x1C, 0<<3,1); // +-4g, don't look at the datasheet
  _delay_ms(1);
  mBusWrite(MIMU, 0x1B, 1<<3,1); // +-500 deg/s
  _delay_ms(1);

  mOrangeON;

  _delay_ms(500);
  _delay_ms(500);

  mRedOFF;
  mGreenOFF;
  mBlueOFF;
  mWhiteOFF;
  mOrangeOFF;

  uint16_t t_old = 0, t_now = 0;

  const int16_t imu_offset[6] = {0, 0, 4500, 0, 0, 0};
  const float gyro_sensitivity = 65.5*180/pi;
  const float alpha = 0.001;

  float roll = 0, pitch = 0, yaw = 0;
  float des_thrust = 0.0, des_roll = 0, des_pitch = 0, des_yaw = 0;
  float current_yaw = 0;
  uint8_t enable_motors = 0;
  uint8_t use_external_yaw = 0;
  uint8_t rx_buf[150];
  uint8_t tx_buf[150];

  int count = 0;
  uint8_t ret;

  while(1)
  {
    t_old = t_now;
    t_now = TIM_GetCounter(TIM3);
    uint8_t raw_imu_data_buf[14];

    ret = mBusReadBurstStartNB(MIMU, 0x3B, 14,1);
    mWhiteON;

    uint8_t zb_irq_stat = ZigBee_ReadIrq();
    if((zb_irq_stat & 0x08) == 0x08)
    {
      mGreenTOGGLE;
      uint8_t rx_len = 0;
      ZigBee_ReadFrame(rx_buf, &rx_len);
      if(rx_len > 2)
      {
        if(rx_buf[rx_len-1] & 0x80) // Valid CRC
        {
          const uint8_t data_len = rx_len - ZB_RX_FRAME_OVERHEAD;
          if((rx_buf[0] == TYPE_TRPY_CMD) && (data_len == sizeof(struct TRPY_CMD))) // TRPY command
          {
            mBlueTOGGLE;
            struct TRPY_CMD *trpy_cmd = (struct TRPY_CMD*) &rx_buf[1];
            des_thrust = trpy_cmd->thrust*1e-4f;
            des_roll = trpy_cmd->roll*1e-4f;
            des_pitch = trpy_cmd->pitch*1e-4f;
            des_yaw = trpy_cmd->yaw*1e-4f;
            current_yaw = trpy_cmd->current_yaw*1e-4f;
            enable_motors = trpy_cmd->enable_motors;
            use_external_yaw = trpy_cmd->use_external_yaw;
          }
          else if((rx_buf[0] == TYPE_GAINS_CMD) && (data_len == sizeof(struct GAINS_CMD))) // Gains command
          {
            mBlueTOGGLE;
            struct GAINS_CMD *gains_cmd = (struct GAINS_CMD*) &rx_buf[1];
            Kp = gains_cmd->Kp;
            Kd = gains_cmd->Kd;
            Kp_yaw = gains_cmd->Kp_yaw;
            Kd_yaw = gains_cmd->Kd_yaw;
          }
        }
      }
      ZigBee_SetModeTx();
      tx_buf[0] = TYPE_OUTPUT_DATA; // Packet type
      struct OUTPUT_DATA *output_data = (struct OUTPUT_DATA*) &tx_buf[1];
      output_data->time = t_now;
      output_data->des_thrust = des_thrust*1e4f;
      output_data->des_roll = des_roll*1e4f;
      output_data->des_pitch = des_pitch*1e4f;
      output_data->des_yaw = des_yaw*1e4f;
      output_data->est_roll = roll*1e4f;
      output_data->est_pitch = pitch*1e4f;
      output_data->est_yaw = yaw*1e4f;
      output_data->est_angvel_x = imu_data.angvel_x*1e3f;
      output_data->est_angvel_y = imu_data.angvel_y*1e3f;
      output_data->est_angvel_z = imu_data.angvel_z*1e3f;
      output_data->est_acc_x = imu_data.acc_x*1e4f;
      output_data->est_acc_y = imu_data.acc_y*1e4f;
      output_data->est_acc_z = imu_data.acc_z*1e4f;
      output_data->pwm1 = pwm1;
      output_data->pwm2 = pwm2;
      output_data->pwm3 = pwm3;
      output_data->pwm4 = pwm4;
      ret = ZigBee_ReadState();
      while(ret != STATUS_PLL_ON)
      {
        ret = ZigBee_ReadState();
      }
      ZigBee_WriteFrame(tx_buf, 1+sizeof(struct OUTPUT_DATA));
      ret = ZigBee_ReadIrq();
      while((ret & 0x08) == 0x00)
      {
        ret = ZigBee_ReadIrq();
      }
      ZigBee_SetModeRx();
    }

    ret = mBusReadBurstDataNB(14,raw_imu_data_buf,1);
    while(ret == ERROR)
    {
      ret = mBusReadBurstDataNB(14,raw_imu_data_buf,1);
    }
    mWhiteOFF;

    int16_t imu_data_buf[6];
    imu_data_buf[0] = (256*raw_imu_data_buf[0] + raw_imu_data_buf[1]) - imu_offset[0];
    imu_data_buf[1] = (256*raw_imu_data_buf[2] + raw_imu_data_buf[3]) - imu_offset[1];
    imu_data_buf[2] = (256*raw_imu_data_buf[4] + raw_imu_data_buf[5]) - imu_offset[2];

    //int16_t tempr = 256*raw_imu_data_buf[6] + raw_imu_data_buf[7];

    imu_data_buf[3] = (256*raw_imu_data_buf[8] + raw_imu_data_buf[9]) - imu_offset[3];
    imu_data_buf[4] = (256*raw_imu_data_buf[10] + raw_imu_data_buf[11]) - imu_offset[4];
    imu_data_buf[5] = (256*raw_imu_data_buf[12] + raw_imu_data_buf[13]) - imu_offset[5];

    float dt = (t_now - t_old)*1e-4f;
    if(t_now < t_old)
      dt += 1;

    float acc_pitch;
    //acc_pitch  = atan2f(-imu_data_buf[0], imu_data_buf[2]);
    if(imu_data_buf[0] >= 8192)
      acc_pitch = -pi/2;
    else if(imu_data_buf[0] <= -8192)
      acc_pitch = pi/2;
    else
      acc_pitch  = asinf(-imu_data_buf[0]/8192.0f);
    float acc_roll   = atan2f(imu_data_buf[1], imu_data_buf[2]);
    float gyro_roll  = imu_data_buf[3]/gyro_sensitivity;
    float gyro_pitch = imu_data_buf[4]/gyro_sensitivity;
    float gyro_yaw   = imu_data_buf[5]/gyro_sensitivity;
    float acc_x      = imu_data_buf[0]/8192.0f;
    float acc_y      = imu_data_buf[1]/8192.0f;
    float acc_z      = imu_data_buf[2]/8192.0f;

    roll = alpha*acc_roll + (1-alpha)*roll + gyro_roll*dt;
    pitch = alpha*acc_pitch + (1-alpha)*pitch + gyro_pitch*dt;
    if(use_external_yaw)
      yaw = current_yaw;
    else
      yaw += gyro_yaw*dt;
    yaw = normalize_angle(yaw);

    imu_data.roll = roll;
    imu_data.pitch = pitch;
    imu_data.yaw = yaw;
    imu_data.angvel_x = gyro_roll;
    imu_data.angvel_y = gyro_pitch;
    imu_data.angvel_z = gyro_yaw;
    imu_data.acc_x = acc_x;
    imu_data.acc_y = acc_y;
    imu_data.acc_z = acc_z;

    if(fabsf(roll) > pi/3 || fabsf(pitch) > pi/3 )
    {
      updatePWM(0,0,0,0);
      mOrangeOFF;
      motors_on = 0;
      crash = 1;
    }

    if(des_thrust <= 0  || !enable_motors || crash)
    {
      motors_on = 0;
      yaw = 0;
      if (!enable_motors)
        crash = 0;
      mOrangeOFF;
    }
    else
    {
      motors_on = 1;
      mOrangeON;
    }
    calcControl(des_thrust, des_yaw, des_pitch, des_roll);
    if(count == 500)
    {
      mRedTOGGLE;
      count = 0;
    }
    count++;
  }
}

static void calcControl(float des_thrust, float des_yaw, float des_pitch, float des_roll)
{
  float roll = imu_data.roll;
  float pitch = imu_data.pitch;
  float yaw = imu_data.yaw;

  // PD control - attitude
  //float d_omega_x = Kp*normalize_angle(des_roll - roll) + Kd*(0 - imu_data.angvel_x);
  //float d_omega_y = Kp*normalize_angle(des_pitch - pitch) + Kd*(0 - imu_data.angvel_y);
  float d_omega_z = Kp_yaw*normalize_angle(des_yaw - yaw) + Kd_yaw*(0 - imu_data.angvel_z);
  
  // No PD - attitude
  float d_omega_x = Kp*normalize_angle(des_roll);
  float d_omega_y = Kp*normalize_angle(des_pitch);
  //float d_omega_z = Kp_yaw*normalize_angle(des_yaw);

  float pwm_base = (des_thrust/MAX_THRUST)*1000; // PWM->Thrust relation seems almost linear in testing
  int pwm[4];
  pwm[0] = pwm_base - d_omega_y + d_omega_z;
  pwm[1] = pwm_base + d_omega_x - d_omega_z;
  pwm[2] = pwm_base + d_omega_y + d_omega_z;
  pwm[3] = pwm_base - d_omega_x - d_omega_z;
  for(int i = 0; i < 4; i++)
  {
    if(pwm[i] < 0)
      pwm[i] = 0;
    else if(pwm[i] > 1000)
      pwm[i] = 1000;
  }
  if(motors_on)
  {
    pwm1 = pwm[0];
    pwm2 = pwm[1];
    pwm3 = pwm[2];
    pwm4 = pwm[3];
    mOrangeON;
  }
  else
  {
    pwm1 = pwm2 = pwm3 = pwm4 = 0;
    mOrangeOFF;
  }
  updatePWM(pwm1, pwm2, pwm3, pwm4);
}

static float normalize_angle(float angle)
{
  while(angle <= -pi)
    angle += 2*pi;

  while(angle > pi)
    angle -= 2*pi;

  return angle;
}
