// KDC101_CPP_6_30.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <stdlib.h>
#include <conio.h>

#if defined TestCode
#include "..\..\..\Instruments\ThorLabs.TCube.DCServo\ThorLabs.KCube.DCServo\Thorlabs.MotionControl.KCube.DCServo.h"
#else
#include "Thorlabs.MotionControl.KCube.DCServo.h"
#endif


using namespace std;

	int __cdecl wmain(int argc, wchar_t* argv[])
	{
		if (argc < 1)
		{
			printf("Usage = Example_KDC101 [serial_no] [position: optional (0 - 1715200)] [velocity: optional (0 - 3838091)]\r\n");
			char c = _getch();
			return 1;
		}

		int serialNo = 27500576;
		if (argc > 1)
		{
			serialNo = _wtoi(argv[1]);
		}

		// get parameters from command line
		int position = 0;
		if (argc > 2)
		{
			position = _wtoi(argv[2]);
		}

		int velocity = 0;
		if (argc > 3)
		{
			velocity = _wtoi(argv[3]);
		}

		// identify and access device
		char testSerialNo[16];
		sprintf_s(testSerialNo, "%d", serialNo);

		// Build list of connected device
		if (TLI_BuildDeviceList() == 0)
		{
			// get device list size 
			short n = TLI_GetDeviceListSize();
			// get TDC serial numbers
			char serialNos[100];
			TLI_GetDeviceListByTypeExt(serialNos, 100, 27);

			// output list of matching devices
			{
				char *searchContext = nullptr;
				char *p = strtok_s(serialNos, ",", &searchContext);

				while (p != nullptr)
				{
					TLI_DeviceInfo deviceInfo;
					// get device info from device
					TLI_GetDeviceInfo(p, &deviceInfo);
					// get strings from device info structure
					char desc[65];
					strncpy_s(desc, deviceInfo.description, 64);
					desc[64] = '\0';
					char serialNo[9];
					strncpy_s(serialNo, deviceInfo.serialNo, 8);
					serialNo[8] = '\0';
					// output
					printf("Found Device %s=%s : %s\r\n", p, serialNo, desc);
					p = strtok_s(nullptr, ",", &searchContext);
				}
			}

			// open device
			if (CC_Open(testSerialNo) == 0)
			{
				// start the device polling at 200ms intervals
				CC_StartPolling(testSerialNo, 200);

				Sleep(3000);
				// Home device
				CC_ClearMessageQueue(testSerialNo);
				CC_Home(testSerialNo);
				printf("Device %s homing\r\n", testSerialNo);

				// wait for completion
				WORD messageType;
				WORD messageId;
				DWORD messageData;
				CC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
				while (messageType != 2 || messageId != 0)
				{
					CC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
				}

				// set velocity if desired
				if (velocity > 0)
				{
					int currentVelocity, currentAcceleration;
					CC_GetVelParams(testSerialNo, &currentAcceleration, &currentVelocity);
					CC_SetVelParams(testSerialNo, currentAcceleration, velocity);
				}

				// move to position (channel 1)
				CC_ClearMessageQueue(testSerialNo);
				CC_MoveToPosition(testSerialNo, position);
				printf("Device %s moving\r\n", testSerialNo);

				// wait for completion
				CC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
				while (messageType != 2 || messageId != 1)
				{
					CC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
				}

				// get actual poaition
				int pos = CC_GetPosition(testSerialNo);
				printf("Device %s moved to %d\r\n", testSerialNo, pos);

				// stop polling
				CC_StopPolling(testSerialNo);
				// close device
				CC_Close(testSerialNo);
			}
		}

		char c = _getch();
		return 0;
	}

	


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
