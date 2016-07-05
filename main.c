#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// Simplelink includes
#include "simplelink.h"

//Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "common.h"
//Common interface includes
#include "gpio_if.h"
#include "common.h"
#ifndef NOTERM
#include "uart_if.h"
#endif
#include "pinmux.h"
#include "button_if.h"
#define APPLICATION_NAME        "Hw4_2"
#define APPLICATION_VERSION     "1.1.1"

char* DeviceFileName = "Hw4.txt";
unsigned long MaxSize = 63 * 1024; //62.5K is max file size
long DeviceFileHandle = -1;
long RetVal;        //negative retval is an error
unsigned long Offset = 0;
unsigned char InputBuffer[100];
char *oldValue = "";
char *newValue = "000";
/* Application specific status/error codes */
typedef enum {
	// Choosing this number to avoid overlap w/ host-driver's error codes
	FILE_ALREADY_EXIST = -0x7D0,
	FILE_CLOSE_ERROR = FILE_ALREADY_EXIST - 1,
	FILE_NOT_MATCHED = FILE_CLOSE_ERROR - 1,
	FILE_OPEN_READ_FAILED = FILE_NOT_MATCHED - 1,
	FILE_OPEN_WRITE_FAILED = FILE_OPEN_READ_FAILED - 1,
	FILE_READ_FAILED = FILE_OPEN_WRITE_FAILED - 1,
	FILE_WRITE_FAILED = FILE_READ_FAILED - 1,

	STATUS_CODE_MAX = -0xBB8
} e_AppStatusCodes;

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************

#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- Start
//*****************************************************************************

//*****************************************************************************
//
//! This function gets triggered when HTTP Server receives Application
//! defined GET and POST HTTP Tokens.
//!
//! \param pHttpServerEvent Pointer indicating http server event
//! \param pHttpServerResponse Pointer indicating http server response
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pSlHttpServerEvent,
		SlHttpServerResponse_t *pSlHttpServerResponse) {

}

//*****************************************************************************
//
//!  \brief This function handles WLAN events
//!
//!  \param[in] pSlWlanEvent is the event passed to the handler
//!
//!  \return None
//
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pSlWlanEvent) {

}

//*****************************************************************************
//
//!  \brief This function handles events for IP address 
//!           acquisition via DHCP indication
//!
//!  \param[in] pNetAppEvent is the event passed to the handler
//!
//!  \return None
//
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent) {

}

//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]     pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent) {

}

//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock) {

}

//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End
//*****************************************************************************

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void BoardInit(void) {
	// In case of TI-RTOS vector table is initialize by OS itself
#ifndef USE_TIRTOS
	//
	// Set vector table base
	//
#if defined(ccs)
	MAP_IntVTableBaseSet((unsigned long) &g_pfnVectors[0]);
#endif
#if defined(ewarm)
	MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
	//
	// Enable Processor
	//
	MAP_IntMasterEnable();
	MAP_IntEnable(FAULT_SYSTICK);

	PRCMCC3200MCUInit();
}

void hw2() {

	if (GPIO_IF_LedStatus(MCU_GREEN_LED_GPIO) == 0) {
		GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
		strcpy(newValue, "100");
		printf("GREEN ON");
	} else if (GPIO_IF_LedStatus(MCU_ORANGE_LED_GPIO) == 0) {
		GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
		strcpy(newValue, "110");
		printf("ORANGE ON");
	} else if (GPIO_IF_LedStatus(MCU_RED_LED_GPIO) == 0) {
		GPIO_IF_LedOn(MCU_RED_LED_GPIO);
		strcpy(newValue, "111");
		printf("RED ON");
	} else {
		printf("ALL LED ON");
	}
	UtilsDelay(3000000);
	Button_IF_EnableInterrupt(SW2);
}

void pushButtonInterruptHandler3() {
	if (GPIO_IF_LedStatus(MCU_RED_LED_GPIO) == 1) {
		GPIO_IF_LedOff(MCU_RED_LED_GPIO);
		strcpy(newValue, "110");
		printf("RED OFF");
	} else if (GPIO_IF_LedStatus(MCU_ORANGE_LED_GPIO) == 1) {
		GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
		strcpy(newValue, "100");
		printf("ORANGE OFF");
	} else if (GPIO_IF_LedStatus(MCU_GREEN_LED_GPIO) == 1) {
		GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
		strcpy(newValue, "000");
		printf("GREEN OFF");
	} else {
		printf("ALL OFF");
	}
	UtilsDelay(3000000);
	Button_IF_EnableInterrupt(SW3);
}

