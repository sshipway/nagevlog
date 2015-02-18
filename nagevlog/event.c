/*-----------------------------------------------------------------------------
 *
 *  event.c - Event module 
 *
 *    Copyright (c) 1998, SaberNet.net - All rights reserved
 *
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License
 *    as published by the Free Software Foundation; either version 2
 *    of the License, or (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307
 *
 *  $Id: event.c,v 1.11 2002/09/20 06:12:47 jason Exp jason $
 *
 *  Revision history:
 *    01-Dec-1999  JRR  Removed superfluous gethostbyaddr
 *    28-Sep-1999  JRR  Added support for secondary syslog host
 *    18-Aug-1998  JRR  Module completed
 *
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include "event.h"

#include "send-nsca.h"
#include "nsca.h"

/*-------------------------------[ static data ]------------------------------*/
#define SYSLOG_NAME         "nsca"
#define SYSLOG_PORT         5667
#define REG_BUFFER_LEN      1024
#define NSCA_REG_KEY_0    "NSCAD"  /* we now log to NSCA daemon */
#define NSCA_REG_KEY_1    "NSCAD1"
#define NSCA_PORT_KEY_0    "Port"  
#define NSCA_PORT_KEY_1    "Port1"

void nsca_host(char *host,  int *port, char *regkey, char*pregkey);

/*------------------------------[ event_output ]------------------------------
 *  Output the event
 *
 *  Parameters:
 *		event		event to format
 *		fp			file pointer
 *
 *  Return value:
 *		success		(0)
 *		failure		(-1)
 *
 *----------------------------------------------------------------------------*/

/* from nsca.c */
extern char * nscahostname;
extern int logalerts;
extern int read_config();

int _event_output(ntsl_event *eventp, char *server_name, int port)
{
	WSADATA  			 wsaData;
	int rv;
	char nscamsg[MAX_PLUGINOUTPUT_LENGTH];

	if ( !eventp || !server_name || !*server_name || !port)
		return(-1);

	if((eventp->status > 3) || !eventp->service[0]) {
		if(logalerts) {
			ntsl_log_info("Ignored event. Filter matching (%s)",
				  (eventp->filter?eventp->filter:"Unknown"));
		}
		return(0);
	}

    if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
    {
        ntsl_log_error(NTSL_ERROR_WSASTARTUP, WSAGetLastError());
        WSACleanup();
        return(-1);
    }

	_snprintf0(nscamsg,sizeof(nscamsg),"%s %s [%s #%d]: %s", 
		eventp->eventlog, eventp->etype, 
		eventp->source, eventp->id, eventp->msg );

	rv = send_nsca(server_name, port, eventp->service, eventp->status, nscamsg );
	if(rv) { 
		if(rv>1) {
			ntsl_log_error("Winsock error %d trying to send to NSCA.  Check the error logs for the NSCA server!", WSAGetLastError());
		}
		WSACleanup(); return(-1); 
	}

	WSACleanup();

	if(logalerts) {
		ntsl_log_info("Successfully sent NSCA notification to %s:%d\nService [%s], status %d\nMessage: %s\nFilter matching (%s)",
			  server_name, port, 
			  eventp->service, eventp->status,
			  nscamsg, 
			  (eventp->filter?eventp->filter:"Unknown"));
	}

	return(0);
}

char eohost[2][NTSL_SYS_LEN] = { "", "" };
int eoport[2] = { 5667, 5667 };

int event_output(ntsl_event *eventp)
{
	int rc[2] = {-1, -1};
	if (eohost[0][0] == (char)0) /* only do the lookup once */
	{
		nsca_host(eohost[0], eoport, NSCA_REG_KEY_0,NSCA_PORT_KEY_0);
		nsca_host(eohost[1], eoport+1, NSCA_REG_KEY_1,NSCA_PORT_KEY_1);

		if ((eohost[0][0] == 0 ) || (eoport[0] == 0)) 
			ntsl_die(NTSL_ERROR_SYSLOG_CONFIG);

	}
	
	rc[0] = _event_output(eventp, eohost[0],eoport[0]);
	if( eohost[1][0] && eoport[1] ) {
		rc[1] = _event_output(eventp, eohost[1],eoport[1]);
	}
	return(rc[0] == 0 || rc[1] == 0 ? 0 : -1);
}

void nsca_host(char *host, int *port, char *regkey, char *pregkey)
{
	char		buffer[REG_BUFFER_LEN];
	HKEY		hReg;
	int32		rc;
	int len;

	if (host != NULL)
	{
		*host = (char)0; *port = 0;		
		while(1)    // while loop used as control structure
		{
			_snprintf0(buffer, REG_BUFFER_LEN, "SOFTWARE\\%s\\Nagios", DEV_NAME);
			rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, buffer, 0, KEY_READ, &hReg);
			if (rc != ERROR_SUCCESS) {
				break;
			}
			len = NTSL_SYS_LEN;
			rc  = RegQueryValueEx(hReg, regkey, NULL, NULL, (unsigned char*)host, &len);
			if (rc != ERROR_SUCCESS) {
				*host = (char)0; *port = 0;
				break;
			}
			len = sizeof(DWORD);
			rc  = RegQueryValueEx(hReg, pregkey, NULL, NULL, (unsigned char*)port, &len);
			if (rc != ERROR_SUCCESS) {
				*port = 5667; /* default port */
				break;	
			}
			break;
		}		
		RegCloseKey(hReg);
	}
}
