// NTSyslogCtrl.h : main header file for the NTSYSLOGCTRL application
//

#if !defined(AFX_NTSYSLOGCTRL_H__9FB33EE5_E0E8_11D5_B306_0040055338AF__INCLUDED_)
#define AFX_NTSYSLOGCTRL_H__9FB33EE5_E0E8_11D5_B306_0040055338AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define NTSYSLOG_SERVICE_NAME	_T("NagiosEventLog")
#define NTSYSLOG_DISPLAY_NAME	_T("Nagios Event Log Monitor")

#define NTSYSLOG_SOFTWARE_KEY	_T( "SOFTWARE\\Cheshire Cat")
#define NTSYSLOG_SYSLOG_KEY		_T( "Nagios")
#define APPLICATION_SECTION		_T( "Application")
#define SECURITY_SECTION		_T( "Security")
#define SYSTEM_SECTION			_T( "System")
#define EVENTLOG_REG_PATH		_T( "System\\CurrentControlSet\\Services\\EventLog")

#define PRIMARY_SYSLOGD_ENTRY	_T( "NSCAD")
#define BACKUP_SYSLOGD_ENTRY	_T( "NSCAD1")
#define PRIMARY_NSCA_PORT	_T( "Port")
#define BACKUP_NSCA_PORT	_T( "Port1")
#define HOSTNAME_ENTRY		_T( "MyHostname" )
#define PASSWORD_ENTRY		_T( "nscaPassword" )
#define METHOD_ENTRY		_T( "encryptionMethod" )
#define DELAY_ENTRY		_T( "processDelay" )

#define STATUS_ENTRY		_T( "status" )
#define SERVICENAME_ENTRY	_T( "serviceName" )
#define MATCH_ENTRY			_T( "matchString" )
#define ID_ENTRY			_T( "ID" )
#define SOURCE_ENTRY		_T( "source" )
#define EVENTLOG_ENTRY		_T( "eventLog" )
#define EVENTLOGNAME_ENTRY	_T( "eventLogName" )
#define MAXFILTER_ENTRY		_T( "maxFilter" )
#define FILTERDESC_ENTRY	_T( "filterDesc" )
#define NOTMATCH_ENTRY		_T( "notMatch" )
#define NOTEVENT_ENTRY		_T( "notID" )
#define NOTSOURCE_ENTRY		_T( "notSource" )
#define NSCALOG_ENTRY		_T( "logAlerts" )
#define DEBUG_ENTRY			_T( "DEBUG" )
#define HBFREQ_ENTRY		_T( "heartbeatFreq" )
#define HBSVC_ENTRY			_T( "heartbeatSvc" )

#define INFORMATION_ENTRY		_T( "Information")
#define WARNING_ENTRY			_T( "Warning" )
#define ERROR_ENTRY				_T( "Error")
#define AUDIT_SUCCESS_ENTRY		_T( "Audit Success")
#define AUDIT_FAILURE_ENTRY		_T( "Audit Failure")

#define CHECK_NOT_ENABLED		0
#define CHECK_INFORMATION		1
#define CHECK_WARNING			2
#define CHECK_ERROR				4
#define CHECK_AUDIT_SUCCESS		8
#define CHECK_AUDIT_FAILURE		16

#define DEFAULT_CHECKS ( CHECK_ERROR + CHECK_AUDIT_FAILURE)
#define ALL_CHECKS (CHECK_INFORMATION + CHECK_WARNING + CHECK_ERROR \
						+ CHECK_AUDIT_SUCCESS + CHECK_AUDIT_FAILURE)

#define DEFAULT_STATUS		0

/////////////////////////////////////////////////////////////////////////////
// CNTSyslogCtrlApp:
// See NTSyslogCtrl.cpp for the implementation of this class
//

class CNTSyslogCtrlApp : public CWinApp
{
public:
	CNTSyslogCtrlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNTSyslogCtrlApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNTSyslogCtrlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NTSYSLOGCTRL_H__9FB33EE5_E0E8_11D5_B306_0040055338AF__INCLUDED_)