void main() {
	long lRetVal;
	unsigned char policyVal;

	//
	// Initialize Board configurations
	//
	BoardInit();

	//
	// Configure the pinmux settings for the peripherals exercised
	//
	PinMuxConfig();

	lRetVal = sl_Start(NULL, NULL, NULL);
	if (lRetVal < 0) {
		GPIO_IF_LedOn(MCU_RED_LED_GPIO);
		LOOP_FOREVER()
		;
	}

	//
	// reset all network policies
	//
	lRetVal = sl_WlanPolicySet( SL_POLICY_CONNECTION,
			SL_CONNECTION_POLICY(0, 0, 0, 0, 0), &policyVal,
			1 /*PolicyValLen*/);
	if (lRetVal < 0) {
		GPIO_IF_LedOn(MCU_RED_LED_GPIO);
		LOOP_FOREVER()
		;
	}

	SlFsFileInfo_t FsFileInfo;
	RetVal = sl_FsGetInfo("Hw4.txt",0,&FsFileInfo);

	//create file if it is not there and close it..
	    //
	    //  create a user file
	    //
	if(RetVal<0){

	RetVal= sl_FsOpen((unsigned char *)DeviceFileName,
	                                 FS_MODE_OPEN_CREATE(MaxSize , _FS_FILE_OPEN_FLAG_NO_SIGNATURE_TEST | _FS_FILE_OPEN_FLAG_COMMIT ),
	                                 NULL, &DeviceFileHandle);

	 if(RetVal < 0)
	    {
	        //
	        // File may already be created
	        //
	        RetVal = sl_FsClose(DeviceFileHandle, 0, 0, 0);
	        ASSERT_ON_ERROR(lRetVal);
	    }
	    else
	    {
	        //
	        // close the user file
	        //
	        RetVal = sl_FsClose(DeviceFileHandle, 0, 0, 0);
	        if (SL_RET_CODE_OK != lRetVal)
	        {
	            ASSERT_ON_ERROR(FILE_CLOSE_ERROR);
	        }
	    }
	}

	GPIO_IF_LedConfigure(LED1 | LED2 | LED3);

	//set the old LED status..
	 RetVal = sl_FsOpen((unsigned char *)DeviceFileName,
	                        FS_MODE_OPEN_READ,
	                        NULL,
	                        &DeviceFileHandle);
	    if(RetVal < 0)
	    {
	        RetVal = sl_FsClose(DeviceFileHandle, 0, 0, 0);
	        //ASSERT_ON_ERROR(FILE_OPEN_WRITE_FAILED);
	    }


	RetVal = sl_FsRead(DeviceFileHandle, Offset, (unsigned char *)InputBuffer,
			strlen("000"));


    if(InputBuffer[0]=='1'){
    	GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
    }else
    {
    	GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
    }

    if(InputBuffer[1]=='1'){
        	GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
        }else
        {
        	GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
        }

    if(InputBuffer[2]=='1'){
        	GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        }else
        {
        	GPIO_IF_LedOff(MCU_RED_LED_GPIO);
        }

    sl_FsClose(DeviceFileHandle, 0, 0, 0);

	Button_IF_Init(hw2, pushButtonInterruptHandler3);


	while (1) {
		if (strcmp(oldValue, newValue) != 0) {
			RetVal = sl_FsOpen((unsigned char *)DeviceFileName,
					                        FS_MODE_OPEN_WRITE,
					                        NULL,
					                        &DeviceFileHandle);

			RetVal= sl_FsWrite(DeviceFileHandle, Offset, (unsigned char *) newValue,
					strlen(newValue));

			RetVal= sl_FsClose(DeviceFileHandle, 0, 0, 0);
			strcpy(oldValue, newValue);
			printf("New value updated in file\n");
			fflush(stdout);
		} else {
			printf("No button is fired\n");
			fflush(stdout);
		}
	}
}
