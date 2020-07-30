/*
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== sockets.c ========
 */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* SimpleLink Wi-Fi Host Driver Header files */
#include <simplelink.h>
#include <osi.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/WiFi.h>

/* Example/Board Header file */
#include "Board.h"
#include "sockets.h"

#include "simplelink.h"
extern char wireless_mode;// 0=Local, 1=Internet
extern int smartphone_access;
extern char wifi_name[20];// Store the device SSID for local mode
extern char wifi_pass[20];// Store the device password for local mode
//Internet Mode Parameters
extern char ext_apn[20];// Store the SSID of the external device to be connected to
extern char ext_pass[20];// Store the password of the external device to be connected to

extern char portal_ip[4];
extern char portal_name[40];

//#include "sl_common.h"

#ifdef MSP430WARE
#define SPI_BIT_RATE    2000000
#else
#define SPI_BIT_RATE    14000000
#endif

#define APPLICATION_VERSION "1.1.0"

#define SL_STOP_TIMEOUT        0xFF

/**/
#define LOOP_FOREVER() \
            {\
                while(1); \
            }

#define ASSERT_ON_ERROR(error_code) \
            {\
                /* Handling the error-codes is specifc to the application */ \
                if (error_code < 0) return error_code; \
                /* else, continue w/ execution */ \
            }

#define HOST_NAME       "www.avkon.in"

#define SUCCESS         0
#define MAX_SSID_LEN    32
#define MAX_PASSKEY_LEN 32
#define BAUD_RATE       115200

#define PING_INTERVAL        1000
#define PING_SIZE            20
#define PING_REQUEST_TIMEOUT 3000
#define PING_ATTEMPT         3

#define PING_TIMEOUT    3000    /* In msecs */
#define PING_PKT_SIZE   20      /* In bytes */
#define NO_OF_ATTEMPTS  3

/* Status bits - These are used to set/reset the corresponding bits in a 'status_variable' */
typedef enum{
    STATUS_BIT_CONNECTION =  0, /* If this bit is:
                                 *      1 in a 'status_variable', the device is connected to the AP
                                 *      0 in a 'status_variable', the device is not connected to the AP
                                 */

    STATUS_BIT_IP_ACQUIRED,       /* If this bit is:
                                  *      1 in a 'status_variable', the device has acquired an IP
                                  *      0 in a 'status_variable', the device has not acquired an IP
                                  */

    STATUS_BIT_PING_DONE,        /* If this bit is:
                                  *      1 in a 'status_variable', the device has completed the ping operation
                                  *      0 in a 'status_variable', the device has not completed the ping operation
                                  */

    STATUS_BIT_IP_LEASED,        /* If this bit is:
                                  *      1 in a 'status_variable', the device has a client connected to it
                                  *      and the IP has been leased
                                  *      0 in a 'status_variable', the device has no clients connected to it
                                  */
    STATUS_BIT_STA_CONNECTED     /* If this bit is:
                                  *      1 in a 'status_variable', a station has connected to the device
                                  *      and the IP has been leased
                                  *      0 in a 'status_variable', the station couldn't connect to the device
                                  */

}e_StatusBits;

