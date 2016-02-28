/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    USBDESC.C
 *      Purpose: USB Descriptors
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This file is part of the uVision/ARM development tools.
 *      This software may only be used under the terms of a valid, current,
 *      end user licence from KEIL for a compatible version of KEIL software
 *      development tools. Nothing else gives you the right to use it.
 *
 *      Copyright (c) 2005-2007 Keil Software.
 *---------------------------------------------------------------------------*/

#include "type.h"

#include "usb.h"
#include "hid.h"
#include "usbcfg.h"
#include "usbdesc.h"


/* HID Report Descriptor */
const BYTE HID_ReportDescriptor[] = {
  HID_UsagePageVendor(0x00),
  HID_Usage(0x01),
  HID_Collection(HID_Application),
   // HID_UsagePage(HID_USAGE_PAGE_BUTTON),
    HID_UsageMin(1),
    HID_UsageMax(0x40),
    HID_LogicalMin(0),
    HID_LogicalMax(0x40),
    HID_ReportCount(0x40),
    HID_ReportSize(0x08),
    HID_Input(HID_Data),
   // HID_ReportCount(1),
   // HID_ReportSize(6),
    //HID_Input(HID_Constant),
  //  HID_UsagePage(HID_USAGE_PAGE_LED),
  //  HID_Usage(HID_USAGE_LED_GENERIC_INDICATOR),
    HID_UsageMin(0),
    HID_UsageMax(0x40),
   // HID_ReportCount(8),
    //HID_ReportSize(1),
    HID_Output(HID_Data),
  HID_EndCollection,
};

const WORD HID_ReportDescSize = sizeof(HID_ReportDescriptor);


/* USB Standard Device Descriptor */
const BYTE USB_DeviceDescriptor[] = {
  USB_DEVICE_DESC_SIZE,              /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0200), /* 2.20 */          /* bcdUSB */
  0x00,                              /* bDeviceClass */
  0x00,                              /* bDeviceSubClass */
  0x00,                              /* bDeviceProtocol */
  USB_MAX_PACKET0,                   /* bMaxPacketSize0 */
  WBVAL(0xC251),                     /* idVendor */
  WBVAL(0x2201),                     /* idProduct */
  WBVAL(0x0200), /* 2.00 */          /* bcdDevice */
  0x04,                              /* iManufacturer */
  0x20,                              /* iProduct */
  0x44,                              /* iSerialNumber */
  0x01                               /* bNumConfigurations */
};

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const BYTE USB_ConfigDescriptor[] = {
/* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(                             /* wTotalLength */
    USB_CONFIGUARTION_DESC_SIZE +
    USB_INTERFACE_DESC_SIZE     +
    HID_DESC_SIZE               +
    USB_ENDPOINT_DESC_SIZE			+
		USB_ENDPOINT_DESC_SIZE
  ),
  0x01,                              /* bNumInterfaces */
  0x01,                              /* bConfigurationValue */
  0x00,                              /* iConfiguration */
  USB_CONFIG_POWERED_MASK /*|*/       /* bmAttributes */
/*USB_CONFIG_REMOTE_WAKEUP*/,
  USB_CONFIG_POWER_MA(500),          /* bMaxPower */
/* Interface 0, Alternate Setting 0, HID Class */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  USB_HID_IF_NUM,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x02,                              /* bNumEndpoints */
  USB_DEVICE_CLASS_HUMAN_INTERFACE,  /* bInterfaceClass */
  HID_SUBCLASS_NONE,                 /* bInterfaceSubClass */
  HID_PROTOCOL_NONE,                 /* bInterfaceProtocol */
  0,                              /* iInterface */
/* HID Class Descriptor */
/* HID_DESC_OFFSET = 0x0012 */
  HID_DESC_SIZE,                     /* bLength */
  HID_HID_DESCRIPTOR_TYPE,           /* bDescriptorType */
  WBVAL(0x0111), /* 1.00 */          /* bcdHID */
  0x00,                              /* bCountryCode */
  0x01,                              /* bNumDescriptors */
  HID_REPORT_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(HID_REPORT_DESC_SIZE),       /* wDescriptorLength */
/* Endpoint, HID Interrupt In */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(1),                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_INTERRUPT,       /* bmAttributes */
  WBVAL(0x0040),                     /* wMaxPacketSize */
  0x01,          /* 32ms */          /* bInterval */
/* Terminator */
/* Endpoint, HID Interrupt OUT */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_OUT(1),                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_INTERRUPT,       /* bmAttributes */
  WBVAL(0x0040),                     /* wMaxPacketSize */
  0x01,          /* 32ms */          /* bInterval */
/* Terminator */
  0                                  /* bLength */
};

/* USB String Descriptor (optional) */
const BYTE USB_StringDescriptor[] = {
/* Index 0x00: LANGID Codes */
  0x04,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0409), /* US English */    /* wLANGID */
/* Index 0x01: Manufacturer */
  (9*2 + 2),                        /* bLength (13 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'A',0,
  'M',0,
  'B',0,
  'O',0,
  '-',0,
  'T',0,
  'E',0,
  'C',0,
  'H',0,
/* Index 0x02: Product */
  (12*2 + 2),                        /* bLength ( 17 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'A',0,
  'M',0,
  'B',0,
  'O',0,
  '-',0,
  'G',0,
  'T',0,
  '4',0,
  '0',0,
  '0',0,
  'V',0,
  'N',0,
/* Index 0x03: Serial Number */
  (1*2 + 2),                        /* bLength (12 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  '0',0,
/* Index 0x5E: Interface 0, Alternate Setting 0 */
  (3*2 + 2),                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'H',0,
  'I',0,
  'D',0,
};
