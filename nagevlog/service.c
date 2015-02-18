/*-----------------------------------------------------------------------------
 *
 *  service.c - NT Service module
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
 *  $Id: service.c,v 1.12 2002/08/24 00:01:30 jason Exp $
 *
 *  Comments:
 *	  This module is based on sample code by Craig Link - Microsoft 
 *    Developer Support
 *
 *  Revision history:
 *    02-Aug-98  JRR  Module completed
 *
 *----------------------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <tchar.h>
#include "ntsl.h"
#include "service.h"

LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize );


  
/*------------------------------[ private data ]------------------------------*/
static SERVICE_STATUS			service_status;       
static SERVICE_STATUS_HANDLE	service_status_handle;
static int						service_debug_mode = 0;
static uint32					service_error;
static char						service_error_msg[NTSL_SYS_LEN];
static HANDLE					service_stop_event;

/*----------------------------[ private functions ]---------------------------*/
static void WINAPI service_main(int argc, char **argv);
static void WINAPI service_ctrl(uint32 ctrlCode);
static bool        service_report_status(uint32 currentState, uint32 exitCode, 
								  uint32 waitHint);
static void        service_install();
static void        service_remove();
static void	       service_debug(int argc, char **argv);
static bool WINAPI service_control_handler(uint32 ctrlType);
static void        service_addEventSource(const char* path);

/*----------------------------------[ main ]----------------------------------
 *  Service entrypoint.
 *
 *  Parameters:
 *		argc  -	 number of command line arguments
 *		argv  -  array of command line arguments
 *
 *  Return value:
 *		none
 *----------------------------------------------------------------------------*/
void /*_CRTAPI1*/ main(int argc, char **argv)
{
    SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { TEXT(SERVICE_NAME), (LPSERVICE_MAIN_FUNCTION)service_main },
        { NULL, NULL }
    };

    if ( (argc > 1) && ((*argv[1] == '-') || (*argv[1] == '/')) )
    {
        if ( _stricmp( "install", argv[1]+1 ) == 0 )
        {
            service_install();
        }
        else if ( _stricmp( "remove", argv[1]+1 ) == 0 )
        {
            service_remove();
        }
#ifdef _DEBUG
        else if ( _stricmp( "debug", argv[1]+1 ) == 0 )
        {
            service_debug_mode = 1;
            service_debug(argc, argv);
        }
#endif
        else
        {
            goto dispatch;
        }
        exit(0);
    }

    // if it doesn't match any of the above parameters
    // the service control manager may be starting the service
    // so we must call StartServiceCtrlDispatcher
    dispatch:
		printf( "%s Version %s.%s\n%s\n\n", APP_NAME, VERSION_MAJOR, VERSION_MINOR, COPYRIGHT);
        printf( "%s -install          to install the service\n", APP_NAME );
        printf( "%s -remove           to remove the service\n", APP_NAME );
#ifdef _DEBUG
		printf( "%s -debug <params>   to run as a console app for debugging\n", APP_NAME );
#endif
        printf( "\nStartServiceCtrlDispatcher being called.\n" );
        printf( "This may take several seconds.  Please wait.\n" );

        if (!StartServiceCtrlDispatcher(dispatchTable))
            ntsl_log_error(NTSL_ERROR_SERVICE_DISPATCH, GetLastError());
}


/*------------------------------[ service_main ]------------------------------
 *  Calls service initialization routines.
 *
 *  Parameters:
 *		argc  -	 number of command line arguments
 *		argv  -  array of command line arguments
 *
 *  Return value:
 *		none
 *----------------------------------------------------------------------------*/
static void WINAPI service_main(int argc, char **argv)
{
    service_status_handle = RegisterServiceCtrlHandler(SERVICE_NAME, service_ctrl);

    if (!service_status_handle)
        goto cleanup;

    service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    service_status.dwServiceSpecificExitCode = 0;


    // report the status to the service control manager.
    if (!service_report_status(SERVICE_START_PENDING, NO_ERROR, 3000))                 
        goto cleanup;

    service_start(argc, argv);

	cleanup:
		// try to report the stopped status to the service control manager.
		if (service_status_handle)
			(VOID)service_report_status(SERVICE_STOPPED, service_error, 0);

    return;
}



/*------------------------------[ service_ctrl ]------------------------------
 *  Called by the SCM whenever ControlService() is called for this service
 *
 *  Parameters:
 *		ctrlCode - type of control requested
 *
 *  Return value:
 *		none
 *----------------------------------------------------------------------------*/
void WINAPI service_ctrl(uint32 ctrlCode)
{
    switch(ctrlCode)
    {
        // stop the service.
        //
        // SERVICE_STOP_PENDING should be reported before
        // setting the Stop Event - hServerStopEvent - in
        // service_stop().  This avoids a race condition
        // which may result in a 1053 - The Service did not respond...
        // error.
        case SERVICE_CONTROL_STOP:
            service_report_status(SERVICE_STOP_PENDING, NO_ERROR, 0);
            service_stop();
            return;

        // Update the service status.
        //
        case SERVICE_CONTROL_INTERROGATE:
            break;

        // invalid control code
        //
        default:
            break;
    }

    service_report_status(service_status.dwCurrentState, NO_ERROR, 0);
}