/* Application specific status/error codes */
typedef enum{
    LAN_CONNECTION_FAILED = -0x7D0,        /* Choosing this number to avoid overlap w/ host-driver's error codes */
    INTERNET_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
    DEVICE_NOT_IN_STATION_MODE = INTERNET_CONNECTION_FAILED - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

#define SET_STATUS_BIT(status_variable, bit)    status_variable |= (1<<(bit))
#define CLR_STATUS_BIT(status_variable, bit)    status_variable &= ~(1<<(bit))
#define GET_STATUS_BIT(status_variable, bit)    (0 != (status_variable & (1<<(bit))))

#define IS_CONNECTED(status_variable)           GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_CONNECTION)
#define IS_IP_ACQUIRED(status_variable)          GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_IP_ACQUIRED)
#define IS_PING_DONE(status_variable)           GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_PING_DONE)
#define IS_IP_LEASED(status_variable)           GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_IP_LEASED)
#define IS_STA_CONNECTED(status_variable)       GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_STA_CONNECTED)

typedef struct{
	int8_t  SSID[MAX_SSID_LEN];
	int32_t  encryption;
	int8_t  password[MAX_PASSKEY_LEN];
}UserInfo;

extern int wifi_activ;
extern int wifi_config;

/*
 * GLOBAL VARIABLES -- Start
 */
uint8_t  g_Status = 0;
uint32_t g_PingPacketsRecv = 0;
uint32_t sta_IP = 0;
uint32_t g_GatewayIP = 0;

/*
 * GLOBAL VARIABLES -- End
 */

/*
 * STATIC FUNCTION DEFINITIONS -- Start
 */
int32_t  configureSimpleLinkToDefaultState(int8_t *);
int32_t  initializeAppVariables();
//int32_t  ConfigureAPmode(UserInfo UserParams);
int32_t checkLanConnection();
int32_t checkInternetConnection();
int32_t establishConnectionWithAP(UserInfo UserParams);
/*
 * STATIC FUNCTION DEFINITIONS -- End
 */

/* Connection status variables*/
bool deviceConnected = false;
bool ipAcquired = false;

/*
 *  ======== SimpleLinkWlanEventHandler ========
 *  SimpleLink Host Driver callback for handling WLAN connection or
 *  disconnection events.
 */

/*!
    \brief This function handles WLAN events

    \param[in]      pWlanEvents is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    if(pWlanEvent == NULL)
        System_printf(" [WLAN EVENT] NULL Pointer Error \n\r");

     switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

            /*
             * Information about the connected AP (like name, MAC etc) will be
             * available in 'slWlanConnectAsyncResponse_t' - Applications
             * can use it if required
             *
             * slWlanConnectAsyncResponse_t *pEventData = NULL;
             * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
             *
             */
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_ACQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            /* If the user has initiated 'Disconnect' request, 'reason_code' is SL_USER_INITIATED_DISCONNECTION */
            if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                System_printf(" Device disconnected from the AP on application's request \r\n");
            }
            else
            {
                System_printf(" Device disconnected from the AP on an ERROR..!! \r\n");
            }
            g_GatewayIP = 0;
            wifi_activ=0;
        }
        break;

        case SL_WLAN_STA_CONNECTED_EVENT:
        {
            SET_STATUS_BIT(g_Status, STATUS_BIT_STA_CONNECTED);
            smartphone_access = 1;
        }
        break;

        case SL_WLAN_STA_DISCONNECTED_EVENT:
        {
            CLR_STATUS_BIT(g_Status, STATUS_BIT_STA_CONNECTED);
            CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_LEASED);
            smartphone_access = 0;
            sta_IP = 0;
            wifi_activ=0;
        }
        break;

        default:
        {
            System_printf(" [WLAN EVENT] Unexpected event \r\n");
        }
        break;
     }
}

/*!
    \brief This function handles events for IP address acquisition via DHCP
           indication

    \param[in]      pNetAppEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    if(pNetAppEvent == NULL)
        System_printf(" [NETAPP EVENT] NULL Pointer Error \n\r");

    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            SlIpV4AcquiredAsync_t *pEventData = NULL;

            SET_STATUS_BIT(g_Status, STATUS_BIT_IP_ACQUIRED);

            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;

            g_GatewayIP = pEventData->gateway;
        }
        break;

        case SL_NETAPP_IP_LEASED_EVENT:
        {
            sta_IP = pNetAppEvent->EventData.ipLeased.ip_address;
            SET_STATUS_BIT(g_Status, STATUS_BIT_IP_LEASED);
        }
        break;

        default:
        {
            System_printf(" [NETAPP EVENT] Unexpected event \r\n");
        }
        break;
    }
}

/*!
    \brief This function handles callback for the HTTP server events

    \param[in]      pHttpEvent - Contains the relevant event information
    \param[in]      pHttpResponse - Should be filled by the user with the
                    relevant response information

    \return         None

    \note

    \warning
*/
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
    /*
     * This application doesn't work with HTTP server - Hence these
     * events are not handled here
     */
    System_printf(" [HTTP EVENT] Unexpected event \r\n");
}

