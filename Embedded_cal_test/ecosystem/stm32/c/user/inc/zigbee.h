//Copyright KMel Robotics, LLC, 2012

#ifndef ZIGBEE_H
#define ZIGBEE_H

#include "common_peripherals.h"
#include STM32FXXX_H
#include STM32FXXX_GPIO_H

#define REG_TRX_STATUS   0x01
#define REG_TRX_STATE    0x02
#define REG_TRX_CTRL_0   0x03
#define REG_TRX_CTRL_1   0x04

#define REG_PHY_TX_PWR   0x05
#define REG_PHY_RSSI     0x06
#define REG_PHY_ED_LEVEL 0x07
#define REG_PHY_CC_CCA   0x08
#define REG_CCA_THRES    0x09

#define REG_RX_CTRL      0x0A
#define REG_SFD_VALUE    0x0B
#define REG_TRX_CTRL_2   0x0C
#define REG_ANT_DIV      0x0D
#define REG_IRQ_MASK     0x0E
#define REG_IRQ_STATUS   0x0F
#define REG_VREG_CTRL    0x10
#define REG_BATMON       0x11
#define REG_XOSC_CTRL    0x12
#define REG_CC_CTRL_0    0x13
#define REG_CC_CTRL_1    0x14
#define REG_RX_SYN       0x15

#define REG_XAH_CTRL_1   0x17
#define REG_FTN_CTRL     0x18
#define REG_PLL_CF       0x1A
#define REG_PLL_DCU      0x1B

#define REG_PART_NUM     0x1C
#define REG_VERS_NUM     0x1D

#define REG_MAN_ID_0     0x1E
#define REG_MAN_ID_1     0x1F

#define REG_SHORT_ADDR_0 0x20
#define REG_SHORT_ADDR_1 0x21
#define REG_PAN_ID_0     0x22
#define REG_PAN_ID_1     0x23
#define REG_IEEE_ADDR_0  0x24
#define REG_IEEE_ADDR_1  0x25
#define REG_IEEE_ADDR_2  0x26
#define REG_IEEE_ADDR_3  0x27
#define REG_IEEE_ADDR_4  0x28
#define REG_IEEE_ADDR_5  0x29
#define REG_IEEE_ADDR_6  0x2A
#define REG_IEEE_ADDR_7  0x2B
#define REG_XAH_CTRL_0   0x2C
#define REG_CSMA_SEED_0  0x2D
#define REG_CSMA_SEED_1  0x2E
#define REG_CSMA_BE      0x2F


#define CMD_NOP           0x00
#define CMD_TX_START      0x02
#define CMD_FORCE_TRX_OFF 0x03
#define CMD_FORCE_PLL_ON  0x04
#define CMD_RX_ON         0x06
#define CMD_TRX_OFF       0x08
#define CMD_PLL_ON        0x09
#define CMD_RX_AACK_ON    0x16
#define CMD_TX_ARET_ON    0x19

#define STATUS_P_ON               0x00
#define STATUS_BUSY_RX            0x01
#define STATUS_BUSY_TX            0x02
#define STATUS_RX_ON              0x06
#define STATUS_TRX_OFF            0x08
#define STATUS_PLL_ON             0x09
#define STATUS_SLEEP              0x0F
#define STATUS_BUSY_RX_AACK       0x11
#define STATUS_BUSY_TX_ARET       0x12
#define STATUS_RX_AACK_ON         0x16
#define STATUS_TX_ARET_ON         0x19
#define STATUS_RX_ON_NOCLK        0x1C
#define STATUS_RX_AACK_ON_NOCLK   0x1D
#define STATUS_BUSY_RX_AACK_NOCLK 0x1E
#define STATUS_STATE_TRANSITION   0x1F

#define MAX_ZIGBEE_PACKET_BYTES 125

#ifdef __cplusplus
extern "C" {
#endif

	int ZigBee_InitCommon();
	int ZigBee_Init();
	void ZigBee_ReadReg(uint8_t addr, uint8_t len, uint8_t *dest);
	void ZigBee_WriteReg(uint8_t addr, uint8_t val);
	uint8_t ZigBee_WriteFrame(const void* data, uint8_t len);
	void ZigBee_ReadFrame(uint8_t *data, uint8_t *len);
	int ZigBee_CheckPartAndVers();
	int ZigBee_ReadIrq();
	int ZigBee_SetModeRx();
	int ZigBee_SetModeTx();
	int ZigBee_SetChannel(uint8_t ch);
	int ZigBee_EnableTrxEndIrq();
	uint8_t ZigBee_ReadState();
	void ZigBee_Reset(void);
	int ZigBee_PllOn();
	int ZigBee_ForceTrxOff();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
