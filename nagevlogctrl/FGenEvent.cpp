// FGenEvent.cpp : implementation file
//
#define _CRT_SECURE_NO_DEPRECATE

#include "stdafx.h"
#include "ntsyslogctrl.h"
#include <windows.h>
#include "FGenEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FGenEvent dialog


FGenEvent::FGenEvent(CWnd* pParent /*=NULL*/)
	: CDialog(FGenEvent::IDD, pParent)
{
	//{{AFX_DATA_INIT(FGenEvent)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void FGenEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FGenEvent)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FGenEvent, CDialog)
	//{{AFX_MSG_MAP(FGenEvent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FGenEvent message handlers

void FGenEvent::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void FGenEvent::OnOK() 
{
	char		s[256];
	WORD		etype;
	DWORD		eid;
	CString		csItem;
	char		src[64];
	HANDLE		hEvent;
	const char		*strings[2] = { s, NULL };

	eid = 1;
	csItem = "1";
	GetDlgItemText( IDC_EVENTID, csItem);
	if(csItem.GetLength() >0) { eid = atoi(csItem);}
	csItem = "NagiosEventLog Test";
	GetDlgItemText( IDC_SOURCE, csItem);
	src[0] = 0;
	if(csItem.GetLength() >0) { strcpy(src,csItem);}
	csItem = "Testing the eventlog.";
	GetDlgItemText( IDC_TEXT, csItem);
	s[0] = 0;
	if(csItem.GetLength() >0) { strcpy(s,csItem);}
	etype = EVENTLOG_INFORMATION_TYPE;
	if( IsDlgButtonChecked(IDC_STAT_WARNING) ) { etype = EVENTLOG_WARNING_TYPE; }
	if( IsDlgButtonChecked(IDC_STAT_ERROR) ) { etype = EVENTLOG_ERROR_TYPE; }
	if( IsDlgButtonChecked(IDC_STAT_AUDIT_SUCCESS) ) { etype = EVENTLOG_AUDIT_SUCCESS; }
	if( IsDlgButtonChecked(IDC_STAT_AUDIT_FAIL) ) { etype = EVENTLOG_AUDIT_FAILURE; }

	if(!src[0]) { strcpy(src,"NagiosEventLog Test"); }
	if(!s[0]) { strcpy(s,"Test Message"); }

	hEvent = RegisterEventSource(NULL, src);
	ReportEvent(hEvent, etype, 0, eid, NULL, 1, 0, strings, NULL);
	DeregisterEventSource(hEvent);

	CDialog::OnOK();
}

BOOL FGenEvent::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CheckDlgButton(IDC_STAT_INFO,1);
	SetDlgItemText(IDC_EVENTID,"0");
	SetDlgItemText(IDC_SOURCE,"NagiosEventLog Test");
	SetDlgItemText(IDC_TEXT,"Test message");
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