/*!
    \brief This function handles ping report events

    \param[in]      pPingReport holds the ping report statistics

    \return         None

    \note

    \warning
*/
void SimpleLinkPingReport(SlPingReport_t *pPingReport)
{
    SET_STATUS_BIT(g_Status, STATUS_BIT_PING_DONE);
    g_PingPacketsRecv = pPingReport->PacketsReceived;
}


/*!
    \brief This function handles general error events indication

    \param[in]      pDevEvent is the event passed to the handler

    \return         None
*/
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    /*
     * Most of the general errors are not FATAL are are to be handled
     * appropriately by the application
     */
    System_printf(" [GENERAL EVENT] \r\n");
}

/*!
    \brief This function handles socket events indication

    \param[in]      pSock is the event passed to the handler

    \return         None
*/
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if(pSock == NULL)
        System_printf(" [SOCK EVENT] NULL Pointer Error \n\r");

    switch( pSock->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:
        {
            /*
            * TX Failed
            *
            * Information about the socket descriptor and status will be
            * available in 'SlSockEventData_t' - Applications can use it if
            * required
            *
            * SlSockEventData_t *pEventData = NULL;
            * pEventData = & pSock->EventData;
            */
            switch( pSock->EventData.status )
            {
                case SL_ECLOSE:
                  /*
                   * Close Socket operation:- Failed to transmit all the
                   * queued packets.
                   */
                    System_printf(" [SOCK EVENT] Failed to transmit packets \r\n");
                break;
                default:
                    System_printf(" [SOCK EVENT] Unexpected event \r\n");
                break;
            }
        }
        break;

        default:
            System_printf(" [SOCK EVENT] Unexpected event \r\n");
        break;
    }
}
/*
 * ASYNCHRONOUS EVENT HANDLERS -- End
 */
int32_t initializeAppVariables()
{
    g_Status = 0;
    g_PingPacketsRecv = 0;
    sta_IP = 0;
    g_GatewayIP = 0;

    return SUCCESS;
}

