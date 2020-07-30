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
 *  ======== sockets.h ========
 *  Simple compatibility file, used to factor out platform specific code to
 *  allow our sockets Task to use generic, portable BSD sockets C code.
 *
 *  This is meant to be just good enough for the simple TI-RTOS examples.
 */
#ifndef _SOCKETS_H_
#define _SOCKETS_H_

/* Configuration of the device when it comes up in AP mode */
#define SSID_AP_MODE       "IR000002"       /* SSID of the CC3100 in AP mode */
#define PASSWORD_AP_MODE   "avkon134"                  /* Password of CC3100 AP */

#define SEC_TYPE_AP_MODE   	SL_SEC_TYPE_WPA_WPA2  //SL_SEC_TYPE_OPEN    /* Can take SL_SEC_TYPE_WEP or
                                                /* SL_SEC_TYPE_WPA as well */


#define CONFIG_IP       SL_IPV4_VAL(192,168,0,1)    /* Static IP to be configured */
#define CONFIG_MASK     SL_IPV4_VAL(255,255,255,0)  /* Subnet Mask for the station */
#define CONFIG_GATEWAY  SL_IPV4_VAL(192,168,0,1)    /* Default Gateway address */
#define CONFIG_DNS      SL_IPV4_VAL(192,168,0,1)    /* DNS Server Address */


#define DHCP_START_IP    SL_IPV4_VAL(192,168,0,100) /* DHCP start IP address */
#define DHCP_END_IP      SL_IPV4_VAL(192,168,0,200) /* DHCP End IP address */
#define IP_LEASE_TIME        3600

/*
 *  ======== asynchCallback ========
 */
void asynchCallback(long eventType, char *data, unsigned char length);

/*
 *  ======== smartConfigFxn ========
 */
void smartConfigFxn(void);

/*
 *  ======== socketsShutDown ========
 */
void socketsShutDown(void *handle);

/*
 *  ======== socketsStartUp ========
 */
void *socketsStartUp(void);

#endif /* _SOCKETS_H_ */
