#include <wx/wx.h>
#include <stdio.h>
#include <signal.h>

#include "UsbDevice.h"
#include "UsbConfiguration.h"
#include "UsbInterface.h"
#include "UsbEndpoint.h"
#include "UsbIpServer.h"

class Test : public wxAppConsole {
    bool OnInit();
    int OnRun();
};

wxIMPLEMENT_APP(Test);

bool Test::OnInit() {
    if (!wxAppConsole::OnInit()) {
	return false;
    }
    wxPrintf("Hello World\n");
    return true;
}

int Test::OnRun() {
    UsbIpServer test;
    if (test.Init()) {
	wxPrintf("Init OK\n");
    } else {
	return false;
    }

    UsbEndpoint ep1(0x81, 2, 64, 10);
    UsbEndpoint* epList0[1] = { &ep1 };
    UsbInterface iface0(0, 0, 0xff, 1, 2, 0, 1, epList0);

    UsbEndpoint ep2(0x82, 2, 64, 10);
    UsbEndpoint* epList1[1] = { &ep2 };
    UsbInterface iface1(1, 0, 0xff, 3, 4, 0, 1, epList1);
    UsbInterface* interfaceList[2] = { &iface0, &iface1 };
    UsbConfiguration config1(1, 0, 0xc0, 100, 2, interfaceList);
    UsbConfiguration* configurationList[1] = { &config1 }; 

    // unsigned char buffer[512];
    // memset(buffer, 0, 512);
    // int size = config1.GenerateDescriptor(buffer, 0);

    // wxPrintf("Size = %d\n", size);
    // for (int idx = 0; idx < size; idx++) {
    // 	wxPrintf("%.2x, ", buffer[idx]);
    // 	if ((idx % 16) == 7) wxPrintf("  ");
    // 	if ((idx % 16) == 15) wxPrintf("\n");
    // }
    // wxPrintf("\n");
    
    UsbDevice dev(0x00fa, 0xc001, 0x1234, 0xff, 0, 0, 1, configurationList);
    
    test.AddDevice(&dev, "My First Virtual Device", "1-1", 2, 3, USB_SPEED_HIGH);
    bool res = test.StartServer();
    wxPrintf("Res = %d\n", int(res));

    wxSleep(10);
    
    test.StopServer();

    return false;
}
