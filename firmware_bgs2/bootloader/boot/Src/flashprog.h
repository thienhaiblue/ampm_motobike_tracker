#ifndef __FLASH_PROG_H__
#define __FLASH_PROG_H__
//======================================================

void myFMC_Erase(uint32_t u32addr);
void FMC_ProgramPage(uint32_t u32startAddr, uint32_t * u32buff);
void myFMC_Write(uint32_t u32addr, uint32_t u32data);
#endif
