/*-----------------------------------------------------------------------------
 *
 *  eventlog.c - Windows NT eventlog module
 *
 *    Copyright (c) 1998-2002, SaberNet.net - All rights reserved
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
 *  $Id: eventlog.c,v 1.13 2002/09/20 06:12:47 jason Exp $
 *
 *  Revision history:
 *    22-Jul-2002  JRR  Added support for user defined event logs
 *    14-Jun-2002  LWE  Fixed compiler warning.
 *    22-Apr-2002  FK   Fixed language independence and multiple message files
 *    15-Oct-1999  JRR  Fixed handling of events w/ missing descriptions
 *    06-Jul-1998  JRR  Module completed
 *
 *----------------------------------------------------------------------------*/

#include "windows.h"
#include <time.h>
#include <stdio.h>
#include "ntsl.h"
#include "eventlog.h"
#include "engine.h"
#include "service.h"
#include "list.h"

#include "filter.h"

extern int debugmode; /* from nsca.c */

/*-------------------------------[ static data ]------------------------------*/
#define REG_BUFFER_LEN				2048
#define EVENTLOG_BUFFER_LEN			511*1024 // Changed to support .NET
#define MAX_LOG_NAME_LEN			256
#define MAX_MSG_STRINGS				100	// FormatMessage(): %n = {1..99}
#define LAST_RUN_REG				"LastRun"

#define EVENTLOG_NO_FLAGS			0x0
#define EVENTLOG_INFORMATION_FLAG	0x1
#define EVENTLOG_WARNING_FLAG		0x2
#define EVENTLOG_ERROR_FLAG			0x4
#define EVENTLOG_AUDIT_SUCCESS_FLAG	0x8
#define EVENTLOG_AUDIT_FAILURE_FLAG	0x10
#define EVENTLOG_ALL_FLAGS			0x1f

#define EVENTLOG_DEFAULT_PRIORITY	9	// Default to user.alert


/*---------------------------[ private structures ]---------------------------*/
typedef struct eventlog_data_st
{
	int 	captureFlags;

	int 	informationPriority;
	int 	warningPriority;
	int 	errorPriority;
	int 	auditSuccessPriority;
	int 	auditFailurePriority;
	char	name[MAX_LOG_NAME_LEN];
} eventlog_data;

/*----------------------------[ private functions ]---------------------------*/
static char  *eventlog_lcase(char *s);
static int    eventlog_append_data(char *s, int len, char* data, BOOL chop);
static char **eventlog_strings_to_array(char *strings, int num);
static int    eventlog_read_events(eventlog_data* eventlog, uint32 *lastrun, uint32 *thisrun);
static int    eventlog_read_lastrun(uint32 *lastrun);
static int    eventlog_write_lastrun(uint32 *lastrun);
static int    eventlog_set_event_type(ntsl_event *event, int id);
static int    eventlog_set_event_priority(ntsl_event *event, int type, eventlog_data* eventlog);
static int    eventlog_set_user(ntsl_event *event, PSID sid, DWORD len);
static eventlog_data* eventlog_new_log(const char* name);



/*-------------------------------[ static data ]------------------------------*/
static List* eventlog_list = NULL;


/*-----------------------------[ eventlog_lcase ]-----------------------------
 * Convert string to lowercase
 *----------------------------------------------------------------------------*/
static char *eventlog_lcase(char *s)
{
	char *sp = s;

	while(*sp) { *sp = tolower(*sp); sp++; }
	return(s);
} 


/*--------------------------[ eventlog_append_data ]--------------------------
 * Appends up to n bytes of data to the end of the buffer. (Null terminating) 
 *
 *	Returns:
 *		success		0
 *		failure		-1 
 *----------------------------------------------------------------------------*/
static int eventlog_append_data(char *buffer, int n, char *data, BOOL chop)
{
	int	rc = -1;

	if ( (buffer != NULL) && (data != NULL) )
	{
		int i  = (int)strlen(buffer);
		int	nl = 0;

		if (chop)
			while(iswspace(*data)) data++;

		while (*data)
		{
			if (isprint(*data) || *data == '\n')
			{
				if (*data == '\n') 
					*data = ' '; 
				if (i < (n - 1))	
					buffer[i++] = *data;
			}
			data++;
		}
		buffer[i] = (char)0;
		rc = 0;
	}

	return(rc);
}