/*!
    \brief This function puts the device in its default state. It:
           - Set the mode to STATION
           - Configures connection policy to Auto and AutoSmartConfig
           - Deletes all the stored profiles
           - Enables DHCP
           - Disables Scan policy
           - Sets Tx power to maximum
           - Sets power policy to normal
           - Unregister mDNS services
           - Remove all filters

    \param[in]      none

    \return         On success, zero is returned. On error, negative is returned
*/
int32_t configureSimpleLinkToDefaultState(int8_t *pConfig)
{
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    uint8_t           val = 1;
    uint8_t           configOpt = 0;
    uint8_t           configLen = 0;
    uint8_t           power = 0;

    int32_t          retVal = -1;
    int32_t          mode = -1;

//    mode = sl_Start(0, pConfig, 0);
    mode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(mode);

    /* If the device is not in station-mode, try configuring it in station-mode */
    if (ROLE_STA != mode)
    {
        if (ROLE_AP == mode)
        {
            /* If the device is in AP mode, we need to wait for this event before doing anything */
            while(!IS_IP_ACQUIRED(g_Status))	Task_sleep(100);
        }

        /* Switch to STA role and restart */
        retVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(retVal);

        retVal = sl_Stop(SL_STOP_TIMEOUT);
        ASSERT_ON_ERROR(retVal);

        retVal = sl_Start(0, pConfig, 0);
        ASSERT_ON_ERROR(retVal);

        /* Check if the device is in station again */
        if (ROLE_STA != retVal)
        {
            /* We don't want to proceed if the device is not coming up in station-mode */
            ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
        }
    }

    /* Get the device's version-information */
    configOpt = SL_DEVICE_GENERAL_VERSION;
    configLen = sizeof(ver);
    retVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (uint8_t *)(&ver));
    ASSERT_ON_ERROR(retVal);

    System_printf("Host Driver Version: %s\n",SL_DRIVER_VERSION);
    System_printf("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n",
                        ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
                        ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
                        ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
                        ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
                        ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);

    /* Set connection policy to Auto + SmartConfig (Device's default connection policy) */
    retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Remove all profiles */
    retVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(retVal);

    /*
     * Device in station-mode. Disconnect previous connection if any
     * The function returns 0 if 'Disconnected done', negative number if already disconnected
     * Wait for 'disconnection' event if 0 is returned, Ignore other return-codes
     */
    retVal = sl_WlanDisconnect();
    if(0 == retVal)
    {
        /* Wait */
        while(IS_CONNECTED(g_Status))  Task_sleep(100);
    }

    /* Enable DHCP client*/
    retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&val);
    ASSERT_ON_ERROR(retVal);

    /* Disable scan */
    configOpt = SL_SCAN_POLICY(0);
    retVal = sl_WlanPolicySet(SL_POLICY_SCAN , configOpt, NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Set Tx power level for station mode
       Number between 0-15, as dB offset from max power - 0 will set maximum power */
    power = 0;
    retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (uint8_t *)&power);
    ASSERT_ON_ERROR(retVal);

    /* Set PM policy to normal */
    retVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Unregister mDNS services */
    retVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(retVal);

    /* Remove  all 64 filters (8*8) */
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    retVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (uint8_t *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(retVal);

    retVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(retVal);

    retVal = initializeAppVariables();
    ASSERT_ON_ERROR(retVal);

    return retVal; /* Success */
}

/*!
    \brief Connecting to a WLAN Access point

    This function connects to the AP (SSID_NAME).
    The function will return once we are connected and have acquired IP address

    \param[in]  UserParams - Structure having SSID, security type and pass key
                of AP to connect

    \return     0 for success, negative otherwise

    \note

    \warning    If the WLAN connection fails or we don't acquire an IP address,
                We will be stuck in this function forever.
*/
int32_t establishConnectionWithAP(UserInfo UserParams)
{
    SlSecParams_t secParams = {0};
    int32_t retVal = -1;

    if(UserParams.encryption == 0)
    {
        secParams.Key = "";
        secParams.KeyLen = 0;
        secParams.Type = SL_SEC_TYPE_OPEN;
    }

    else
    {
        secParams.Key = UserParams.password;
        secParams.KeyLen = strlen((const char *)UserParams.password);
        secParams.Type = UserParams.encryption;
    }


    retVal = sl_WlanConnect(UserParams.SSID,strlen((const char *)UserParams.SSID),0,
                                                                &secParams,0);
    ASSERT_ON_ERROR(retVal);
//    System_printf("Connecting to AP %s...\n", UserParams.SSID  );

    /* Wait */
    while((!IS_CONNECTED(g_Status)) || (!IS_IP_ACQUIRED(g_Status)))
    {
    	Task_sleep(100);
    }

    return retVal;
}

/*!
    \brief This function checks the LAN connection by pinging the AP's gateway

    \param[in]  None

    \return     0 on success, negative error-code on error
*/
int32_t checkLanConnection()
{
    SlPingStartCommand_t pingParams = {0};
    SlPingReport_t pingReport = {0};

    int32_t retVal = -1;

    CLR_STATUS_BIT(g_Status, STATUS_BIT_PING_DONE);
    g_PingPacketsRecv = 0;

    /* Set the ping parameters */
    pingParams.PingIntervalTime = PING_INTERVAL;
    pingParams.PingSize = PING_PKT_SIZE;
    pingParams.PingRequestTimeout = PING_TIMEOUT;
    pingParams.TotalNumberOfAttempts = NO_OF_ATTEMPTS;
    pingParams.Flags = 0;
    pingParams.Ip = g_GatewayIP;

    /* Check for LAN connection */
    retVal = sl_NetAppPingStart( (SlPingStartCommand_t*)&pingParams, SL_AF_INET,
                                 (SlPingReport_t*)&pingReport, SimpleLinkPingReport);
    ASSERT_ON_ERROR(retVal);

    /* Wait */
    while(!IS_PING_DONE(g_Status))	Task_sleep(100);

    if(0 == g_PingPacketsRecv)
    {
        /* Problem with LAN connection */
        ASSERT_ON_ERROR(LAN_CONNECTION_FAILED);
    }

    /* LAN connection is successful */
    return SUCCESS;
}