/*-----------------------------[ service_halting ]-----------------------------
 *  Service status
 *
 *  Returns:	
 *		running		0
 *		halting		1
 *----------------------------------------------------------------------------*/
int service_halting()
{
	return (WaitForSingleObject(service_stop_event, NTSL_LOOP_WAIT) == WAIT_OBJECT_0);
}



/*--------------------------[ service_report_status ]--------------------------
 *  Sets the current status and reports it to the Service Control Manager
 *
 *  Parameters:
 *		currentState	-  the state of the service
 *		exitCode		-  error code to report
 *		waitHint		-  worst case estimate to next checkpoint
 *
 *  Return value:
 *		true			-  success
 *		false			-  failure
 *----------------------------------------------------------------------------*/
static bool service_report_status(uint32 currentState, uint32 exitCode, 
								  uint32 waitHint)
{
    static uint32 checkPoint = 1;
    bool		  rc         = true;

    if (!service_debug_mode) // when debugging we don't report to the SCM
    {
        if (currentState == SERVICE_START_PENDING)
            service_status.dwControlsAccepted = 0;
        else
            service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

        service_status.dwCurrentState  = currentState;
        service_status.dwWin32ExitCode = exitCode;
        service_status.dwWaitHint      = waitHint;

        if ( ( currentState == SERVICE_RUNNING ) ||
             ( currentState == SERVICE_STOPPED ) )
            service_status.dwCheckPoint = 0;
        else
            service_status.dwCheckPoint = checkPoint++;

        // report the status of the service to the service control manager.
        if (!(rc = SetServiceStatus( service_status_handle, &service_status))) 
            ntsl_log_error(NTSL_ERROR_SERVICE_STATUS, GetLastError());
    }
    return(rc);
}



/*-----------------------------[ service_install ]-----------------------------
 *  Installs the service.
 *----------------------------------------------------------------------------*/
static void service_install()
{
    SC_HANDLE   service;
    SC_HANDLE   manager;
	char		path[NTSL_PATH_LEN];

    if (GetModuleFileName(NULL, path , NTSL_PATH_LEN - 1) == 0)
    {
        _tprintf(TEXT("Unable to install %s - %s\n"), TEXT(SERVICE_NAME),
			GetLastErrorText(service_error_msg, NTSL_SYS_LEN));
        return;
    }

	// quote path
//	path[0] = '"';
//	strcat(path, "\"");

	service_addEventSource(path);

    manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (manager)
    {
        service = CreateService(
            manager,                    // SCManager database
            TEXT(SERVICE_NAME),         // name of service
            TEXT(SERVICE_NAME),			// name to display
            SERVICE_ALL_ACCESS,         // desired access
            SERVICE_WIN32_OWN_PROCESS,  // service type
            SERVICE_AUTO_START,			// start type
            SERVICE_ERROR_NORMAL,       // error control type
            path,                       // service's binary
            NULL,                       // no load ordering group
            NULL,                       // no tag identifier
            "",                         // dependencies
            NULL,                       // LocalSystem account
            NULL);                      // no password

        if (service)
        {
            _tprintf(TEXT("%s installed.\n"), TEXT(SERVICE_NAME) );


            CloseServiceHandle(service);
        }
        else
        {
            _tprintf(TEXT("CreateService failed - %s\n"), 
						GetLastErrorText(service_error_msg, NTSL_SYS_LEN));
        }

        CloseServiceHandle(manager);
    }
    else
        _tprintf(TEXT("OpenSCManager failed - %s\n"), 
				GetLastErrorText(service_error_msg, NTSL_SYS_LEN));
}



/*-----------------------------[ service_remove ]-----------------------------
 *  Stops and removes the service
 *----------------------------------------------------------------------------*/
static void service_remove()
{
    SC_HANDLE   service;
    SC_HANDLE   manager;

    manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (manager)
    {
        service = OpenService(manager, TEXT(SERVICE_NAME), SERVICE_ALL_ACCESS);

        if (service)
        {
            // try to stop the service
            if ( ControlService( service, SERVICE_CONTROL_STOP, &service_status ) )
            {
                _tprintf(TEXT("Stopping %s."), TEXT(SERVICE_NAME));
                Sleep(1000);

                while(QueryServiceStatus(service, &service_status))
                {
                    if (service_status.dwCurrentState == SERVICE_STOP_PENDING)
                    {
                        _tprintf(TEXT("."));
                        Sleep( 1000 );
                    }
                    else
                        break;
                }

                if (service_status.dwCurrentState == SERVICE_STOPPED)
                    _tprintf(TEXT("\n%s stopped.\n"), TEXT(SERVICE_NAME));
                else
                    _tprintf(TEXT("\n%s failed to stop.\n"), TEXT(SERVICE_NAME));
            }

            // now remove the service
            if( DeleteService(service) )
                _tprintf(TEXT("%s removed.\n"), TEXT(SERVICE_NAME));
            else
                _tprintf(TEXT("DeleteService failed - %s\n"), 
					GetLastErrorText(service_error_msg, NTSL_SYS_LEN));


            CloseServiceHandle(service);
        }
        else
            _tprintf(TEXT("OpenService failed - %s\n"), 
				GetLastErrorText(service_error_msg, NTSL_SYS_LEN));

        CloseServiceHandle(manager);
    }
    else
        _tprintf(TEXT("OpenSCManager failed - %s\n"), 
			GetLastErrorText(service_error_msg, NTSL_SYS_LEN));
}



