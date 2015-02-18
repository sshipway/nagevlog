/* definition for all filter checking functions */

#define _CRT_SECURE_NO_DEPRECATE

#include "event.h"
#include "string.h"
#include <winnt.h>
#include "ntsl.h"

#include "regexp.h"

#define EVENTLOG_NO_FLAGS			0x0
#define EVENTLOG_INFORMATION_FLAG	0x4
#define EVENTLOG_WARNING_FLAG		0x2
#define EVENTLOG_ERROR_FLAG			0x1
#define EVENTLOG_AUDIT_SUCCESS_FLAG	0x8
#define EVENTLOG_AUDIT_FAILURE_FLAG	0x10
#define EVENTLOG_ALL_FLAGS			0x1f

#define REG_BUFFER_LEN 2048

#define NOT_SOURCE 0x1
#define NOT_MATCH  0x2
#define NOT_EVENT  0x4

#define F_DESCLEN	NTSL_SYS_LEN
#define F_EVENTLOGLEN	NTSL_SYS_LEN
#define F_MAXIDS	64
#define	F_MATCHLEN	512
#define F_EVENTIDLEN NTSL_EVENT_LEN
#define F_SOURCELEN	NTSL_SYS_LEN
#define F_SERVICELEN NTSL_SVC_LEN

#define XOR(a,b) (((a)&&!(b))||((b)&&!(a)))

typedef struct s_filter {
	char desc[F_DESCLEN];
	/* flags */
	int typeflags;
	/* match criteria */
	char eventlog[F_EVENTLOGLEN]; 
	int eventlognum;
	unsigned short id[F_MAXIDS]; /* list of integers */
	int  numids;
	char match[F_MATCHLEN];
	struct regexp *regex;
	char source[F_SOURCELEN];
	int notflags;
	/* result */
	int status;
	char service[F_SERVICELEN];
	/* link */
	struct s_filter *next;
}  filter;

extern int debugmode; /* from nsca.c */

filter *filter_head = (filter *)0;