/*------------------------[ eventlog_strings_to_array ]------------------------
 * Converts a concatenation of null terminated strings to an array of strings.
 * (Null terminating)
 *----------------------------------------------------------------------------*/
static char **eventlog_strings_to_array(char *strings, int num)
{
	static char *array[MAX_MSG_STRINGS];
	int			 i;

	if (strings == NULL)
	{
		array[0] = NULL;
		return(array);
	}

	if (num > MAX_MSG_STRINGS)
		num = MAX_MSG_STRINGS;

	for(i=0; i<num; i++)
	{
		array[i] = strings;
		strings += strlen(strings) + 1;
	}
	array[i] = NULL;

	return(array);
}



/*-------------------------[ eventlog_set_event_type ]-------------------------
 * Sets the type for the given event.
 *----------------------------------------------------------------------------*/
static int eventlog_set_event_type(ntsl_event *event, int id)
{
	int   rc    = -1;
	char *eType = NULL;

	if (NULL != event)
	{
		switch (id)
		{
			case EVENTLOG_ERROR_TYPE:		eType = NTSL_EVENT_ERROR;	     break;
			case EVENTLOG_WARNING_TYPE:     eType = NTSL_EVENT_WARNING;      break;
			case EVENTLOG_INFORMATION_TYPE: eType = NTSL_EVENT_INFORMATION;  break;
			case EVENTLOG_AUDIT_SUCCESS:    eType = NTSL_EVENT_SUCCESS;      break;
			case EVENTLOG_AUDIT_FAILURE:    eType = NTSL_EVENT_FAILURE;      break;
		};
		
		if (eType != NULL)
		{
			strncpy0(event->etype,  eType, NTSL_SYS_LEN);	
			rc = 0;
		}
	}

	return(rc);
}





/*-------------------------[ eventlog_set_event_msg ]-------------------------
 * Retrieves the event message from the appropriate DLL.
 *
 *	Returns:
 *		success		0
 *		failure		-1 
 *----------------------------------------------------------------------------*/
static int eventlog_set_event_msg(ntsl_event *event, char *logType,
										uint32 id, char *strings, int numStrings)
{
	char	**array; 
	char	buffer[REG_BUFFER_LEN];
	char	dll[REG_BUFFER_LEN];
	uint32	bufsize = REG_BUFFER_LEN;
	uint32	bytes   = 0;
	uint32	regtype = 0;
	HANDLE	hlib;
	HKEY	hkey;
	uint32  sequencenumber = 0;
	char    singeldll[REG_BUFFER_LEN];
	BOOL    handled = false;

	/* check paramaters */
	if ( (event == NULL) || (logType == NULL) || (strings == NULL) )
		return(-1);

	array = eventlog_strings_to_array(strings, numStrings);
	event->id = id;
	event->id &= 0x0000FFFF;
	

	/* check strings array */
	if ( (numStrings) && (array[0] == NULL) )
		return(-1);

	/* build registry path */
	_snprintf0(buffer, REG_BUFFER_LEN, 
		"SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s", logType,
		 event->source);

	/* load message text */
	if (RegOpenKey(HKEY_LOCAL_MACHINE, buffer, &hkey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hkey, "EventMessageFile", 0, &regtype, 
				(unsigned char*)buffer, &bufsize) == ERROR_SUCCESS)
		{
			if (ExpandEnvironmentStrings(buffer, dll, REG_BUFFER_LEN) > 0)
			{
				/* Parse into different ones if needed */
				while(!eventlog_parse_libs(dll,singeldll,sequencenumber))
				{
					if ((hlib = LoadLibraryEx(singeldll, NULL, DONT_RESOLVE_DLL_REFERENCES)) != NULL)
					{
						LPCVOID msg  = NULL;
					
						bytes = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
							 		FORMAT_MESSAGE_FROM_HMODULE |
									FORMAT_MESSAGE_ARGUMENT_ARRAY | 60, hlib, id, 
									0, (LPTSTR)&msg, NTSL_EVENT_LEN, array);
	
						if (bytes > 0)
						{
							handled = true;
							event->msg[0] = (char)0;
							eventlog_append_data(event->msg, NTSL_EVENT_LEN, (char *)msg, TRUE);
							LocalFree((HANDLE)msg);
						}

						FreeLibrary(hlib);
					}
					sequencenumber++;
				}
			}
		}
		RegCloseKey(hkey);
	}

	if (handled)
	{
			; // handled above
	}
	else if (numStrings > 0)
	{
		int i;
						
		for (i=0; i<numStrings; i++)
		{
			eventlog_append_data(event->msg, NTSL_EVENT_LEN, array[i], FALSE);
			eventlog_append_data(event->msg, NTSL_EVENT_LEN, " ", FALSE);
		}
	}
	else
	{
		eventlog_append_data(event->msg, NTSL_EVENT_LEN, "No description available", FALSE);
	}

	return(0);
}

