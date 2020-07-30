#include <string.h>
#include <stdio.h>
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/GPIO.h>

/* SimpleLink Wi-Fi Host Driver Header files */
#include <simplelink.h>
/* Local Platform Specific Header file */
#include "sockets.h"

/* Example/Board Header file */
#include "Board.h"

/* Local Platform Specific Header file */
#include "sockets.h"
#include "GlobalConst.h"		/* Global Constants prototypes     	  */

#define LOGFILE_BUFF_SIZE       20000

#define TCPPORT         1000
extern char wireless_mode;// 0=Local, 1=Internet
extern char portal_ip[4];
extern char portal_name[40];

extern void  message_store();
extern void  synctoportal(char table);
extern uint32_t strtoint1(char *tempstring);
extern uint8_t Read_log(uint16_t read_startloc);
extern void socketsShutDown(void *handle);
extern void *socketsStartUp(void);

int flag1_30s=1;
extern 	char message[8][30];	//for temporary storing the message
extern	char input_buffer[1000];
extern	char ouput_buffer[20000];
extern char temp1[20000];
extern uint8_t remote_data_recevied_flag;


extern char logfile_buff[LOGFILE_BUFF_SIZE];
extern volatile uint8_t PanelState;
extern uint32_t tempmsgid;
extern uint32_t noofmsg;
extern uint8_t remote_data_recevied_flag;
extern void *netIF;

extern int taskactive;
extern int logflag;
extern int logsend;

extern char msgtoportal[200];
extern char logtoportal[200];
extern char msgfromportal[200];


extern char deviceid[11];
extern char str[20],len[10],temp[100];

extern int PrevPanelState;
extern char synccount;
extern char commu_on;//1 if GSM Module in connect mode
extern char sendack;
extern char system_rst;
extern uint8_t localaccessflag;

extern int wifi_activ;
extern int wifi_config;
extern void *netIF;

#define TCPPACKETSIZE   15000

void WIFI_Main();
void WIFI_Internet();
void echoFxn(int port);

extern void GSM_EEPROM_UPDATE();
/*
 *  ======== echoFxn ========
 *  Echoes TCP messages.
 */
void echoFxn(int port)
{
	int i=0,i1=0,j=0,k=0,l=0,m=0;
	int start=0,end=0;
    int         bytesRcvd;
    int         bytesSent;
    int         status;
    int         clientfd;
    int         server;
    sockaddr_in localAddr;
    sockaddr_in clientAddr;
    socklen_t   addrlen = sizeof(clientAddr);
   // char        buffer[TCPPACKETSIZE];

    memset(ouput_buffer,0, sizeof(ouput_buffer));
    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) {
        //System_printf("Error: socket not created.\n");
        goto shutdown;
    }

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(port);

    status = bind(server, (const sockaddr *)&localAddr, sizeof(localAddr));
    if (status == -1) {
        //System_printf("Error: bind failed.\n");
        goto shutdown;
    }

    status = listen(server, 0);
    if (status == -1){
        //System_printf("Error: listen failed.\n");
        goto shutdown;
    }

    while ((clientfd =
            accept(server, (sockaddr *)&clientAddr, &addrlen)) != -1) {


        while ((bytesRcvd = recv(clientfd, ouput_buffer, TCPPACKETSIZE, 0)) > 0) {
        	////System_printf("%s\r\n",ouput_buffer);
        //	System_flush();


			i=0;
			//for(i=0;i<1;i++)
			//{
				repeat:if((ouput_buffer[i]=='*')&&(ouput_buffer[i+1]=='|'))
				{
					for(i1=0;i1<8;i1++)
					{
						memset(message[i1],0, sizeof(message[i1]));
					}
					j=i+2;	start=j;	k=j;
					do
					{
						if(((ouput_buffer[k]=='|')&&(ouput_buffer[k+1]=='|'))||((ouput_buffer[k]=='|')&&(ouput_buffer[k+1]=='*')))
						{
							end=k;	k=k+1;
							if(k>19999)	k=0;

							while(start<end)
							{
								message[m][l]=ouput_buffer[start];
								l++;	start++;
							}
							m++;
							if(m==8)	m=0;
							l=0;
							start=start+2;
						}
						k++;
						if(k>19999)	k=0;

						j++;
						if(j>19999)	j=0;

					}while((ouput_buffer[j]!='*'));

					message_store();

					remote_data_recevied_flag = 1;

					if(ouput_buffer[j+1]== 32)
					{
						i=j+2;
						goto repeat;
					}

				}
			//}

			if (strcmp(message[0], "table_data")==0)
			{
				bytesSent = send(clientfd, ouput_buffer, strlen(ouput_buffer), 0);
			}
			else
			{
				bytesSent = send(clientfd, ouput_buffer, bytesRcvd, 0);
			}
			if (bytesSent < 0 || bytesSent != bytesRcvd) {
				////System_printf("Error: send failed.\n");
				//break;
			}
			/* addrlen is a value-result param, must reset for next accept call */
			addrlen = sizeof(clientAddr);
			close(clientfd);

			memset(&ouput_buffer, 0, sizeof(ouput_buffer));

			start=0;	end=0;	i=0;	j=0;
			k=0;	l=0;	m=0;
			GSM_EEPROM_UPDATE();		// Func used to update the values in eeprom
        }

    }

    //System_printf("Error: accept failed.\n");