/*!
    \brief This function checks the internet connection by pinging
           the external-host (HOST_NAME)

    \param[in]  None

    \return     0 on success, negative error-code on error
*/
int32_t checkInternetConnection()
{
    SlPingStartCommand_t pingParams = {0};
    SlPingReport_t pingReport = {0};

    unsigned long ipAddr = 0;

    int32_t retVal = -1;

    CLR_STATUS_BIT(g_Status, STATUS_BIT_PING_DONE);
    g_PingPacketsRecv = 0;

    /* Set the ping parameters */
    pingParams.PingIntervalTime = PING_INTERVAL;
    pingParams.PingSize = PING_PKT_SIZE;
    pingParams.PingRequestTimeout = PING_TIMEOUT;
    pingParams.TotalNumberOfAttempts = NO_OF_ATTEMPTS;
    pingParams.Flags = 0;
    pingParams.Ip = g_GatewayIP;

//    /* Check for Internet connection */
//    retVal = sl_NetAppDnsGetHostByName((int8_t *)HOST_NAME, sizeof(HOST_NAME), &ipAddr, SL_AF_INET);
//    ASSERT_ON_ERROR(retVal);

    /* Replace the ping address to match HOST_NAME's IP address */
    pingParams.Ip = ipAddr;
    pingParams.Ip =  SL_IPV4_VAL(portal_ip[3],portal_ip[2],portal_ip[1],portal_ip[0]);
    //SL_IPV4_VAL(119,18,57,185);//www.avkon.in

    /* Try to ping HOST_NAME */
    retVal = sl_NetAppPingStart( (SlPingStartCommand_t*)&pingParams, SL_AF_INET,
                                 (SlPingReport_t*)&pingReport, SimpleLinkPingReport);
    ASSERT_ON_ERROR(retVal);

    /* Wait */
    while(!IS_PING_DONE(g_Status))  Task_sleep(100);

    if (0 == g_PingPacketsRecv)
    {
        /* Problem with internet connection*/
        ASSERT_ON_ERROR(INTERNET_CONNECTION_FAILED);
    }

    /* Internet connection is successful */
    return SUCCESS;
}



/*
 *  ======== setAPMode ========
 *  Sets the SimpleLink Wi-F in station mode and enables DHCP client
 */
