/*
             LUFA Library
     Copyright (C) Dean Camera, 2014.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2014  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#include "Descriptors.h"

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
    .Header = {
        .Size = sizeof (USB_Descriptor_Device_t),
        .Type = DTYPE_Device
    },

    .USBSpecification = VERSION_BCD(1,1,0),
    .Class = USB_CSCP_NoDeviceClass,
    .SubClass = USB_CSCP_NoDeviceSubclass,
    .Protocol = USB_CSCP_NoDeviceProtocol,

    .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,

    .VendorID = 0x03EB,
    .ProductID = 0x2040,
    .ReleaseNumber = VERSION_BCD(0,0,1),

    .ManufacturerStrIndex = STRING_ID_Manufacturer,
    .ProductStrIndex = STRING_ID_Product,
    .SerialNumStrIndex = STRING_ID_Serial,

    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
    .Config = {
        .Header = {
            .Size = sizeof (USB_Descriptor_Configuration_Header_t),
            .Type = DTYPE_Configuration
        },

        .TotalConfigurationSize = sizeof (USB_Descriptor_Configuration_t),
        .TotalInterfaces = 1,

        .ConfigurationNumber = 1,
        .ConfigurationStrIndex = NO_DESCRIPTOR,

        .ConfigAttributes = USB_CONFIG_ATTR_RESERVED,

        .MaxPowerConsumption = USB_CONFIG_POWER_MA(500),
    },

    .Vendor_Interface = {
        .Header = {
            .Size = sizeof (USB_Descriptor_Interface_t),
            .Type = DTYPE_Interface
        },

        .InterfaceNumber = 0,
        .AlternateSetting = 0,

        .TotalEndpoints = 1,

        .Class = 0xFF,
        .SubClass = 0xFF,
        .Protocol = 0xFF,

        .InterfaceStrIndex = NO_DESCRIPTOR,
    },

    .Vendor_DataOutEndpoint = {
        .Header = {
            .Size = sizeof (USB_Descriptor_Endpoint_t),
            .Type = DTYPE_Endpoint
        },

        .EndpointAddress = 0x01,
        .Attributes = EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        .EndpointSize = 8,
        .PollingIntervalMS = 0x01
    }
};

const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"Antumbra");
const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(L"Glow test");
const USB_Descriptor_String_t PROGMEM SerialString = USB_STRING_DESCRIPTOR(L"Fantastic Edition");

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void **DescriptorAddress)
{
    const uint8_t DescriptorType = wValue >> 8;
    const uint8_t DescriptorNumber = wValue & 0xFF;

    const void *Address = NULL;
    uint16_t Size = NO_DESCRIPTOR;

    switch (DescriptorType) {
    case DTYPE_Device:
        Address = &DeviceDescriptor;
        Size = sizeof (USB_Descriptor_Device_t);
        break;
    case DTYPE_Configuration:
        Address = &ConfigurationDescriptor;
        Size = sizeof (USB_Descriptor_Configuration_t);
        break;
    case DTYPE_String:
        switch (DescriptorNumber) {
        case STRING_ID_Language:
            Address = &LanguageString;
            Size = pgm_read_byte(&LanguageString.Header.Size);
            break;
        case STRING_ID_Manufacturer:
            Address = &ManufacturerString;
            Size = pgm_read_byte(&ManufacturerString.Header.Size);
            break;
        case STRING_ID_Product:
            Address = &ProductString;
            Size = pgm_read_byte(&ProductString.Header.Size);
            break;
        case STRING_ID_Serial:
            Address = &SerialString;
            Size = pgm_read_byte(&SerialString.Header.Size);
            break;
        }

        break;
    }

    *DescriptorAddress = Address;
    return Size;
}
