/******************************************************************************
    Filename: hal_rf.c

    This file contains functions for accessing the CC1100/CC2500 family
    of RF ICs from Texas Instruments.

    Copyright 2007 Texas Instruments, Inc.
******************************************************************************/
#include "spi.h"
#include "hal_rf.h"
#include "cc2500.h"



uint8_t halSpiWrite(uint8_t addr, const uint8_t* data, uint16_t length)
{
    uint16_t i;
    uint8_t rc;
		uint32_t timeOut = SPI_TIMEOUT;
		// Send SRES command
    HAL_SPI2_CS_ASSERT;
    while(HAL_SPI2_SOMI_VAL)
		{
			if(timeOut-- == 0) return 0xff;
		}
		rc = halSpiWriteByte(SPI2,addr);
		for (i = 0; i < length; i++)
		{
			halSpiWriteByte(SPI2,data[i]);
		}
		HAL_SPI2_CS_DEASSERT;
		return(rc);
}

uint8_t halSpiRead(uint8_t addr, uint8_t* data, uint16_t length)
{
    uint16_t i;
    uint8_t rc;
		uint32_t timeOut = SPI_TIMEOUT;
		// Send SRES command
    HAL_SPI2_CS_ASSERT;
    while(HAL_SPI2_SOMI_VAL)
		{
			if(timeOut-- == 0) return 0xff;
		}
		rc = halSpiWriteByte(SPI2,addr);
		for (i = 0; i < length; i++)
		{
			data[i] = halSpiWriteByte(SPI2,0xff);
		}
		HAL_SPI2_CS_DEASSERT;
    return(rc);
}


uint8_t halSpiStrobe(uint8_t cmd)
{
    uint8_t rc;
		uint32_t timeOut = SPI_TIMEOUT;
		// Send SRES command
    HAL_SPI2_CS_ASSERT;
    while(HAL_SPI2_SOMI_VAL)
		{
			if(timeOut-- == 0) return 0xff;
		}
		rc = halSpiWriteByte(SPI2,cmd);
		HAL_SPI2_CS_DEASSERT;	
    return(rc);
}


void halMcuWaitUs(uint32_t uS)
{
	uS *= 10;//=48MhZ/5 ; 5 is for while loop
	while(uS--);
}
//----------------------------------------------------------------------------------
//  void halRfResetChip(void)
//
//  DESCRIPTION:
//    Resets the chip using the procedure described in the datasheet.
//----------------------------------------------------------------------------------
void halRfResetChip(void)
{
		uint32_t timeOut = SPI_TIMEOUT;
    // Toggle chip select signal
    HAL_SPI2_CS_DEASSERT;
    halMcuWaitUs(30);
    HAL_SPI2_CS_ASSERT;
    halMcuWaitUs(30);
    HAL_SPI2_CS_DEASSERT;
    halMcuWaitUs(45);

    // Send SRES command
    HAL_SPI2_CS_ASSERT;
    while(HAL_SPI2_SOMI_VAL)
		{
			if(timeOut-- == 0) return;
		}
    halSpiWriteByte(SPI2,CC2500_SRES);
    // Wait for chip to finish internal reset
    while (HAL_SPI2_SOMI_VAL)
		{
			if(timeOut-- == 0) return;
		}
    HAL_SPI2_CS_DEASSERT;
}

