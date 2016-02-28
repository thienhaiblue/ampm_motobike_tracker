#ifndef _SST25_H_
#define _SST25_H_
#include <stdint.h>

#define SECTOR_SIZE	4096

#define FLASH_SIZE_MAX	(1*1024*1024)//1Mbytes
#define FIRMWARE_MAX_SIZE		0x00020000

#define BASE_FIRMWARE_INFO_ADDR 			0
#define BASE_FIRMWARE_BASE_ADDR			(0x00001000 + BASE_FIRMWARE_INFO_ADDR)

#define FIRMWARE_INFO_ADDR 								(BASE_FIRMWARE_BASE_ADDR + FIRMWARE_MAX_SIZE)
#define FIRMWARE_BASE_ADDR								(0x00001000 + FIRMWARE_INFO_ADDR)


#define FIRMWARE_STATUS_OFFSET			(1)
#define FIRMWARE_FILE_SIZE_OFFSET		(2)
#define FIRMWARE_CRC_OFFSET					(3)
#define FIRMWARE_START_ADDR_OFFSET	(4)
#define FIRMWARE_ERR_ADDR_OFFSET		(5)


#define FIRMWARE_FILE_SIZE_ERR		0xAAAAAAAA
#define FIRMWARE_START_ADDR_ERR		0xBBBBBBBB
#define FIRMWARE_END_ADDR_ERR			0xCCCCCCCC
#define FIRMWARE_CRC_ERR					0xDDDDDDDD


enum SST25_ERASE_MODE {
    all,
    block4k,
    block32k,
    block64k
};

uint8_t SST25_Init(void);
uint8_t SST25_Erase(uint32_t addr, enum SST25_ERASE_MODE mode);
uint8_t SST25_Read(uint32_t addr, uint8_t *buf, uint32_t count);
uint8_t SST25_Write(uint32_t addr, const uint8_t *buf, uint32_t count);
void SPItest1(void);
#endif
