#include <avr/pgmspace.h>

#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

struct cfgdescriptor {
    USB_Descriptor_Configuration_Header_t cfg;
    USB_Descriptor_Interface_t intf;

    USB_Descriptor_Endpoint_t cfoutep;
    USB_Descriptor_Endpoint_t cfinep;
    USB_Descriptor_Endpoint_t ledoutep;
};

const USB_Descriptor_Device_t PROGMEM devdes = {
    .Header = {
        .Size = sizeof (USB_Descriptor_Device_t),
        .Type = DTYPE_Device,
    },
    .USBSpecification = VERSION_BCD(1, 1, 0),
    .Class = USB_CSCP_NoDeviceClass,
    .SubClass = USB_CSCP_NoDeviceSubclass,
    .Protocol = USB_CSCP_NoDeviceProtocol,
    .Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,
    .VendorID = 0x03EB,
    .ProductID = 0x2040,
    .ReleaseNumber = VERSION_BCD(0, 0, 1),
    .ManufacturerStrIndex = 1,
    .ProductStrIndex = 2,
    .SerialNumStrIndex = 3,
    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS,
};

const struct cfgdescriptor PROGMEM cfgdes = {
    .cfg = {
        .Header = {
            .Size = sizeof (USB_Descriptor_Configuration_Header_t),
            .Type = DTYPE_Configuration,
        },
        .TotalConfigurationSize = sizeof (struct cfgdescriptor),
        .TotalInterfaces = 1,
        .ConfigurationNumber = 1,
        .ConfigurationStrIndex = NO_DESCRIPTOR,
        .ConfigAttributes = USB_CONFIG_ATTR_RESERVED,
        .MaxPowerConsumption = USB_CONFIG_POWER_MA(500),
    },
    .intf = {
        .Header = {
            .Size = sizeof (USB_Descriptor_Interface_t),
            .Type = DTYPE_Interface,
        },
        .InterfaceNumber = 0,
        .AlternateSetting = 0,
        .TotalEndpoints = 2,
        .Class = 0xff,
        .SubClass = 0xff,
        .Protocol = 0xff,
        .InterfaceStrIndex = 4,
    },
    .cfoutep = {
        .Header = {
            .Size = sizeof (USB_Descriptor_Endpoint_t),
            .Type = DTYPE_Endpoint,
        },
        .EndpointAddress = 0x01,
        .Attributes = EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        .EndpointSize = 64,
        .PollingIntervalMS = 0x05,
    },
    .cfinep = {
        .Header = {
            .Size = sizeof (USB_Descriptor_Endpoint_t),
            .Type = DTYPE_Endpoint,
        },
        .EndpointAddress = 0x82,
        .Attributes = EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        .EndpointSize = 64,
        .PollingIntervalMS = 0x05,
    },
    .ledoutep = {
        .Header = {
            .Size = sizeof (USB_Descriptor_Endpoint_t),
            .Type = DTYPE_Endpoint,
        },
        .EndpointAddress = 0x03,
        .Attributes = EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        .EndpointSize = 8,
        .PollingIntervalMS = 0x05,
    },
};

const USB_Descriptor_String_t PROGMEM langstr = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
const USB_Descriptor_String_t PROGMEM manustr = USB_STRING_DESCRIPTOR(L"Antumbra");
const USB_Descriptor_String_t PROGMEM prodstr = USB_STRING_DESCRIPTOR(L"Glow test");
const USB_Descriptor_String_t PROGMEM seristr = USB_STRING_DESCRIPTOR(L"Superlative Edition");
const USB_Descriptor_String_t PROGMEM magic = USB_STRING_DESCRIPTOR(
    L"io.antumbra.glowapi/01/82/io.antumbra.glow.v3"
);

uint16_t CALLBACK_USB_GetDescriptor(uint16_t val, uint8_t idx,
                                    const void **addr)
    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

uint16_t CALLBACK_USB_GetDescriptor(uint16_t val, uint8_t idx,
                                    const void **addr)
{
    uint8_t destype = val >> 8, desnum = val & 0xff;

    if (destype == DTYPE_Device) {
        *addr = &devdes;
        return sizeof devdes;
    }
    else if (destype == DTYPE_Configuration) {
        *addr = &cfgdes;
        return sizeof cfgdes;
    }
    else if (destype == DTYPE_String) {
        switch (desnum) {
        case 0:
            *addr = &langstr;
            return pgm_read_byte(&langstr.Header.Size);
        case 1:
            *addr = &manustr;
            return pgm_read_byte(&manustr.Header.Size);
        case 2:
            *addr = &prodstr;
            return pgm_read_byte(&prodstr.Header.Size);
        case 3:
            *addr = &seristr;
            return pgm_read_byte(&seristr.Header.Size);
        case 4:
            *addr = &magic;
            return pgm_read_byte(&magic.Header.Size);
        }
    }

    *addr = NULL;
    return NO_DESCRIPTOR;
}

void EVENT_USB_Device_Connect(void) {}
void EVENT_USB_Device_Disconnect(void) {}
void EVENT_USB_Device_ConfigurationChanged(void)
{
	Endpoint_ConfigureEndpoint(0x01, EP_TYPE_BULK, 64, 1);
	Endpoint_ConfigureEndpoint(0x82, EP_TYPE_BULK, 64, 1);
}
void EVENT_USB_Device_ControlRequest(void) {}

void rawusb_init(void)
{
    USB_Init();
}

void rawusb_tick(void)
{
    USB_USBTask();
}

bool rawusb_recv_bulk(uint8_t ep, void *out, uint16_t sz)
{
    Endpoint_SelectEndpoint(ep);
    if (Endpoint_IsOUTReceived()) {
        if (Endpoint_Read_Stream_LE(out, sz, NULL) != ENDPOINT_RWSTREAM_NoError)
            return false;
        Endpoint_ClearOUT();
        return true;
    }
    return false;
}

bool rawusb_send_bulk(uint8_t ep, const void *in, uint16_t sz)
{
    Endpoint_SelectEndpoint(ep);
    if (Endpoint_IsINReady()) {
        if (Endpoint_Write_Stream_LE(in, sz, NULL) != ENDPOINT_RWSTREAM_NoError)
            return false;
        Endpoint_ClearIN();
        return true;
    }
    return false;
}