//----------------------------------------------------------------------------------
//  void halRfConfig(const HAL_RF_CONFIG* rfConfig, const uint8_t* rfPaTable, uint8_t rfPaTableLen)
//
//  DESCRIPTION:
//    Used to configure the CC1100/CC2500 registers with exported register
//    settings from SmartRF Studio.
//
//  ARGUMENTS:
//    rfConfig     - register settings (as exported from SmartRF Studio)
//    rfPaTable    - array of PA table values (from SmartRF Studio)
//    rfPaTableLen - length of PA table
//
//----------------------------------------------------------------------------------
void halRfConfig(const HAL_RF_CONFIG* rfConfig, const uint8_t* rfPaTable, uint8_t rfPaTableLen)
{
	

    halRfWriteReg(CC2500_FSCTRL1,  rfConfig->fsctrl1);    // Frequency synthesizer control.
    halRfWriteReg(CC2500_FSCTRL0,  rfConfig->fsctrl0);    // Frequency synthesizer control.
    halRfWriteReg(CC2500_FREQ2,    rfConfig->freq2);      // Frequency control word, high byte.
    halRfWriteReg(CC2500_FREQ1,    rfConfig->freq1);      // Frequency control word, middle byte.
    halRfWriteReg(CC2500_FREQ0,    rfConfig->freq0);      // Frequency control word, low byte.
    halRfWriteReg(CC2500_MDMCFG4,  rfConfig->mdmcfg4);    // Modem configuration.
    halRfWriteReg(CC2500_MDMCFG3,  rfConfig->mdmcfg3);    // Modem configuration.
    halRfWriteReg(CC2500_MDMCFG2,  rfConfig->mdmcfg2);    // Modem configuration.
    halRfWriteReg(CC2500_MDMCFG1,  rfConfig->mdmcfg1);    // Modem configuration.
    halRfWriteReg(CC2500_MDMCFG0,  rfConfig->mdmcfg0);    // Modem configuration.
    halRfWriteReg(CC2500_CHANNR,   rfConfig->channr);     // Channel number.
    halRfWriteReg(CC2500_DEVIATN,  rfConfig->deviatn);    // Modem deviation setting (when FSK modulation is enabled).
    halRfWriteReg(CC2500_FREND1,   rfConfig->frend1);     // Front end RX configuration.
    halRfWriteReg(CC2500_FREND0,   rfConfig->frend0);     // Front end RX configuration.
    halRfWriteReg(CC2500_MCSM0,    rfConfig->mcsm0);      // Main Radio Control State Machine configuration.
    halRfWriteReg(CC2500_FOCCFG,   rfConfig->foccfg);     // Frequency Offset Compensation Configuration.
    halRfWriteReg(CC2500_BSCFG,    rfConfig->bscfg);      // Bit synchronization Configuration.
    halRfWriteReg(CC2500_AGCCTRL2, rfConfig->agcctrl2);   // AGC control.
    halRfWriteReg(CC2500_AGCCTRL1, rfConfig->agcctrl1);   // AGC control.
    halRfWriteReg(CC2500_AGCCTRL0, rfConfig->agcctrl0);   // AGC control.
    halRfWriteReg(CC2500_FSCAL3,   rfConfig->fscal3);     // Frequency synthesizer calibration.
    halRfWriteReg(CC2500_FSCAL2,   rfConfig->fscal2);     // Frequency synthesizer calibration.
    halRfWriteReg(CC2500_FSCAL1,   rfConfig->fscal1);     // Frequency synthesizer calibration.
    halRfWriteReg(CC2500_FSCAL0,   rfConfig->fscal0);     // Frequency synthesizer calibration.
    halRfWriteReg(CC2500_FSTEST,   rfConfig->fstest);     // Frequency synthesizer calibration.
    halRfWriteReg(CC2500_TEST2,    rfConfig->test2);      // Various test settings.
    halRfWriteReg(CC2500_TEST1,    rfConfig->test1);      // Various test settings.
    halRfWriteReg(CC2500_TEST0,    rfConfig->test0);      // Various test settings.
    halRfWriteReg(CC2500_IOCFG2,   rfConfig->iocfg2);     // GDO2 output pin configuration.
    halRfWriteReg(CC2500_IOCFG0,   rfConfig->iocfg0);     // GDO0 output pin configuration.
    halRfWriteReg(CC2500_PKTCTRL1, rfConfig->pktctrl1);   // Packet automation control.
    halRfWriteReg(CC2500_PKTCTRL0, rfConfig->pktctrl0);   // Packet automation control.
    halRfWriteReg(CC2500_ADDR,     rfConfig->addr);       // Device address.
    halRfWriteReg(CC2500_PKTLEN,   rfConfig->pktlen);     // Packet length.

    halSpiWrite(CC2500_PATABLE | CC2500_WRITE_BURST, rfPaTable, rfPaTableLen);
}


//----------------------------------------------------------------------------------
//  void  halRfBurstConfig(const HAL_RF_BURST_CONFIG rfConfig, const uint8_t* rfPaTable, uint8_t rfPaTableLen)
//
//  DESCRIPTION:
//    Used to configure all of the CC1100/CC2500 registers in one burst write.
//
//  ARGUMENTS:
//    rfConfig     - register settings
//    rfPaTable    - array of PA table values (from SmartRF Studio)
//    rfPaTableLen - length of PA table
//
//----------------------------------------------------------------------------------
void  halRfBurstConfig(const HAL_RF_BURST_CONFIG rfConfig, const uint8_t* rfPaTable, uint8_t rfPaTableLen)
{
    halSpiWrite(CC2500_IOCFG2  | CC2500_WRITE_BURST, rfConfig, sizeof(rfConfig));
    halSpiWrite(CC2500_PATABLE | CC2500_WRITE_BURST, rfPaTable, rfPaTableLen);
}

//----------------------------------------------------------------------------------
//  uint8_t halRfGetChipId(void)
//----------------------------------------------------------------------------------
uint8_t halRfGetChipId(void)
{
    return(halRfReadStatusReg(CC2500_PARTNUM));
}