void read_filters() {
	filter *n, *l;
	int rc;
	int maxfilter, len,f,c;
	char buffer[REG_BUFFER_LEN];
	HKEY	hReg;
	char sitem[REG_BUFFER_LEN];
	int dwitem;
	char *t;
	int typeflags;
	unsigned short idno;
	int fc = 0;

	if( filter_head ) { return; } /* already read in the filters */

	if(debugmode) {	ntsl_log_info("Reading in filter configuration");}

	filter_head = l = n = (filter *)0;

	/* read the registry stuff in here! */
	_snprintf0(buffer, REG_BUFFER_LEN, "SOFTWARE\\%s\\Nagios", DEV_NAME);
	rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, buffer, 0, KEY_READ, &hReg);
	if (rc != ERROR_SUCCESS) { 
		ntsl_log_error("Can't open registry! SOFTWARE\\%s\\Nagios",DEV_NAME);
		return; }

	maxfilter = -1;
	len = sizeof(DWORD);
	rc  = RegQueryValueEx(hReg, "maxFilter", NULL, NULL, (unsigned char *) &maxfilter, &len);
	if (rc != ERROR_SUCCESS) { 
		ntsl_log_error("Can't find a maxfilter value!  Guessing.....");
		maxfilter = 100; }
	RegCloseKey(hReg);
	for( f = 0; f <= maxfilter; f++ ) {
		_snprintf0(buffer, REG_BUFFER_LEN, "SOFTWARE\\%s\\Nagios\\Filter%d", DEV_NAME,f);
		rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, buffer, 0, KEY_READ, &hReg);
		if(rc != ERROR_SUCCESS ) { continue; }

		fc += 1;
		/* new filter : make and hook in */
		n = (filter *) malloc(sizeof(filter));
		/* log an error if !n */
		if( !n ) {
			/* log an error if !n */
			ntsl_log_error(NTSL_ERROR_EVENT_MALLOC);
			break;
		}
		if(l) {	l->next = n; } else { filter_head = n; }
		l = n; /* set 'last' pointer */
		memset( n, 0, sizeof(filter));
		n->next = (struct s_filter *)0;
		len = sizeof(DWORD);
		rc  = RegQueryValueEx(hReg, "status", NULL, NULL, (unsigned char *)&dwitem, &len);
		if(rc == ERROR_SUCCESS) { n->status = dwitem;}
		len = F_SERVICELEN; sitem[0] = (char)0;
		rc  = RegQueryValueEx(hReg, "serviceName", NULL, NULL, sitem, &len);
		if(rc == ERROR_SUCCESS) { strncpy(n->service,sitem,F_SERVICELEN-1);}
		len = F_DESCLEN;sitem[0] = (char)0;
		rc  = RegQueryValueEx(hReg, "filterDesc", NULL, NULL, sitem, &len);
		if(rc == ERROR_SUCCESS) { strncpy(n->desc,sitem,F_DESCLEN-1);}
		len = F_SOURCELEN;sitem[0] = (char)0;
		rc  = RegQueryValueEx(hReg, "source", NULL, NULL, sitem, &len);
		if(rc == ERROR_SUCCESS) { strncpy(n->source,sitem,F_SOURCELEN-1);}
		len = F_MATCHLEN;sitem[0] = (char)0;
		rc  = RegQueryValueEx(hReg, "matchString", NULL, NULL, sitem, &len);
		if(rc == ERROR_SUCCESS) { 
			strncpy(n->match,sitem,F_MATCHLEN-1);
			if(sitem[0]) { 
				n->regex = regcomp(sitem); 
				if(!n->regex) {
					ntsl_log_error("Invalid regular expression in matchstring");
				}
			}

		}
		len = F_EVENTIDLEN;sitem[0] = (char)0;
		rc  = RegQueryValueEx(hReg, "ID", NULL, NULL, sitem, &len);
		c = 0;
		for( t = strtok(sitem,","); t; t = strtok(NULL,",")) {
			if( !*t ) { continue; }
			idno = atoi(t);
			if( (idno >= 0) && (idno < 65536)) {
				n->id[c] = idno; c++; if(c==F_MAXIDS) {break;}
			}
		}
		n->numids = c;
		/* now the flags */
		typeflags = 0;
		len = sizeof(DWORD);
		rc  = RegQueryValueEx(hReg, "Error", NULL, NULL, (unsigned char *)&dwitem, &len);
		if((rc == ERROR_SUCCESS)&&(dwitem)) { typeflags |= EVENTLOG_ERROR_FLAG;}
		len = sizeof(DWORD);
		rc  = RegQueryValueEx(hReg, "Warning", NULL, NULL, (unsigned char *)&dwitem, &len);
		if((rc == ERROR_SUCCESS)&&(dwitem)) { typeflags |= EVENTLOG_WARNING_FLAG;}
		len = sizeof(DWORD);
		rc  = RegQueryValueEx(hReg, "Information", NULL, NULL, (unsigned char *)&dwitem, &len);
		if((rc == ERROR_SUCCESS)&&(dwitem)) { typeflags |= EVENTLOG_INFORMATION_FLAG;}
		len = sizeof(DWORD);
		rc  = RegQueryValueEx(hReg, "Audit Success", NULL, NULL,(unsigned char *) &dwitem, &len);
		if((rc == ERROR_SUCCESS)&&(dwitem)) { typeflags |= EVENTLOG_AUDIT_SUCCESS_FLAG;}
		len = sizeof(DWORD);
		rc  = RegQueryValueEx(hReg, "Audit Failure", NULL, NULL, (unsigned char *)&dwitem, &len);
		if((rc == ERROR_SUCCESS)&&(dwitem)) { typeflags |= EVENTLOG_AUDIT_FAILURE_FLAG;}
		n->typeflags = typeflags;
		/* the notflags */
		typeflags = 0;
		len = sizeof(DWORD);
		rc  = RegQueryValueEx(hReg, "notID", NULL, NULL, (unsigned char *)&dwitem, &len);
		if((rc == ERROR_SUCCESS)&&(dwitem)) { typeflags |= NOT_EVENT;}
		len = sizeof(DWORD);
		rc  = RegQueryValueEx(hReg, "notMatch", NULL, NULL, (unsigned char *)&dwitem, &len);
		if((rc == ERROR_SUCCESS)&&(dwitem)) { typeflags |= NOT_MATCH;}
		len = sizeof(DWORD);
		rc  = RegQueryValueEx(hReg, "notSource", NULL, NULL, (unsigned char *)&dwitem, &len);
		if((rc == ERROR_SUCCESS)&&(dwitem)) { typeflags |= NOT_SOURCE;}
		n->notflags = typeflags;
		/* and the event log name */
		len = sizeof(DWORD); dwitem = 0;
		rc  = RegQueryValueEx(hReg, "eventLog", NULL, NULL, (unsigned char *)&dwitem, &len);
		if(rc == ERROR_SUCCESS) { n->eventlognum = dwitem; }
		if(dwitem) {
			len = 64; sitem[0] = (char)0;
			rc  = RegQueryValueEx(hReg, "eventLogName", NULL, NULL, sitem, &len);
			if(rc == ERROR_SUCCESS) {strncpy(n->eventlog,sitem,64); }
		}
		RegCloseKey(hReg);
	}
	if(debugmode) {
		ntsl_log_info("Read in %d filters (maxfilter=%d).",fc,maxfilter);
	}
	if(!fc) {
		ntsl_die("No filters defined -- unable to forward anything.  Bye!");
	}

}