void setAPMode(void) {
	char z=0;
    int           mode;
    int           response;
    unsigned char param;
    SlNetCfgIpV4Args_t ipV4 = {0};
    SlNetAppDhcpServerBasicOpt_t dhcpParams = {0};

    mode = sl_Start(0, 0, 0);
    if (mode < 0) {
     //   System_abort("Could not initialize SimpleLink Wi-Fi");
    }

    /* Change network processor to Access Point mode */
    if (mode != ROLE_AP) {
        sl_WlanSetMode(ROLE_AP);

        /* Restart network processor */
        sl_Stop(BIOS_WAIT_FOREVER);
        mode = sl_Start(0, 0, 0);
        if (mode < 0) {
       //     System_abort("Failed to set SimpleLink Wi-Fi to Station mode");
        }
    }

    uint8_t  str[33];
    uint16_t len;
    memset(str, 0, 33);
    //len=strlen(SSID_AP_MODE);
    len=strlen(wifi_name);
    memcpy(str, wifi_name, len);

    for(z=0;z<20;z++)
    {
    	if(str[z] == 32)	str[z] = 0;
    }
    len=strlen((const char *)str);
    /* Configure the SSID of the CC3100 */
    response = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SSID,len, str);
    if (response < 0) {
      //      System_abort("Failed to set Access Point SSID");
        }

    /* Configure the Security parameter for the AP mode */
    param = SEC_TYPE_AP_MODE;
    response = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SECURITY_TYPE, 1,
                (uint8_t *)&param);
    if (response < 0) {
		//	System_abort("Failed to set Security Parameter");
		}

    memset(str, 0, 33);
	//len=strlen(PASSWORD_AP_MODE);
	len=strlen(wifi_pass);
	memcpy(str, wifi_pass, len);

	for(z=0;z<20;z++)
	{
		if(str[z] == 32)	str[z] = 0;
	}
	len=strlen((const char *)str);

    response = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_PASSWORD, len,str);
    if (response < 0) {
		//	System_abort("Failed to set Password");
		}

    param = 4;//Channel 4
    response = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_CHANNEL, 1, (uint8_t *)&param);
    if (response < 0) {
    		//	System_abort("Failed to set Channel");
    		}

    /* Configure the Static IP */
    ipV4.ipV4 = CONFIG_IP;
	ipV4.ipV4Mask = CONFIG_MASK;
	ipV4.ipV4Gateway = CONFIG_GATEWAY;
	ipV4.ipV4DnsServer = CONFIG_DNS;

	response = sl_NetCfgSet(SL_IPV4_AP_P2P_GO_STATIC_ENABLE,1,sizeof(SlNetCfgIpV4Args_t),
			(uint8_t *)&ipV4);
	if (response < 0) {
	//	System_abort("Failed to Default Gateway");
	}

	dhcpParams.lease_time      = IP_LEASE_TIME;
	dhcpParams.ipv4_addr_start =  DHCP_START_IP;
	dhcpParams.ipv4_addr_last  =  DHCP_END_IP;

	response = sl_NetAppSet(SL_NET_APP_DHCP_SERVER_ID, NETAPP_SET_DHCP_SRV_BASIC_OPT,
			sizeof(SlNetAppDhcpServerBasicOpt_t), (uint8_t*)&dhcpParams);
	if (response < 0) {
	//	System_abort("Failed to Start DHCP Server");
	}

    sl_Stop(BIOS_WAIT_FOREVER);

    /* Set connection variables to initial values */
    deviceConnected = false;
    ipAcquired = false;
}

//void setStationMode(void) {
//    int           mode;
//    int           response;
//    unsigned char param;
//
//    mode = sl_Start(0, 0, 0);
//    if (mode < 0) {
//        System_abort("Could not initialize SimpleLink Wi-Fi");
//    }
//
//    /* Change network processor to station mode */
//    if (mode != ROLE_STA) {
//        sl_WlanSetMode(ROLE_STA);
//
//        /* Restart network processor */
//        sl_Stop(BIOS_WAIT_FOREVER);
//        mode = sl_Start(0, 0, 0);
//        if (mode < 0) {
//            System_abort("Failed to set SimpleLink Wi-Fi to Station mode");
//        }
//    }
//
//    sl_WlanDisconnect();
//
//    /* Set auto connect policy */
//    response = sl_WlanPolicySet(SL_POLICY_CONNECTION,
//            SL_CONNECTION_POLICY(1, 0, 0, 0, 0), NULL, 0);
//    if (response < 0) {
//        System_abort("Failed to set connection policy to auto");
//    }
//
//    /* Enable DHCP client */
//    param = 1;
//    response = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, 1, 1, &param);
//    if(response < 0) {
//        System_abort("Could not enable DHCP client");
//    }
//
//    sl_Stop(BIOS_WAIT_FOREVER);
//
//    /* Set connection variables to initial values */
//    deviceConnected = false;
//    ipAcquired = false;
//}