/*--------------------------[ eventlog_read_events ]--------------------------
 * Read messages from the event log 
 *
 *	Returns:
 *		success		(0)
 *		error		(-1)	
 *----------------------------------------------------------------------------*/
static int eventlog_read_events(eventlog_data* eventlog, uint32 *lastrun,
	uint32 *thisrun)
{
    char                buffer[EVENTLOG_BUFFER_LEN];
    int                 useTimeStamp = 1;
    uint32              bytes;
    uint32              next;
    EVENTLOGRECORD      *record = (EVENTLOGRECORD *)&buffer;
    HANDLE              hLog;
	int					status;
			ntsl_event	*eventp;
			char		*source ;
			char		*computer;
			char		*strings;
			time_t       t;
			struct tm   time;
			
	if ( (lastrun == NULL) || (thisrun == NULL) || (buffer == NULL) )
		return(-1);

    if ((hLog = OpenEventLog(NULL, eventlog->name)) == NULL) 
	{
		ntsl_log_error(NTSL_ERROR_EVENT_LOG_ACCESS, eventlog->name);
		return(-1);
	}

	if(debugmode) {
		ntsl_log_info("Reading %s event log...\n", eventlog->name);
	}



	while ((status=ReadEventLog(hLog, EVENTLOG_FORWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
		0, record, EVENTLOG_BUFFER_LEN, &bytes, &next)))   
	{
		// check for service shutdown
		if (service_halting())
			return(-1);

	     while (bytes > 0) 
		 {  
			source   = (LPSTR) ((LPBYTE) record + sizeof(EVENTLOGRECORD));			
			computer = source + strlen(source) + 1;
			strings  = (LPSTR) ((LPBYTE) record + record->StringOffset);
			t        = record->TimeGenerated;
			memset(&time,0,sizeof(struct tm));
			localtime_s(&time,&t);		

			/* If this event happened between the last time we ran, and the current
			   time, and it is one we are interested in, then fill in an ntsl_event
			   structure, and pass it to the engine to log. */
			if ( (record->TimeWritten >  (uint32)*lastrun) && 
	             (record->TimeWritten <= (uint32)*thisrun) &&
				 strcmp(source,APP_NAME)) /* ignore anything WE logged */
			{
				if ( (eventp = malloc0(sizeof(ntsl_event))) == NULL)
				{
					ntsl_log_error(NTSL_ERROR_EVENT_MALLOC);
					return(-1);
				}	
				memset(eventp,0,sizeof(ntsl_event));
				
				eventp->msg[0] = 0;
				strftime(eventp->date, NTSL_DATE_LEN, "%b %d %H:%M:%S", &time);
				if (eventp->date[4] == '0') // Unix style formatting
					eventp->date[4] = ' ';
				strncpy0(eventp->host, eventlog_lcase(computer), NTSL_SYS_LEN);
//				strncpy0(eventp->source, eventlog_lcase(source), NTSL_SYS_LEN);
				strncpy0(eventp->source, source, NTSL_SYS_LEN);
				eventlog_set_event_type(eventp, record->EventType);
				strncpy0(eventp->eventlog, eventlog->name, NTSL_SYS_LEN);
				eventp->status = 4;
				eventp->service[0] = (char)0;

				eventlog_set_event_msg(eventp, eventlog->name, record->EventID, strings, record->NumStrings);
				eventlog_set_user(eventp, ((LPBYTE) record + record->UserSidOffset), record->UserSidLength);
 			
				if( check_filter( record, eventp ) ) {		
					if(!eventp->service[0] || (eventp->status>3)) {
						if(debugmode) {
							ntsl_log_info("Caught an event to skip (Src=%s).\nMatching filter was '%s'",source,eventp->filter);
						}
					} else {
						engine_process_event(eventp);
						if(debugmode) {
							ntsl_log_info("Caught an event and sent it on (Src=%s).\nMatching filter was '%s'",source,eventp->filter);
						}						
					}
				} else {
					if(debugmode) {
						ntsl_log_info("Caught an event, but it didnt match any filters (Src=%s)", source);
					}
				}
				free(eventp);
				eventp = NULL;
			}
			
			bytes -= record->Length; 
			record = (EVENTLOGRECORD *) ((LPBYTE) record + record->Length); 
		}
 
		record = (EVENTLOGRECORD *)&buffer; 
	} 

	if ( (status == 0) && (GetLastError() == ERROR_INSUFFICIENT_BUFFER) )
	{
		ntsl_log_error(NTSL_ERROR_EVENT_READ_BUF, EVENTLOG_BUFFER_LEN, next);
	}
 
    CloseEventLog(hLog); 

	return(0);
}


