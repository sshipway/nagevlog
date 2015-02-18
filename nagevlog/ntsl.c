/*-----------------------------------------------------------------------------
 *
 *  ntsl.c - NTSysLog main
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
 *  $Id: ntsl.c,v 1.10 2002/07/23 06:36:51 jason Exp $
 *
 *	Options:
 *	  _DEBUG     -  Turns on memory leak detection 
 *    NTSL_STUB  -  Minimal functions for module testing
 *
 *  Revision history:
 *    17-Aug-98  JRR  Module completed
 *
 *----------------------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <direct.h>
#include <process.h>
#include <time.h>
#include "ntsl.h"
#include "service.h"
#include "eventlog.h"
#include "engine.h"
#include "event.h"

/*-------------------------------[ static data ]------------------------------*/
#define	MAX_ERROR_LEN		1024
static void ntsl_cron(void *arg);
int eventlogdelay = 20;
extern int debugmode;
int heartbeatfreq = 0;
char hbsvc[64];

/*-------------------------------[ heartbeat ]------------------------------*/
void sendheartbeat( int status, char *msg ) {
	ntsl_event e;
	/* create a dummy event to log */
	if(!*hbsvc || !heartbeatfreq) { return; }
	strcpy_s(e.msg,sizeof(e.msg),msg);
	strcpy_s(e.service,sizeof(e.service),hbsvc);
	e.eventlog[0] = e.etype[0] = (char)0;
	e.status = status;
	e.id = status;
	if( status == 0 ) {	strcpy_s(e.source,sizeof(e.eventlog),"INFO");}
	if( status == 1 ) {	strcpy_s(e.source,sizeof(e.eventlog),"WARN");}
	if( status == 2 ) {	strcpy_s(e.source,sizeof(e.eventlog),"CRIT");}
	strcpy_s(e.eventlog,sizeof(e.eventlog),"HEARTBEAT");
	event_output(&e); /* send the dummy alert */
}

/*-----------------------------[ ntsl_log_error ]-----------------------------
 * Report error to the event log
 *----------------------------------------------------------------------------*/
void ntsl_log_error(char *format, ...)
{
	va_list     args;
    char        s[MAX_ERROR_LEN];
	
    va_start(args, format);
    _vsnprintf_s(s, MAX_ERROR_LEN,MAX_ERROR_LEN, format, args);
    va_end(args);

	s[MAX_ERROR_LEN - 1] = 0;
	ntsl_log_msg(EVENTLOG_ERROR_TYPE, s);

	/* commented out because an error on a NSCA send generates an error, which
	   would generate another send, etc etc etc */
/*	if(heartbeatfreq) { sendheartbeat(2,s); } /* send a heartbeat error */
}

/*------------------------------[ ntsl_log_info ]-----------------------------
 * Write information message to the event log
 *----------------------------------------------------------------------------*/
void ntsl_log_info(char *format, ...)
{
	va_list     args;
    char        s[MAX_ERROR_LEN];
	
    va_start(args, format);
    _vsnprintf_s(s, MAX_ERROR_LEN,MAX_ERROR_LEN, format, args);
    va_end(args);

	s[MAX_ERROR_LEN - 1] = 0;
	ntsl_log_msg(EVENTLOG_INFORMATION_TYPE, s);	
}

/*----------------------------[ ntsl_log_warning ]----------------------------
 * Write information message to the event log
 *----------------------------------------------------------------------------*/
void ntsl_log_warning(char *format, ...)
{
	va_list     args;
    char        s[MAX_ERROR_LEN];
	
    va_start(args, format);
    _vsnprintf_s(s, MAX_ERROR_LEN,MAX_ERROR_LEN, format, args);
    va_end(args);

	s[MAX_ERROR_LEN - 1] = 0;
	ntsl_log_msg(EVENTLOG_WARNING_TYPE, s);

/*	if(heartbeatfreq) { sendheartbeat(1,s); } /* send a heartbeat warning */
}

/*------------------------------[ ntsl_log_msg ]-----------------------------
 * Write message to the event log
 *----------------------------------------------------------------------------*/
void ntsl_log_msg(uint16 etype, char *format, ...)
{
    va_list     args;
    char        s[MAX_ERROR_LEN];
	
    va_start(args, format);
    _vsnprintf_s(s, MAX_ERROR_LEN,MAX_ERROR_LEN, format, args);
    va_end(args);

	s[MAX_ERROR_LEN - 1] = 0;

	{
		HANDLE		hEvent = RegisterEventSource(NULL, APP_NAME);
		char		*strings[2] = { s, NULL };

		ReportEvent(hEvent, etype, 0, 0, NULL, 1, 0, strings, NULL);
		DeregisterEventSource(hEvent);
	}
	#ifdef _DEBUG
    fprintf(stderr, "%s\n", s);
	#endif
}


