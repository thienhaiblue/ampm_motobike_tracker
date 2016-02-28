/***********************************************************************************
    Filename: hal_rf.h

    Copyright 2007 Texas Instruments, Inc.
***********************************************************************************/

#ifndef HAL_RF_H
#define HAL_RF_H
#include <stdint.h>


//----------------------------------------------------------------------------------
// Type declarations
//----------------------------------------------------------------------------------

// The following structure can be used when configuring the RF chip. SmartRF Studio
// can be used to export a predefined instance of this structure.
typedef struct {
    uint8_t fsctrl1;    // Frequency synthesizer control.
    uint8_t fsctrl0;    // Frequency synthesizer control.
    uint8_t freq2;      // Frequency control word, high byte.
    uint8_t freq1;      // Frequency control word, middle byte.
    uint8_t freq0;      // Frequency control word, low byte.
    uint8_t mdmcfg4;    // Modem configuration.
    uint8_t mdmcfg3;    // Modem configuration.
    uint8_t mdmcfg2;    // Modem configuration.
    uint8_t mdmcfg1;    // Modem configuration.
    uint8_t mdmcfg0;    // Modem configuration.
    uint8_t channr;     // Channel number.
    uint8_t deviatn;    // Modem deviation setting (when FSK modulation is enabled).
    uint8_t frend1;     // Front end RX configuration.
    uint8_t frend0;     // Front end RX configuration.
    uint8_t mcsm0;      // Main Radio Control State Machine configuration.
    uint8_t foccfg;     // Frequency Offset Compensation Configuration.
    uint8_t bscfg;      // Bit synchronization Configuration.
    uint8_t agcctrl2;   // AGC control.
    uint8_t agcctrl1;   // AGC control.
    uint8_t agcctrl0;   // AGC control.
    uint8_t fscal3;     // Frequency synthesizer calibration.
    uint8_t fscal2;     // Frequency synthesizer calibration.
    uint8_t fscal1;     // Frequency synthesizer calibration.
    uint8_t fscal0;     // Frequency synthesizer calibration.
    uint8_t fstest;     // Frequency synthesizer calibration.
    uint8_t test2;      // Various test settings.
    uint8_t test1;      // Various test settings.
    uint8_t test0;      // Various test settings.
    uint8_t iocfg2;     // GDO2 output pin configuration.
    uint8_t iocfg0;     // GDO0 output pin configuration.
    uint8_t pktctrl1;   // Packet automation control.
    uint8_t pktctrl0;   // Packet automation control.
    uint8_t addr;       // Device address.
    uint8_t pktlen;     // Packet length.
} HAL_RF_CONFIG;

// For rapid chip configuration with a minimum of function overhead.
// The array has to be set up with predefined values for all registers.
typedef uint8_t HAL_RF_BURST_CONFIG[47];

// The chip status byte, returned by chip for all SPI accesses
typedef uint8_t HAL_RF_STATUS;



//----------------------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------------------
uint8_t RF_Init(void);
void  halRfConfig(const HAL_RF_CONFIG* rfConfig, const uint8_t* rfPaTable, uint8_t rfPaTableLen);
void  halRfBurstConfig(const HAL_RF_BURST_CONFIG rfConfig, const uint8_t* rfPaTable, uint8_t rfPaTableLen);
void  halRfResetChip(void);
uint8_t halRfGetChipId(void);
uint8_t halRfGetChipVer(void);
uint8_t halRfReadStatusReg(uint8_t addr);
uint8_t halRfReadReg(uint8_t addr);

HAL_RF_STATUS halRfWriteReg(uint8_t addr, uint8_t data);
HAL_RF_STATUS halRfWriteFifo(const uint8_t* data, uint8_t length);
HAL_RF_STATUS halRfReadFifo(uint8_t* data, uint8_t length);
HAL_RF_STATUS halRfStrobe(uint8_t cmd);
HAL_RF_STATUS halRfGetTxStatus(void);
HAL_RF_STATUS halRfGetRxStatus(void);


/**********************************************************************************/
#endif