/*-------------------------[ eventlog_read_lastrun ]---------------------------
 *  Reads last run times
 *
 *	Returns:
 *		success		(0)
 *		error		(-1)	
 *----------------------------------------------------------------------------*/
static int eventlog_read_lastrun(uint32 *lastrun)
{
    char        buffer[REG_BUFFER_LEN];
    HKEY        hReg;
	int32		size = sizeof(*lastrun);
    int32       rc, rv;

	if (lastrun == NULL)
		return(-1);

    _snprintf0(buffer, REG_BUFFER_LEN, "SOFTWARE\\%s\\Nagios", DEV_NAME);
    rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, buffer, 0, KEY_READ, &hReg);
    if (rc != ERROR_SUCCESS)
            return(-1);

    rv  = RegQueryValueEx(hReg, LAST_RUN_REG, NULL, NULL, (unsigned char*)lastrun, &size);
    RegCloseKey(hReg);

    if ( (rv != ERROR_SUCCESS) || (*lastrun <0) )
	{
		*lastrun = 0;
	}	

	return(0);
}


/*--------------------------[ eventlog_write_lastrun ]-----------------------
 *  Writes last run time
 *
 *	Returns:
 *		success		(0)
 *		error		(-1)	
 *----------------------------------------------------------------------------*/
static int eventlog_write_lastrun(uint32 *lastrun)
{
    char        buffer[REG_BUFFER_LEN];
    HKEY        hReg;
	int32		size = sizeof(*lastrun);
    int32       rc, rv;

	if (lastrun == NULL)
		return(-1);

    _snprintf0(buffer, REG_BUFFER_LEN, "SOFTWARE\\%s\\Nagios", DEV_NAME);
    rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, buffer, 0, KEY_SET_VALUE, &hReg);
    if (rc != ERROR_SUCCESS)
            return(-1);

    rv  = RegSetValueEx(hReg, LAST_RUN_REG, 0, REG_DWORD, (unsigned char*)lastrun, size);
    RegCloseKey(hReg);

    if (rv != ERROR_SUCCESS)
		ntsl_die(NTSL_ERROR_TIME_DATA_WRITE, LAST_RUN_REG);

	return(0);
}


/*--------------------------[ eventlog_check_events ]--------------------------
 * Locate recent event entries to be processed
 *----------------------------------------------------------------------------*/
void eventlog_check_events()
{
	int		running = 1;	
	uint32 	lastrun;
	uint32 	thisrun;
	time_t t;

	time(&t); thisrun = (uint32)t;

	if (eventlog_read_lastrun(&lastrun) == 0)
	{
		if (lastrun > 0)
		{
			ListIterator* i = list_iterator(eventlog_list);

			if (i)
			{
				eventlog_data* log = list_first(i);

				while(log != NULL)
				{
					if (log->captureFlags != EVENTLOG_NO_FLAGS)
						eventlog_read_events(log, &lastrun, &thisrun);

					if (running)
						log = list_next(i);
					else
						log = NULL;
				}

				i = list_iterator_delete(i);
			}
		}

		if (running)
			eventlog_write_lastrun(&thisrun);
	}
}