/*------------------------------[ service_debug ]------------------------------
 *  Runs the service as a console application
 *
 *  Parameters:
 *		argc  -  number of command line arguments
 *		argv  -  array of command line arguments
 *----------------------------------------------------------------------------*/
static void service_debug(int argc, char **argv)
{
    _tprintf(TEXT("Debugging %s.\n"), TEXT(SERVICE_NAME));

    SetConsoleCtrlHandler( service_control_handler, TRUE );
    service_start(argc, argv);
}


/*-------------------------[ service_control_handler ]-------------------------
 *  Handles console control events.
 *
 *  Parameters:
 *		crtlType  -  type of control event
 *
 *  Return value:
 *		true  - handled
 *      false - not handled
 *----------------------------------------------------------------------------*/
static bool WINAPI service_control_handler(uint32 ctrlType)
{
    switch(ctrlType)
    {
        case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
        case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
            _tprintf(TEXT("Stopping %s.\n"), TEXT(SERVICE_NAME));
            service_stop();
            return true;
            break;
    }
    return false;
}


/*------------------------------[ service_start ]------------------------------
 * Starts and runs the service
 *----------------------------------------------------------------------------*/
void service_start(int argc, char **argv)
{
    // report the status to the service control manager.
    if (!service_report_status(SERVICE_START_PENDING, NO_ERROR, 3000))
		return;

    // create the event object. The control handler function signals
    // this event when it receives the "stop" control code.
    service_stop_event = CreateEvent(NULL, true, false, NULL);

    if (NULL == service_stop_event)
        return;

    // report the status to the service control manager.
    if (!service_report_status(SERVICE_START_PENDING, NO_ERROR, 3000))
		return;

	ntsl_init();
 
    // report the status to the service control manager.
    if (!service_report_status(SERVICE_RUNNING, NO_ERROR, 0))
		return;

	ntsl_run();
}


/*------------------------------[ service_stop ]------------------------------
 * Stops the service.  
 *
 * NOTE: If this service will take longer than 3 seconds,
 * spawn a thread to execute the stop code and return.  
 * Otherwise the SCM will think the service has stopped responding.
 *----------------------------------------------------------------------------*/
void service_stop()
{
    if (service_stop_event)
        SetEvent(service_stop_event);
}


LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize )
{
    DWORD dwRet;
    LPTSTR lpszTemp = NULL;

    dwRet = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
						   FORMAT_MESSAGE_ARGUMENT_ARRAY,
                           NULL,
                           GetLastError(),
                           LANG_NEUTRAL,
                           (LPTSTR)&lpszTemp,
                           0,
                           NULL );

    // supplied buffer is not long enough
    if ( !dwRet || ( (long)dwSize < (long)dwRet+14 ) )
        lpszBuf[0] = TEXT('\0');
    else
    {
        lpszTemp[lstrlen(lpszTemp)-2] = TEXT('\0');  //remove cr and newline character
        _stprintf( lpszBuf, TEXT("%s (0x%x)"), lpszTemp, GetLastError() );
    }

    if ( lpszTemp )
        LocalFree((HLOCAL) lpszTemp );

    return lpszBuf;
}

void service_addEventSource(const char* path)
{
    HKEY hk; 
    DWORD dwData; 
	char  key[256];
 
	if (path == NULL) return;

    // Add your source name as a subkey under the Application 
    // key in the EventLog registry key. 
 
	_snprintf(key, 256, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s", APP_NAME);
    if (RegCreateKey(HKEY_LOCAL_MACHINE, key, &hk)) 
        ntsl_die("Could not create the registry key."); 
   
 
    // Add the name to the EventMessageFile subkey. 
 
    if (RegSetValueEx(hk,             // subkey handle 
            "EventMessageFile",       // value name 
            0,                        // must be zero 
            REG_EXPAND_SZ,            // value type 
            (LPBYTE) path,           // pointer to value data 
            (int)strlen(path) + 1))       // length of value data 
        ntsl_die("Could not set the event message file."); 
 
    // Set the supported event types in the TypesSupported subkey. 
 
    dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | 
        EVENTLOG_INFORMATION_TYPE; 
 
    if (RegSetValueEx(hk,      // subkey handle 
            "TypesSupported",  // value name 
            0,                 // must be zero 
            REG_DWORD,         // value type 
            (LPBYTE) &dwData,  // pointer to value data 
            sizeof(DWORD)))    // length of value data 
        ntsl_die("Could not set the supported types."); 
 
    RegCloseKey(hk); 
} 
