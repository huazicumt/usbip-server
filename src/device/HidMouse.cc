/*******************************************************
 usbip-server - a platform for USB device prototyping

 Fredrik Andersson
 Copyright 2016, All Rights Reserved.

 This software may be used by anyone for any reason so
 long as the copyright notice in the source files
 remains intact.

 code repository located at:
	http://github.com/freand76/usbip-server
********************************************************/

#include "HidMouse.h"
#include "UsbUtil.h"

using namespace UsbUtil;

/* Debug */
#include "Verbose.h"
using namespace Verbose;

#define HID_MOUSE_REPORT_IDX 34

int HidMouseInterface::GenerateConfigurationDescriptor(uint8_t* buffer, int offset) {
    int pos = offset;
    pos += UsbInterface::GenerateConfigurationDescriptor(buffer, pos);
    int rSize = GenerateHIDReportDescriptor(NULL, 0);

    pos += SetUint(9,                    buffer, pos, 1);
    pos += SetUint(0x21,                 buffer, pos, 1);
    pos += SetUint(0x110,                buffer, pos, 2);
    pos += SetUint(0,                    buffer, pos, 1);
    pos += SetUint(1,                    buffer, pos, 1);
    pos += SetUint(HID_MOUSE_REPORT_IDX, buffer, pos, 1);
    pos += SetUint(rSize,                buffer, pos, 2);

    return pos - offset;
}

static uint8_t reportDescriptorHID[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xc0,                          //   END_COLLECTION
    0xc0
};

int HidMouseInterface::GenerateHIDReportDescriptor(uint8_t* buffer, int offset) {
    int pos = offset;
    for (unsigned int idx = 0; idx < sizeof(reportDescriptorHID); idx++) {
	pos += SetUint(reportDescriptorHID[idx], buffer, pos, 1);
    }
    return pos-offset;
}

int HidMouseInterface::GetDescriptor(uint8_t* setup, uint8_t* data, uint8_t* replyBuffer, int bufLength) {
    uint8_t bDescriptorType = setup[3];

    if (bDescriptorType == HID_MOUSE_REPORT_IDX) {
	return GenerateHIDReportDescriptor(replyBuffer, 0);
    }

    return UsbInterface::GetDescriptor(setup, data, replyBuffer, bufLength);
}

int HidMouseEndpoint::Data(uint8_t* inData, uint8_t* outBuffer, int length) {
    (void)inData;
    (void)length;

    outBuffer[0] = event.but;
    outBuffer[1] = event.x;
    outBuffer[2] = event.y;

    event.but = 0;
    event.x = 0;
    event.y = 0;
    event.free = true;

    return 3;
}

bool HidMouse::Move(int but, int x, int y) {
    if (endpoint.event.free) {
	endpoint.event.but = but;
	endpoint.event.x = x;
	endpoint.event.y = y;
	endpoint.event.free = false;
	return true;
    }
    return false;
}