/*----------------------------[ eventlog_read_reg ]----------------------------
 * Read eventlog registry settings 
 *----------------------------------------------------------------------------*/
static void eventlog_read_reg(eventlog_data* eventlog)
{
//    char        buffer[REG_BUFFER_LEN];
//	int			val;
 //   HKEY        hReg;
//	int32		size = sizeof(val);
 //   int32       rc, rv;

	if (eventlog)
	{

		ntsl_log_info("Setting up monitoring of event log %s", eventlog->name);


		
		// Set up the structure with clean defaults.
		eventlog->captureFlags = EVENTLOG_ALL_FLAGS; /* check all logs */

	}
}


int eventlog_init()
{
	eventlog_list = list_new();

	if (eventlog_list != NULL)
	{
    	char        buffer[REG_BUFFER_LEN];
    	HKEY        hReg;
    	int32       rc;

//   	_snprintf0(buffer, REG_BUFFER_LEN, "SOFTWARE\\%s\\Nagios", DEV_NAME); 
    	_snprintf0(buffer, REG_BUFFER_LEN, "SYSTEM\\CurrentControlSet\\Services\\EventLog");
    	rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, buffer, 0, KEY_READ, &hReg);
    	if (rc == ERROR_SUCCESS)
		{
			int32		i = 0;
	
			while (RegEnumKey(hReg, i++, buffer, REG_BUFFER_LEN)
					 == ERROR_SUCCESS)
			{
				list_add(eventlog_list, eventlog_new_log(buffer));
			}

			RegCloseKey(hReg);
			if(debugmode){ntsl_log_info("Registered event logs.");}
		} else {
			ntsl_log_error("Unable to read registry event logs!");
		}
		
	}

	return(eventlog_list != NULL);
}

void eventlog_shutdown()
{
	eventlog_list = list_delete(eventlog_list);	// free list contents
}	

/*--------------------------[ eventlog_parse_libs ]--------------------------
 * Parse different libraries from string 
 *
 *	Returns:
 *		success		(0)
 *		error		(-1)	
 *----------------------------------------------------------------------------*/
int eventlog_parse_libs(char *dllIn, char *dllOut, uint32 sequence)
{
	int start = 0;
	int target = 0;
	
	while (sequence > 0 && dllIn[start] != '\0')
	{
		if (dllIn[start] == ';')
			sequence--;
		start++;
	}

	if (dllIn[start] == '\0')
		return -1;

	while (dllIn[start] != '\0' && dllIn[start] != ';')
	{
		dllOut[target] = dllIn[start];
		target++;
		start++;
	}
	dllOut[target] = '\0';
	return 0;
}

/*----------------------------[ eventlog_new_log ]----------------------------
 * Obtains the user name associated with a event 
 *
 *	Returns:
 *		success		(0)
 *		error		(-1)	
 *----------------------------------------------------------------------------*/
static int eventlog_set_user(ntsl_event *event, PSID sid, DWORD len)
{
	DWORD			us = NTSL_SYS_LEN - 1;
	DWORD			ds = NTSL_SYS_LEN - 1;
	SID_NAME_USE	pe;

	if (event == NULL) return -1;

	event->user[0] = 0;
	event->domain[0] = 0;

	if ( (len > 0) && (LookupAccountSid(NULL, sid, event->user, &us, event->domain, &ds, &pe)) )
	{
		return 0;
	}

	return -1;

}

/*----------------------------[ eventlog_new_log ]----------------------------
 * Mallocs new eventlog entry 
 *
 *	Returns:
 *		success		(!NULL)
 *		error		(NULL)	
 *----------------------------------------------------------------------------*/
static eventlog_data* eventlog_new_log(const char* name)
{
	eventlog_data*	data = NULL;

	if (name != NULL)
	{
		data = malloc(sizeof(eventlog_data));

		if (data != NULL)
		{
			strncpy0(data->name, name, MAX_LOG_NAME_LEN);
			eventlog_read_reg(data);
			
		}
	}

	return data;
}
	
