/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    USBHW.H
 *      Purpose: USB Hardware Layer Definitions
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#ifndef __USBHW_H__
#define __USBHW_H__


/* USB Hardware Functions */
extern void  USB_Init       (void);
extern void  USB_Connect    (uint32_t  con);
extern void  USB_Reset      (void);
extern void  USB_Suspend    (void);
extern void  USB_Resume     (void);
extern void  USB_WakeUp     (void);
extern void  USB_WakeUpCfg  (uint32_t  cfg);
extern void  USB_SetAddress (uint32_t adr);
extern void  USB_Configure  (uint32_t cfg);
extern void  USB_ConfigEP   (USB_ENDPOINT_DESCRIPTOR *pEPD);
extern void  USB_DirCtrlEP  (uint32_t dir);
extern void  USB_EnableEP   (uint32_t EPNum);
extern void  USB_DisableEP  (uint32_t EPNum);
extern void  USB_ResetEP    (uint32_t EPNum);
extern void  USB_SetStallEP (uint32_t EPNum);
extern void  USB_ClrStallEP (uint32_t EPNum);
extern uint32_t USB_ReadEP     (uint32_t EPNum, uint8_t *pData);
extern uint32_t USB_WriteEP    (uint32_t EPNum, uint8_t *pData, uint32_t cnt);
extern uint32_t USB_GetFrame   (void);


#endif  /* __USBHW_H__ */