/* return 1 if we want to send this message, 0 otherwise 
   if we want to send it, set status code and service name in event
   we could even set the message now as well?
*/
int check_filter( EVENTLOGRECORD      *record,ntsl_event *event) {
	int rv = 0; /* no match */
	int idx, inlist;
	int chk;
	filter *fp;

	if(!filter_head) { 	read_filters(); }

	rv = 0; /* set to 1 when a match is found. make sure it is set to NO */
	/* work down chain looking for match */
	for( fp = filter_head; fp; fp = fp->next ) {
		/* event log name */
		if(fp->eventlognum && fp->eventlog[0] && strcmp(fp->eventlog,event->eventlog)) {
			if(debugmode) {
				ntsl_log_info("Skipping filter %s as it is for another log: [%s]!=[%s]",
					fp->desc,fp->eventlog,event->eventlog);
			}
			continue;
		}
		/* type */
		if(debugmode) {
		ntsl_log_info("Checking %d against typeflags (%d).",record->EventType,fp->typeflags);
		}
		if((record->EventType == EVENTLOG_ERROR_TYPE)
			&& !(fp->typeflags & EVENTLOG_ERROR_FLAG)) { continue; }
		if((record->EventType == EVENTLOG_WARNING_TYPE)
			&& !(fp->typeflags & EVENTLOG_WARNING_FLAG)) { continue; }
		if((record->EventType == EVENTLOG_AUDIT_FAILURE)
			&& !(fp->typeflags & EVENTLOG_AUDIT_FAILURE_FLAG)) { continue; }
		if((record->EventType == EVENTLOG_AUDIT_SUCCESS)
			&& !(fp->typeflags & EVENTLOG_AUDIT_SUCCESS_FLAG)) { continue; }
		if((record->EventType == EVENTLOG_INFORMATION_TYPE)
			&& !(fp->typeflags & EVENTLOG_INFORMATION_FLAG)) { continue; }
		/* id */
		if(fp->numids > 0) {
			if(debugmode) { ntsl_log_info("Checking %u against ID list (length %d).",(record->EventID & 0xffff),fp->numids); }
			inlist = 0;
			for( idx = 0; ((idx < fp->numids)&&(idx<F_MAXIDS)); idx++ ) {
				if( fp->id[idx] == (unsigned short)(record->EventID & 0xffff) ) {
					inlist = 1;
					if(debugmode) { ntsl_log_info("Match found in list at index %d.",idx); }
					break;
				}
			} // see if it works better when we get rid of the XOR
			if( fp->notflags & NOT_EVENT) {
				if(inlist) { continue; }
			} else {
				if(!inlist) { continue; }
			}
		} /* numids>0 */
		/* source */
		if( fp->source[0] ) {
			if(debugmode) {
				ntsl_log_info("Checking [%s] against source [%s]",event->source,fp->source);
			}
			if( XOR(strncmp(fp->source, event->source, F_SOURCELEN),
				(fp->notflags & NOT_SOURCE))) { continue; }
		}
		/* match string */
		if( fp->regex ) {
			chk = regexec(fp->regex,event->msg);
			if(debugmode) {
				ntsl_log_info("Checking [%s] against matchstring [%s]: result is: %s",
					event->msg,fp->match, (chk?"Match!":"Not found"));
			}
			if( fp->notflags & NOT_MATCH ) {
				if( chk ) { continue; } 
			} else {
				if(!chk ) { continue; }
			}
//			if( XOR(!regexec(fp->regex,event->msg),
//				(fp->notflags & NOT_MATCH))) { continue; }
		}
		/* if we get this far, its a match. */
		rv = 1; 
		break;
	}

	if(rv && fp) {
		/* copy the service name and status over */
		event->status = fp->status;
		memset(event->service,0,NTSL_SVC_LEN);
		strncpy(event->service, fp->service,NTSL_SVC_LEN-1);
		memset(event->filter,0,NTSL_SYS_LEN);
		strncpy(event->filter,fp->desc,NTSL_SYS_LEN-1);
	}
	return(rv); 
}