/*
 *  ======== socketsShutDown ========
 *  Generic routine, defined to close down the WiFi in this case.
 */
void socketsShutDown(void *handle)
{
    WiFi_close((WiFi_Handle)handle);
}

UserInfo User1={0};
/*
 *  ======== socketsStartUp ========
 *  Generic routine, in this case defined to open the WiFi and await a
 *  connection, using Smart Config if the appropriate button is pressed.
 */
void *socketsStartUp(void)
{
    int8_t *pConfig = NULL;
    WiFi_Params        wifiParams;
    WiFi_Handle        handle;
//    SlNetCfgIpV4Args_t ipV4;
    int                result;
    int l;

    /* Open WiFi driver */
    WiFi_Params_init(&wifiParams);
    wifiParams.bitRate = SPI_BIT_RATE;
    handle = WiFi_open(Board_WIFI, Board_SPI_CC3100, NULL, &wifiParams);
    if (handle == NULL) {
      //  System_abort("WiFi driver failed to open.");
      // Give Alert
    }
	for(l=0;l<1;l++)
	{
		result = configureSimpleLinkToDefaultState(pConfig);
		if (result < 0) {
		 //   System_abort("Could not intialize SimpleLink Wi-Fi");
		 // Give Alert
		}

		if(wireless_mode == 1)
		{
			/* Host driver starts the network processor */
			result = sl_Start(NULL, NULL, NULL);
			if (result < 0) {
			 //   System_abort("Could not intialize SimpleLink Wi-Fi");
			}

			memset(User1.SSID,0, sizeof(User1.SSID));
			sprintf ((char *)User1.SSID,ext_apn);

			memset(User1.password,0, sizeof(User1.password));
			sprintf ((char *)User1.password,ext_pass);

			User1.encryption = 2;//WPA

			result = establishConnectionWithAP(User1);

			wifi_config=1;
			if (result < 0) {
			 //   System_abort("Could not intialize SimpleLink Wi-Fi");
			}

		//    result = checkLanConnection();
		//   	if(result < 0)
		//   	{
		//   	   System_printf(" Device couldn't connect to the internet, Error code: %ld\r\n",result);
		//   	  // LOOP_FOREVER();
		//   	}

			result = checkInternetConnection();//todo
			if(result < 0)
			{
			   System_printf(" Device couldn't connect to the internet, Error code: %ld\r\n",result);
			  // LOOP_FOREVER();
			}
			else
			{
				wifi_activ=1;
			}
		}
		else
		{
			/* Set the CC3100 into AP Mode for this example */
			setAPMode();
			wifi_config=1;
			/* Host driver starts the network processor */
			result = sl_Start(NULL, NULL, NULL);
			if (result < 0) {
			 //   System_abort("Could not intialize SimpleLink Wi-Fi");
			}
			wifi_activ=1;
		}
		//Task_sleep(2000);
	}

    /*
     * Wait for SimpleLink to connect to an AP. If connecting to the AP for
     * the first time, press Board_BUTTON0 to start SmartConfig.
     */
    while ((g_GatewayIP == 0) && (sta_IP == 0)) {

        Task_sleep(100);
    }

    //GPIO_write(Board_LED1, Board_LED_ON);

//    /* Print IP address */
//    sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO, &dhcpIsOn, &len,
//            (unsigned char *)&ipV4);
//    System_printf(
//            "CC3100 has connected to an AP and acquired an IP address.\n");
//    System_printf("IP Address: %d.", SL_IPV4_BYTE(ipV4.ipV4, 3));
//    System_printf("%d.", SL_IPV4_BYTE(ipV4.ipV4, 2));
//    System_printf("%d.", SL_IPV4_BYTE(ipV4.ipV4, 1));
//    System_printf("%d", SL_IPV4_BYTE(ipV4.ipV4, 0));
//    System_flush();
    /* pass back the handle to our WiFi device */
    return ((void *)handle);
}