shutdown:
    if (server >= 0) {
        close(server);
    }
}


void WIFI_Main()
{
	if((wifi_activ==1)&&(wifi_config==1))
	{
		if(wireless_mode == 1)
		{
			if(flag1_30s>=200)
			{
				WIFI_Internet();
			}
		}
		else
		{
			echoFxn(TCPPORT);
		}
	}
	else
	{
		if(wireless_mode == 1)
		{
			/*Reset the Connection*/
			socketsShutDown(netIF);
			Task_sleep(1000);
			netIF = socketsStartUp();
		}
	}

	/* Close the network - don't do this if other tasks are using it */
//	socketsShutDown(netIF);

	Task_sleep(20);
}
uint8_t taskKey;
uint8_t  hwiKey;

void WIFI_Internet()
{
	SlSockAddrIn_t  Addr;

	int16_t sd = 0;
	int16_t AddrSize = 0;
	int32_t ret_val = 0;

	int i=0,i1=0,j=0,k=0,l=0,m=0;
	int start=0,end=0;

	Addr.sin_family = SL_AF_INET;
	Addr.sin_port = sl_Htons(80);

	/* Connect to avkon.in(119.18.57.185) */
	/* Connect to avkon.in(103.53.42.113) */

	/* Change the DestinationIP endianity, to big endian */
	//Addr.sin_addr.s_addr = sl_Htonl(SL_IPV4_VAL(119,18,57,185));
	//Addr.sin_addr.s_addr = sl_Htonl(SL_IPV4_VAL(103,53,42,113));
	Addr.sin_addr.s_addr = sl_Htonl(SL_IPV4_VAL(portal_ip[3],portal_ip[2],portal_ip[1],portal_ip[0]));

	AddrSize = sizeof(SlSockAddrIn_t);

	//socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sd = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, SL_IPPROTO_TCP );//Socket ID is returned
	if( sd < 0 )
	{
		//System_printf(" Error creating socket\n\r\n\r");
	}

	ret_val = sl_Connect(sd, ( SlSockAddr_t *)&Addr, AddrSize);
	if( ret_val < 0 )
	{
		/* error */
		//System_printf(" Error connecting to server\n\r\n\r");
		wifi_activ=0;//internet connection disturbed so re connect
	}

	/* Get Data from the server using a HTTP Post Call */
	if((PanelState==psPassword)||(PanelState==psSetup)||(PanelState==psManual)||(PanelState==psProgram)||(PanelState==psInitSetup))
	{
		PrevPanelState=PanelState;
		localaccessflag=2;

		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|Access_data||LocalAccessOn||%d||0||%s||0||0|*\r\n",localaccessflag,deviceid);

		memset(ouput_buffer,0, sizeof(ouput_buffer));
		memset(len,0, sizeof(len));

		sprintf(len, "%d", (strlen(temp)+8));
		strcat (ouput_buffer,msgtoportal);
		strcat (ouput_buffer,len);
		strcat (ouput_buffer,"\r\n\r\n");
		strcat (ouput_buffer,"ackdata=");
		strcat (ouput_buffer,temp);

	}
	if(((PanelState==psHome)||(PanelState==psLogs))&&(PrevPanelState==psHome))
	{
		if(logsend == 0)
		{
			Read_log(0);
			memset(ouput_buffer,0, sizeof(ouput_buffer));
			memset(len,0, sizeof(len));

			sprintf(len, "%d", (strlen(logfile_buff)+34));
			strcat (ouput_buffer,logtoportal);
			strcat (ouput_buffer,len);
			strcat (ouput_buffer,"\r\n\r\n");
			strcat (ouput_buffer,"logdata=");
			strcat (ouput_buffer,"*|log_data||");
			strcat (ouput_buffer,deviceid);
			strcat (ouput_buffer,"||");
			strcat (ouput_buffer,logfile_buff);
			strcat (ouput_buffer,"|*\r\n");
			memset(logfile_buff,0, sizeof(logfile_buff));
			logsend = 1;

		}
		else
		{
			if(sendack==0)
			{
				noofmsg=0;
				memset(ouput_buffer,0, sizeof(ouput_buffer));
				memset(len,0, sizeof(len));

				strcat (ouput_buffer,msgfromportal);
				//strcat (ouput_buffer,msgfromportal);
				sprintf(len, "%d", (strlen(deviceid)+13));
				strcat (ouput_buffer,len);
				strcat (ouput_buffer,"\r\n\r\n");
				strcat (ouput_buffer,"deviceiddata=");
				strcat (ouput_buffer,deviceid);
				sendack=1;
				flag1_30s=190;
				//remote_data_recevied_flag = 1;
			}
			else
			{
				sendack=0;
				synctoportal(10);
			}
		}
	}
	if((PanelState==psHome)&&(PrevPanelState!=psHome))
	{
		if(localaccessflag==2)
		{
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|Access_data||LocalAccessOn||%d||0||%s||0||0|*\r\n",localaccessflag,deviceid);

			memset(ouput_buffer,0, sizeof(ouput_buffer));
			memset(len,0, sizeof(len));

			sprintf(len, "%d", (strlen(temp)+8));
			strcat (ouput_buffer,msgtoportal);
			strcat (ouput_buffer,len);
			strcat (ouput_buffer,"\r\n\r\n");
			strcat (ouput_buffer,"ackdata=");
			strcat (ouput_buffer,temp);
			localaccessflag=0;
		}
		else
		{
			if(system_rst==1)
			{
				memset(temp,0, sizeof(temp));
				memset(ouput_buffer,0, sizeof(ouput_buffer));
				memset(len,0, sizeof(len));

				sprintf (temp,"*|Access_data||LocalAccessOn||%d||0||%s||0||0|*\r\n",localaccessflag,deviceid);
				strcat (temp1,temp);

				sprintf(len, "%d", (strlen(temp1)+8));
				strcat (ouput_buffer,msgtoportal);
				strcat (ouput_buffer,len);
				strcat (ouput_buffer,"\r\n\r\n");
				strcat (ouput_buffer,"ackdata=");
				strcat (ouput_buffer,temp1);
				memset(temp1,0, sizeof(temp1));
				PrevPanelState=psHome;
			}
			if(system_rst==0)
			{
				if(PrevPanelState!=psHome)
				{
					flag1_30s=190;
					//synccount=6;
					//synccount++;
					synctoportal(synccount);
					if(synccount>9)
					{
						PrevPanelState=psHome;
						synccount=0;
						system_rst=1;
					}
				}
			}
		}
	}

	ret_val = sl_Send(sd, ouput_buffer,strlen(ouput_buffer), 0);
	if(ret_val != strlen(ouput_buffer));
		//ASSERT_ON_ERROR(HTTP_SEND_ERROR);

	memset(ouput_buffer,0, sizeof(ouput_buffer));

	/* Receive response */
	//taskKey = Task_disable();
//	hwiKey = Hwi_disable();
	ret_val = sl_Recv(sd, &ouput_buffer[0], sizeof(ouput_buffer), 0);
	if(ret_val <= 0);
		//ASSERT_ON_ERROR(HTTP_RECV_ERROR);
//	Hwi_restore(hwiKey);
//	Task_restore(taskKey);
	ret_val = sl_Close(sd);

	/* Message Separation Logic */
	i=0;
	while(ouput_buffer[i] != 0)
	{
		repeat:if((ouput_buffer[i]=='*')&&(ouput_buffer[i+1]=='|'))
		{
			noofmsg++;
			for(i1=0;i1<8;i1++)
			{
				memset(message[i1],0, sizeof(message[i1]));
			}
			j=i+2;	start=j;	k=j;
			do
			{
				if(((ouput_buffer[k]=='|')&&(ouput_buffer[k+1]=='|'))||((ouput_buffer[k]=='|')&&(ouput_buffer[k+1]=='*')))
				{
					end=k;	k=k+1;
					if(k>19999)	k=0;

					do
					{
						message[m][l]=ouput_buffer[start];
						l++;	start++;
					}while(start<end);
					m++;
					if(m==7)
					{
						m=0;
						break;
					}
					l=0;
					start=start+2;
				}
				k++;
				if(k>19999)	k=0;

				j=k;

//				j++;
//				if(j>19999)	j=0;

			}while((ouput_buffer[j]!='*'));


			strcpy(str, "nodata");
			if(strncmp(message[0],str,6)!=0)
			{
				remote_data_recevied_flag = 1;
				flag1_30s=200;
			}
			else
			{
				flag1_30s=1;
			}
			tempmsgid=(uint32_t)strtoint1(message[3]);

			if(remote_data_recevied_flag == 1)	message_store();



			start=0;	end=0;
				k=0;	l=0;	m=0;
			if(ouput_buffer[j+3]== 0)
			{
				break;
			}

			if(ouput_buffer[j+2]== 10)//13-cr, 10-lf,38-&
			{
				i=j+3;
				goto repeat;
			}
		}
		i++;
	}
}


