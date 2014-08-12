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

#define  INCLUDE_FROM_BULKVENDOR_C
#include "BulkVendor.h"

#include "../led.h"

int main(void)
{
    cli();

	USB_Init();

    led_init();
    led_set_rgb(0, 1, 0);

	sei();

    uint8_t indata[8];
    memset(indata, 0, sizeof indata);

	for (;;) {
		USB_USBTask();

		Endpoint_SelectEndpoint(0x01);
		if (Endpoint_IsOUTReceived()) {
            Endpoint_Read_Stream_LE(indata, sizeof indata, NULL);
			Endpoint_ClearOUT();

            led_set_rgb(indata[0], indata[1], indata[2]);
        }
	}
}

void EVENT_USB_Device_Connect(void)
{
}

void EVENT_USB_Device_Disconnect(void)
{
}

void EVENT_USB_Device_ConfigurationChanged(void)
{
	Endpoint_ConfigureEndpoint(0x01, EP_TYPE_BULK, 8, 1);
}

void EVENT_USB_Device_ControlRequest(void)
{
}
