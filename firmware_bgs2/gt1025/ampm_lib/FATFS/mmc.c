#include "ff.h"
#include "spi.h"
#include "tick.h"
#include "diskio.h"
#include "hw_config.h"

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

#define BIT_0			1
#define BIT_1			2

#define SD_PWR_Assert()			
#define SD_PWR_DeAssert()		

#define SD_CS_Assert()			SPI_SD_CS_PIN_CLR
#define SD_CS_DeAssert()		SPI_SD_CS_PIN_SET

#define SD_CardDetect()			(0)
#define SD_WriteProtect()		(0)

#define SPI_Write(x)				halSpiWriteByte(SPI1,x)

#define LOW_BIT_RATE			100000
#define HIGH_BIT_RATE			5000000

/* Definitions for MMC/SDC command */
#define SD_CMD_IDLE					(0x40+0)	/* GO_IDLE_STATE */
#define SD_CMD_OP_COND				(0x40+1)	/* SEND_OP_COND (MMC) */
#define SD_CMD_IF_COND				(0x40+8)	/* SEND_IF_COND */
#define SD_CMD_CSD					(0x40+9)	/* SEND_CSD */
#define SD_CMD_CID					(0x40+10)	/* SEND_CID */
#define SD_CMD_STOP_TRANSMISSION	(0x40+12)	/* STOP_TRANSMISSION */
#define SD_CMD_SET_BLOCKLEN			(0x40+16)	/* SET_BLOCKLEN */
#define SD_CMD_READ_SINGLE_BLOCK	(0x40+17)	/* READ_SINGLE_BLOCK */
#define SD_CMD_READ_MULTIPLE_BLOCK	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define SD_CMD_SET_BLOCK_COUNT		(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define SD_CMD_WRITE_BLOCK			(0x40+24)	/* WRITE_BLOCK */
#define SD_CMD_WRITE_MULTIPLE_BLOCK	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define SD_CMD_APP_CMD				(0x40+55)	/* APP_CMD */
#define SD_CMD_READ_OCR				(0x40+58)	/* READ_OCR */
#define SD_ACMD_OP_COND						(0xC0+41)	/* SEND_OP_COND (SDC) */
#define SD_ACMD_STATUS						(0xC0+13)	/* SD_STATUS (SDC) */
#define SD_ACMD_SET_WR_BLOCK_ERASE_COUNT	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */


static BYTE SD__SendCommand(BYTE cmd, DWORD arg);
static DWORD SD__Read(BYTE *buffer, UINT size);
static DWORD SD__Write(const BYTE *buffer, BYTE token);
static BYTE SD__WaitReady(void);

static volatile DSTATUS Stat = STA_NOINIT; // disk status
static UINT CardType; // b0:MMC, b1:SDv1, b2:SDv2, b3:Block Addressing
static volatile UINT sdTimer1, sdTimer2; // timeout timers 

DSTATUS MMC_Init(void)
{ 
    BYTE n, cmd;
    BYTE bData[4];
	
	SD_PWR_DeAssert();
	DelayMs(500);
	SD_PWR_Assert();
	DelayMs(500);

	if(Stat & STA_NODISK)
		return Stat;		// no card in the socket
	
	// open SPI at low speed to init SD card
	SPI_InitLowFreq(1);
	
	for(n=0;n<10;n++)
		SPI_Write(0xFF);
    
    // try to make the card enter idle state
    CardType = 0;
    if (SD__SendCommand(SD_CMD_IDLE, 0) == 1) {
    	sdTimer1 = 1000;
    	
	    // check for SDHC card
	    if (SD__SendCommand(SD_CMD_IF_COND, 0x1AA) == 1) {
		    // accepted, so get trailing data of R7 response
			for (n = 0; n < 4; n++) 
				bData[n] = SPI_Write(0xFF);
					
		    if ((bData[2] == 0x01) && (bData[3] == 0xAA)) {
				// card is SDC V2 can work at 2.7 to 3.6Volts   
				// Wait for leaving idle state (ACMD41 with HCS bit)
				while (sdTimer1 && SD__SendCommand(SD_ACMD_OP_COND, 1UL << 30));	
				
				// Check CCS bit in the OCR
				if (sdTimer1 && (SD__SendCommand(SD_CMD_READ_OCR, 0) == 0)) {		
					for (n = 0; n < 4; n++) 
						bData[n] = SPI_Write(0xFF);
						
					CardType = (bData[0] & 0x40) ? 12 : 4;
				}
			} 		    
		} else {
			// card is SDSC or MMC			
			if (SD__SendCommand(SD_ACMD_OP_COND, 0) <= 1) 	{
				// SDC
				CardType = 2; 
				cmd = SD_ACMD_OP_COND;
			} else {
				// MMC
				CardType = 1; 
				cmd = SD_CMD_OP_COND;
			}
			
			// wait for the card to leave idle state
			while (sdTimer1 && SD__SendCommand(cmd, 0));
			// set block length to 512
			if (!sdTimer1 || SD__SendCommand(SD_CMD_SET_BLOCKLEN, 512) != 0)
				CardType = 0;
		}		
	}   
    SD_CS_DeAssert();
    SPI_Write(0xFF);
    
    // success
 	if(CardType != 0){
 		Stat &= ~STA_NOINIT;
 		SPI_InitHighFreq(1);
	}
	
    return Stat;
}  

