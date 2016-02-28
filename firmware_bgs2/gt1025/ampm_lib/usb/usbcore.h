/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    USBCORE.H
 *      Purpose: USB Core Definitions
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#ifndef __USBCORE_H__
#define __USBCORE_H__
#include <stdint.h>
#include "usb.h"
#include "usbcfg.h"
/* USB Endpoint Data Structure */
typedef struct _USB_EP_DATA {
  uint8_t  *pData;
  uint16_t   Count;
} USB_EP_DATA;

/* USB Core Global Variables */
extern uint16_t  USB_DeviceStatus;
extern uint8_t  USB_DeviceAddress;
extern uint8_t  USB_Configuration;
extern uint32_t USB_Connected;
extern uint32_t USB_EndPointMask;
extern uint32_t USB_EndPointHalt;
extern uint32_t USB_EndPointStall;
extern uint8_t  USB_AltSetting[USB_IF_NUM];

/* USB Endpoint 0 Buffer */
extern uint8_t  EP0Buf[USB_MAX_PACKET0];

/* USB Endpoint 0 Data Info */
extern USB_EP_DATA EP0Data;

/* USB Setup Packet */
extern USB_SETUP_PACKET SetupPacket;

/* USB Core Functions */
extern void  USB_ResetCore (void);


#endif  /* __USBCORE_H__ */