//----------------------------------------------------------------------------------
//  uint8_t halRfGetChipVer(void)
//----------------------------------------------------------------------------------
uint8_t halRfGetChipVer(void)
{
    return(halRfReadStatusReg(CC2500_VERSION));
}

//----------------------------------------------------------------------------------
//  HAL_RF_STATUS halRfStrobe(uint8_t cmd)
//----------------------------------------------------------------------------------
HAL_RF_STATUS halRfStrobe(uint8_t cmd)
{
    return(halSpiStrobe(cmd));
}

//----------------------------------------------------------------------------------
//  uint8_t halRfReadStatusReg(uint8_t addr)
//
//  NOTE:
//      When reading a status register over the SPI interface while the register
//      is updated by the radio hardware, there is a small, but finite, probability
//      that the result is corrupt. The CC1100 and CC2500 errata notes explain the
//      problem and propose several workarounds.
//
//----------------------------------------------------------------------------------
uint8_t halRfReadStatusReg(uint8_t addr)
{
    uint8_t reg;
    halSpiRead(addr | CC2500_READ_BURST, &reg, 1);
    return(reg);
}

//----------------------------------------------------------------------------------
//  uint8_t halRfReadReg(uint8_t addr)
//----------------------------------------------------------------------------------
uint8_t halRfReadReg(uint8_t addr)
{
    uint8_t reg;
    halSpiRead(addr | CC2500_READ_SINGLE, &reg, 1);
    return(reg);
}

//----------------------------------------------------------------------------------
//  HAL_RF_STATUS halRfWriteReg(uint8_t addr, uint8_t data)
//----------------------------------------------------------------------------------
HAL_RF_STATUS halRfWriteReg(uint8_t addr, uint8_t data)
{
    uint8_t rc;
    rc = halSpiWrite(addr, &data, 1);
    return(rc);
}

//----------------------------------------------------------------------------------
//  HAL_RF_STATUS halRfWriteFifo(uint8_t* data, uint8_t length)
//----------------------------------------------------------------------------------
HAL_RF_STATUS halRfWriteFifo(const uint8_t* data, uint8_t length)
{
    return(halSpiWrite(CC2500_TXFIFO | CC2500_WRITE_BURST, data, length));
}

//----------------------------------------------------------------------------------
//  HAL_RF_STATUS halRfReadFifo(uint8_t* data, uint8_t length)
//----------------------------------------------------------------------------------
HAL_RF_STATUS halRfReadFifo(uint8_t* data, uint8_t length)
{
    return(halSpiRead(CC2500_RXFIFO | CC2500_READ_BURST, data, length));
}

//----------------------------------------------------------------------------------
//  uint8_t halRfGetTxStatus(void)
//
//  DESCRIPTION:
//      This function transmits a No Operation Strobe (SNOP) to get the status of
//      the radio and the number of free bytes in the TX FIFO
//
//      Status byte:
//
//      ---------------------------------------------------------------------------
//      |          |            |                                                 |
//      | CHIP_RDY | STATE[2:0] | FIFO_BYTES_AVAILABLE (free bytes in the TX FIFO |
//      |          |            |                                                 |
//      ---------------------------------------------------------------------------
//
//  NOTE:
//      When reading a status register over the SPI interface while the register
//      is updated by the radio hardware, there is a small, but finite, probability
//      that the result is corrupt. This also applies to the chip status byte. The
//      CC1100 and CC2500 errata notes explain the problem and propose several
//      workarounds.
//
//----------------------------------------------------------------------------------
HAL_RF_STATUS halRfGetTxStatus(void)
{
    return(halSpiStrobe(CC2500_SNOP));
}

//----------------------------------------------------------------------------------
//  uint8_t halRfGetRxStatus(void)
//
//  DESCRIPTION:
//      This function transmits a No Operation Strobe (SNOP) with the read bit set
//      to get the status of the radio and the number of available bytes in the RX
//      FIFO.
//
//      Status byte:
//
//      --------------------------------------------------------------------------------
//      |          |            |                                                      |
//      | CHIP_RDY | STATE[2:0] | FIFO_BYTES_AVAILABLE (available bytes in the RX FIFO |
//      |          |            |                                                      |
//      --------------------------------------------------------------------------------
//
//  NOTE:
//      When reading a status register over the SPI interface while the register
//      is updated by the radio hardware, there is a small, but finite, probability
//      that the result is corrupt. This also applies to the chip status byte. The
//      CC1100 and CC2500 errata notes explain the problem and propose several
//      workarounds.
//
//----------------------------------------------------------------------------------
HAL_RF_STATUS halRfGetRxStatus(void)
{
    return(halSpiStrobe(CC2500_SNOP | CC2500_READ_SINGLE));
}