DSTATUS MMC_Status(void)
{
	return Stat;	
}
DRESULT MMC_Read(BYTE *buff, DWORD sector, BYTE count)
{
	if(!count)
		return RES_PARERR;
		
	if(Stat & STA_NOINIT)
		return RES_NOTRDY;

    // convert to byte address if required
    if (!(CardType & 8))
    	sector *= 512;
    
    if (count == 1) {
	    // single block read
	 	if ((SD__SendCommand(SD_CMD_READ_SINGLE_BLOCK, sector) == 0)
	 		&& (SD__Read(buff, 512)))
	 		count = 0;
	} else {
		// multiple block read
		if (SD__SendCommand(SD_CMD_READ_MULTIPLE_BLOCK, sector) == 0) {
			do {	
				if (!SD__Read(buff, 512))
					break;
				buff += 512;
			} while (--count);
			SD__SendCommand(SD_CMD_STOP_TRANSMISSION, 0);
		}
	}
      
	SD_CS_DeAssert();
    SPI_Write(0xFF);
    
	
    return count ? RES_ERROR : RES_OK;
}

static DWORD SD__Read(BYTE *buffer, UINT size)
{
	BYTE token;
	
	SD_CS_Assert();	
	
	// read the data token
	sdTimer1 = 100;
	do {
		token = SPI_Write(0xFF);
	} while ((token != 0xFE) && sdTimer1);
	
	// if invalid token return error
	if (token != 0xFE)
		return FALSE;

	while(size--){
		*buffer++ = SPI_Write(0xFF);
	}

	// read and discard the CRC
	SPI_Write(0xFF);
	SPI_Write(0xFF);
    
    return TRUE;
}

#if _READONLY == 0
DRESULT MMC_Write(const BYTE *buff, DWORD sector, BYTE count)
{
	if (!count)
		return RES_PARERR;
		
	if (Stat & STA_NOINIT)
		return RES_NOTRDY;
		
	if (Stat & STA_PROTECT)
		return RES_WRPRT;
		
	if (!(CardType & 8))
		sector *= 512;
	
	if (count == 1) {
		// single block write
		if ((SD__SendCommand(SD_CMD_WRITE_BLOCK, sector) == 0) 
			&& (SD__Write(buff, 0xFE)))
			count = 0;
	} else {
		// multiple block write
		if (CardType & 6) 
			SD__SendCommand(SD_ACMD_SET_WR_BLOCK_ERASE_COUNT, count);
		if (SD__SendCommand(SD_CMD_WRITE_MULTIPLE_BLOCK, sector) == 0) {
			do {
				if (!SD__Write(buff, 0xFC))
					break;	
				buff += 512;
			} while (--count);
			// Send stop trans token
			if (!SD__Write(0, 0xFD))
				count = 1;	
		}
	}
	SD_CS_DeAssert();
    SPI_Write(0xFF);
	
	return count ? RES_ERROR : RES_OK;
}

static DWORD SD__Write(const BYTE *buffer, BYTE token)
{
	BYTE resp;
	UINT blockCount = 512;
    
    if (SD__WaitReady() != 0xFF)
    	return FALSE;
    
    // send data token
    SPI_Write(token);
    
    if (token != 0xFD){
		do{
			SPI_Write(*buffer++);
			SPI_Write(*buffer++);
		}while(blockCount -= 2);
		
		SPI_Write(0xFF);    
		SPI_Write(0xFF);    
		resp = SPI_Write(0xFF);
		if((resp & 0x1F) != 0x05)
			return FALSE;	    
	}
    
    return TRUE;
}

#endif