/*--------------------------------[ ntsl_die ]--------------------------------
 * Log error and exit.
 *----------------------------------------------------------------------------*/
void ntsl_die(char *format, ...)
{
	va_list		args;
    char        s[MAX_ERROR_LEN];
	
    va_start(args, format);
    _vsnprintf_s(s, MAX_ERROR_LEN,MAX_ERROR_LEN, format, args);
    va_end(args);

	s[MAX_ERROR_LEN - 1] = 0;
	ntsl_log_msg(EVENTLOG_ERROR_TYPE, s);
/* this may cause the error on shutdown */
/*	if(heartbeatfreq) { sendheartbeat(2,s); } /* send a heartbeat error */

#ifndef NTSL_STUB
	service_stop();
#else
	exit(1);
#endif
}


#ifdef _DEBUG
#include <crtdbg.h>
#endif

/*--------------------------------[ ntsl_init ]--------------------------------
 * Initialize sybsystems
 *----------------------------------------------------------------------------*/
void ntsl_init()
{
	char		buffer[256];
	HKEY		hReg;
	int32		rc;
	int len;

#ifdef _DEBUG
	_CrtSetReportMode(_CRT_WARN,   _CRTDBG_MODE_FILE   );
	_CrtSetReportFile(_CRT_WARN,   _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode(_CRT_ERROR,  _CRTDBG_MODE_FILE   );
	_CrtSetReportFile(_CRT_ERROR,  _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE   );
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT );
#endif

	engine_init();
	eventlog_init();	

	eventlogdelay = 20;
	_snprintf0(buffer, sizeof(buffer), "SOFTWARE\\%s\\Nagios", DEV_NAME);
	rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, buffer, 0, KEY_READ, &hReg);
	if (rc != ERROR_SUCCESS) {
		sendheartbeat(2,"ERROR: Registry entries not found");
		return;
	}
	len = sizeof(DWORD);
	rc  = RegQueryValueEx(hReg, "processDelay", NULL, NULL, (unsigned char*)&eventlogdelay, &len);
	if (rc != ERROR_SUCCESS) {	eventlogdelay = 20;	}
	if((eventlogdelay < 10)||(eventlogdelay > 20000)) {
		eventlogdelay = 20;
	}
	len = sizeof(DWORD);
	rc  = RegQueryValueEx(hReg, "heartbeatFreq", NULL, NULL, (unsigned char*)&heartbeatfreq, &len);
	if (rc != ERROR_SUCCESS) { heartbeatfreq = 0; }
	if(heartbeatfreq < 0) {	heartbeatfreq = 10;	}

	len = sizeof(hbsvc)-1;
	rc  = RegQueryValueEx(hReg, "heartbeatSvc", NULL, NULL, (unsigned char*)hbsvc, &len);
	if(rc) { hbsvc[0] = (char)0; }
	if(!hbsvc[0]) { heartbeatfreq = 0; } /* disable heartbeat */
}


/*--------------------------------[ ntsl_run ]--------------------------------
 * Service event loop
 *----------------------------------------------------------------------------*/
void ntsl_run()
{
	int size,rc,hbcount;
	char buffer[64];
	HKEY hReg;

   	_snprintf0(buffer, sizeof(buffer), "SOFTWARE\\%s\\Nagios", DEV_NAME);
   	rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, buffer, 0, KEY_READ, &hReg);
   	if (rc != ERROR_SUCCESS) {
		ntsl_log_error("Unable to read registry key.");
		debugmode = 0;
	} else {
		size = sizeof(DWORD);
		rc  = RegQueryValueEx(hReg, "DEBUG", NULL, NULL, (unsigned char*)&debugmode, &size);
	}

	ntsl_log_info("Service started: loop delay=%ds", eventlogdelay);
	sendheartbeat(1,"Service starting");

	hbcount = 0;
	do
	{
		if(debugmode){ntsl_log_info("Scanning event logs...");}

		eventlog_check_events();

		if (!service_halting()) {
			if(debugmode) {	ntsl_log_info("Sleeping..."); }
			Sleep(eventlogdelay * 1000);
			if( heartbeatfreq > 0 ) {
				hbcount++;
				if(hbcount >= heartbeatfreq) {
					hbcount = 0;
					sendheartbeat(0,"Service running OK");
				}
			}
		}
	} while(!service_halting());
	sendheartbeat(2,"Service halting");

	ntsl_shutdown();

	ntsl_log_info(NTSL_INFO_SERVICE, "shutdown");
}

/*------------------------------[ ntsl_shutdown ]------------------------------
 *  Shutdown subsystems
 *----------------------------------------------------------------------------*/
void ntsl_shutdown()
{
	engine_shutdown();
	eventlog_shutdown();

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif
}