DRESULT MMC_Ioctl(BYTE ctrl, void* buff)
{
	DRESULT res;
	BYTE n, csd[16], *ptr;
	DWORD csize;
	
	ptr = buff;	
	
	if (Stat & STA_NOINIT)
		return RES_NOTRDY;
		
	res = RES_ERROR;
	switch (ctrl) {
		case CTRL_SYNC:
			SD_CS_Assert();		// flush buffers
			if (SD__WaitReady() == 0xFF)
				res = RES_OK;
			break;
			
		case GET_SECTOR_COUNT:			// get number of sectors on disk
			if ((SD__SendCommand(SD_CMD_CSD, 0) == 0) &&
				(SD__Read(csd, 16))) {
				if ((csd[0] >> 6) == 1) {
					// SDC ver 2.0
					csize = csd[9] + ((WORD) csd[8]	 << 8) + 1;
					*(DWORD*)buff = (DWORD) csize << 10;
				} else {
					// MMC or SD Ver1
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					csize = (csd[8] >> 6) + ((WORD) csd[7] << 2) + 
								((WORD) (csd[6] & 3) << 10) + 1;
					*(DWORD*)buff = (DWORD)csize << (n - 9);
				}
				res = RES_OK;		
			}
			break;
			
		case GET_SECTOR_SIZE : // get size of sectors on disk 
			*(WORD*) buff = 512;
			res = RES_OK;
			break;
			
		case GET_BLOCK_SIZE : 		// get erase block size in units of sectors
			if (CardType & 4) {
				// SDC V2
				if (SD__SendCommand(SD_ACMD_STATUS, 0) == 0) {
					SPI_Write(0xFF);
					if (SD__Read(csd, 16)) { // read partial data
						for(n = 64 - 16; n; n--)
							SPI_Write(0xFF); // purge trailing data
						*(DWORD*)buff = 16UL << (csd[10] >> 4);
						res = RES_OK;
					}
				}
			} else {
				// SD ver 1 or MMC
				if ((SD__SendCommand(SD_CMD_CSD, 0) == 0) &&
					(SD__Read(csd, 16))) {
					if (CardType & 2) {
						*(DWORD*)buff = (((csd[10] & 63) << 1) +
										((WORD)(csd[11] & 128) >> 7) + 1) <<
										((csd[13] >> 6) - 1);
					} else {
						*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) *
										(((csd[11] & 3) << 3) +
										((csd[11] & 224) >> 5) + 1);
					}
					res = RES_OK;		
				}
			}
			break;
			
		case MMC_GET_TYPE:	// get card types
			*ptr = CardType;
			res = RES_OK;
			break;
						
		case MMC_GET_CSD:	// get CSD block
			if ((SD__SendCommand(SD_CMD_CSD, 0) == 0) 
				&& (SD__Read(buff, 16))) {
				res = RES_OK;
			}
			break;
			
		case MMC_GET_CID:	// get CIF block
			if ((SD__SendCommand(SD_CMD_CID, 0) == 0)
				&& (SD__Read(buff, 16))) {
				res = RES_OK;
			}
			break;
			
		case MMC_GET_OCR:	// receive OCR
			if (SD__SendCommand(SD_CMD_READ_OCR, 0) == 0) {
				for(n = 0; n < 4; n++)
					*((BYTE*)buff + n) = SPI_Write(0xFF);
				res = RES_OK;
			}
			break;
			
		case MMC_GET_SDSTAT: 	// receive SD status
			if (SD__SendCommand(SD_ACMD_STATUS, 0) == 0) {
				SPI_Write(0xFF);
				if (SD__Read(buff, 64))
					res = RES_OK;
			}
			break;
		
		default:
			res = RES_PARERR;	
	}
	
	SD_CS_DeAssert();
    SPI_Write(0xFF);	
	return res;
}
static BYTE SD__SendCommand(BYTE cmd, DWORD arg)
{
	BYTE n, res;
	
	// if we are sending an app command then we must prefix this with CMD55
	if (cmd & 0x80){
		cmd &= 0x7F;
		res = SD__SendCommand(SD_CMD_APP_CMD, 0);
		if(res > 1)
			return res;
	}
	
	SD_CS_DeAssert();
	SD_CS_Assert();
	
	if (SD__WaitReady() != 0xFF)
		return 0xFF;
	
	SPI_Write(cmd);
	SPI_Write(arg>>24);
	SPI_Write(arg>>16);
	SPI_Write(arg>>8);
	SPI_Write(arg);
	n = 0x01;			// dummy CRC + stop	
	if(cmd == SD_CMD_IDLE) // hardcoded CRC for specific commands
	n = 0x95;
	if(cmd == SD_CMD_IF_COND)
	n = 0x87;
	SPI_Write(n);
	
	// skip a stuff byte when stopping reading
	if(cmd == SD_CMD_STOP_TRANSMISSION)
		SPI_Write(0xFF);
	
	n = 10;
	do{
		res = SPI_Write(0xFF);	
	}while((res & 0x80) && --n);
	
	return res;
} 
static BYTE SD__WaitReady(void)
{
	BYTE res;
	
	sdTimer2 = 500; // wait for a timeout of 500ms
	SPI_Write(0xFF);
	do {
		res = SPI_Write(0xFF);
	} while ((res != 0xFF) && sdTimer2);
	
	return res;
}
void disk_timer_proc(void)
{
	static WORD pv;	// present state variable
	WORD p;
	BYTE s;
	UINT n;
	
	// decrement the local timer counters
	n = sdTimer1;
	if (n)
		sdTimer1--;
		
	n = sdTimer2;
	if (n)
		sdTimer2--;
		
	p = pv;
	pv = (SD_CardDetect() << 1) | SD_WriteProtect();  // check the input pins
	
	if (p == pv){
		s = Stat;
		
		if (p & BIT_0)
			s |= STA_PROTECT;
		else
			s &= ~STA_PROTECT;
			
		if (p & BIT_1)
			s |= (STA_NODISK | STA_NOINIT);
		else
			s &= ~ STA_NODISK;
			
		Stat = s;
	}
}

