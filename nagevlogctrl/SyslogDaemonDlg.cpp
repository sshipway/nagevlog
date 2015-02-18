// SyslogDaemonDlg.cpp : implementation file
//
#define _CRT_SECURE_NO_DEPRECATE

#include "stdafx.h"
#include "NTSyslogCtrl.h"
#include "NTService.h"
#include "SyslogDaemonDlg.h"
#include "nsca.h"
#include "ntsl.h"

#include "send-nsca.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSyslogDaemonDlg dialog


CSyslogDaemonDlg::CSyslogDaemonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSyslogDaemonDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSyslogDaemonDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSyslogDaemonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSyslogDaemonDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSyslogDaemonDlg, CDialog)
	//{{AFX_MSG_MAP(CSyslogDaemonDlg)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyslogDaemonDlg message handlers

BOOL CSyslogDaemonDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CWaitCursor	cWait;
	HKEY		hKeyRemote, hKeySoftware,
				hKey;
	DWORD		dwSize,
				dwType, dwValue;
	CString		csKeyName;
	TCHAR		szBuffer[255];
			CStringArray __tmp;

// Fill the encryption types?
#ifdef HAVE_LIBMCRYPT
	CComboBox *peEnc = (CComboBox *)GetDlgItem(IDC_ENCRYPTION);
	peEnc->ResetContent();
	peEnc->AddString(_T("(0) None"));
	peEnc->AddString(_T("(1) XOR"));
	peEnc->AddString(_T("(2) DES"));
	peEnc->AddString(_T("(3) 3DES"));
	peEnc->AddString(_T("(4) CAST-128"));
	peEnc->AddString(_T("(5) CAST-256"));
	peEnc->AddString(_T("(6) xTEA"));
	peEnc->AddString(_T("(7) 3-way"));
	peEnc->AddString(_T("(8) Skipjack"));
	peEnc->AddString(_T("(9) Twofish"));
	peEnc->AddString(_T("(10) LOKI97"));
	peEnc->AddString(_T("(11) RC2"));
	peEnc->AddString(_T("(12) RC4"));
	peEnc->AddString(_T("(13) RC6"));
	peEnc->AddString(_T("(14) Rijndael-128"));
	peEnc->AddString(_T("(15) Rijndael-192"));
	peEnc->AddString(_T("(16) Rijndael-256"));
	peEnc->AddString(_T("(17) Mars"));
	peEnc->AddString(_T("(18) Panama"));
	peEnc->AddString(_T("(19) Wake"));
	peEnc->AddString(_T("(20) Serpent"));
	peEnc->AddString(_T("(21) Idea"));
	peEnc->AddString(_T("(22) ENIGMA (UNIX)"));
	peEnc->AddString(_T("(23) GOST"));
	peEnc->AddString(_T("(24) SAFER-sk64"));
	peEnc->AddString(_T("(25) SAFER-sk128"));
	peEnc->AddString(_T("(26) SAFER+"));
	peEnc->SelectString(0,"(0) None");
#endif


	// Open the registry on HKLM
	if (RegConnectRegistry( (char*)((LPCTSTR)m_csComputer), HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
	{
		csKeyName.Format( _T( "Error while connecting to the registry !\n\nEnsure that\n\n%s\n%s"),
						  _T( "1) Network Registry access is enabled if this is a remote computer."),
						  _T( "2) You have Administrator privilieges on the computer."));
		AfxMessageBox( csKeyName, MB_ICONSTOP);
		CDialog::OnCancel();
		return TRUE;
	}
		// Create the SOFTWARE\Cheshire Cat key or open it if it exists
	if (RegOpenKeyEx( hKeyRemote, NTSYSLOG_SOFTWARE_KEY, 0, KEY_READ, &hKeySoftware) == ERROR_SUCCESS)
	{
		if (RegOpenKeyEx( hKeySoftware, NTSYSLOG_SYSLOG_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
		dwSize = 255*sizeof( TCHAR);
		memset( szBuffer, 0, 255*sizeof( TCHAR));
		if (RegQueryValueEx( hKey, PASSWORD_ENTRY, 0, &dwType, (LPBYTE) szBuffer, &dwSize) == ERROR_SUCCESS)
			SetDlgItemText( IDC_PASSWORD, szBuffer);
		dwSize = 255*sizeof( TCHAR);
		memset( szBuffer, 0, 255*sizeof( TCHAR));
		if (RegQueryValueEx( hKey, HOSTNAME_ENTRY, 0, &dwType, (LPBYTE) szBuffer, &dwSize) == ERROR_SUCCESS)
			SetDlgItemText( IDC_HOSTNAME, szBuffer);
		// Key exist => read values
		// Primary Syslogd
		dwSize = 255*sizeof( TCHAR);
		memset( szBuffer, 0, 255*sizeof( TCHAR));
		if (RegQueryValueEx( hKey, PRIMARY_SYSLOGD_ENTRY, 0, &dwType, (LPBYTE) szBuffer, &dwSize) == ERROR_SUCCESS)
			SetDlgItemText( IDC_PRIMARY_SYSLOGD, szBuffer);
		dwSize = sizeof( DWORD);
		memset( szBuffer, 0, 255 );
		if (RegQueryValueEx( hKey, PRIMARY_NSCA_PORT, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) {
			if((dwValue<1) || (dwValue > 65535)) { dwValue = 5667; }
			sprintf ( szBuffer,"%d",dwValue );
			SetDlgItemText( IDC_NSCA_PORT, szBuffer);
		}
		// Backup Syslogd
		dwSize = 255*sizeof( TCHAR);
		memset( szBuffer, 0, 255*sizeof( TCHAR));
		if (RegQueryValueEx( hKey, BACKUP_SYSLOGD_ENTRY, 0, &dwType, (LPBYTE) szBuffer, &dwSize) == ERROR_SUCCESS)
			SetDlgItemText( IDC_BACKUP_SYSLOGD, szBuffer);
		dwSize = sizeof( DWORD);
		memset( szBuffer, 0, 255 );
		if (RegQueryValueEx( hKey,BACKUP_NSCA_PORT, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) {
			if((dwValue<1) || (dwValue > 65535)) { dwValue = 5667; }
			sprintf ( szBuffer,"%d",dwValue );
			SetDlgItemText( IDC_NSCA_PORT, szBuffer);
		}

		dwSize = sizeof( DWORD);
		if (RegQueryValueEx( hKey, METHOD_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) != ERROR_SUCCESS) {
			dwValue = 0;
		}
		CComboBox *pEncWnd = (CComboBox *)GetDlgItem(IDC_ENCRYPTION);
		if( pEncWnd->SetCurSel(dwValue) == CB_ERR)
		{		pEncWnd->SetCurSel(0);	}


		dwSize = sizeof( DWORD); dwValue = 0;
		if (RegQueryValueEx( hKey, DEBUG_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) {
			CheckDlgButton( IDC_DEBUG, dwValue );
		}

		dwSize = sizeof( DWORD); dwValue = 0;
		if (RegQueryValueEx( hKey, NSCALOG_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) {
			CheckDlgButton( IDC_NSCALOG, dwValue );
		}

		RegCloseKey( hKey);
	}
		RegCloseKey( hKeySoftware);
	}

	RegCloseKey( hKeyRemote);



	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSyslogDaemonDlg::OnOK() 
{
	// TODO: Add extra validation here
	CWaitCursor	cWait;
	HKEY		hKeyRemote, hKeySyslog,
				hKeySoftware;
	DWORD		dwSize,
				dwValue, dwPrimaryPort, dwBackupPort;
	CString		csPrimary, csHostname, csPassword,
				csBackup, csPrimaryPort, csBackupPort;

	if (AfxMessageBox( _T( "Do you really want to save NSCA Daemon parameters ?\n\nNB: if service is started, you must restart it for change to take effect."),
				MB_YESNO|MB_ICONQUESTION) == IDNO)
	{		return;	}
	if (GetDlgItemText( IDC_HOSTNAME, csHostname) == 0)
	{
		AfxMessageBox( _T( "You MUST specify this machine's Nagios host name!"), MB_ICONSTOP);
		return;
	}
	if (GetDlgItemText( IDC_PRIMARY_SYSLOGD, csPrimary) == 0)
	{
		AfxMessageBox( _T( "You MUST specify at least a Primary NSCA Daemon server !"), MB_ICONSTOP);
		return;
	}
	GetDlgItemText( IDC_PASSWORD, csPassword);
	GetDlgItemText( IDC_BACKUP_SYSLOGD, csBackup);
	GetDlgItemText( IDC_NSCA_PORT, csPrimaryPort);
	dwPrimaryPort = atoi(csPrimaryPort);
	GetDlgItemText( IDC_NSCA_PORT1, csBackupPort);
	dwBackupPort = atoi(csBackupPort);

	// Connect to the registry on HKLM
	if (RegConnectRegistry( (char*)((LPCTSTR)m_csComputer), HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error while connecting to the registry !\n\nPlease retry."), MB_ICONSTOP);
		return;
	}
	// Create the SOFTWARE\CheshireCat key or open it if it exists
	if (RegCreateKeyEx( hKeyRemote, NTSYSLOG_SOFTWARE_KEY, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
						KEY_WRITE|KEY_READ, NULL, &hKeySoftware, &dwValue) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey (hKeyRemote);
		return;
	}
		// Create the Syslog subkey or open it if it exists
	if (RegCreateKeyEx( hKeySoftware, NTSYSLOG_SYSLOG_KEY, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
						KEY_WRITE|KEY_READ, NULL, &hKeySyslog, &dwValue) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	dwSize = csPassword.GetLength();
	if (RegSetValueEx( hKeySyslog, PASSWORD_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csPassword), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
			RegCloseKey (hKeySyslog);
		RegCloseKey (hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}


	dwSize = csHostname.GetLength();
	if (RegSetValueEx( hKeySyslog, HOSTNAME_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csHostname), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
			RegCloseKey (hKeySyslog);
		RegCloseKey (hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	// Write the primary syslogd server
	dwSize = csPrimary.GetLength();
	if (RegSetValueEx( hKeySyslog, PRIMARY_SYSLOGD_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csPrimary), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
			RegCloseKey (hKeySyslog);
		RegCloseKey (hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	dwSize = sizeof( DWORD );
	if (RegSetValueEx( hKeySyslog, PRIMARY_NSCA_PORT, 0, REG_DWORD, (LPBYTE ) &dwPrimaryPort, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
			RegCloseKey (hKeySyslog);
		RegCloseKey (hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	
		CComboBox *pEncWnd = (CComboBox *)GetDlgItem(IDC_ENCRYPTION);
		dwValue = pEncWnd->GetCurSel();
			if( dwValue	== CB_ERR) { dwValue = 0; }
		dwSize = sizeof( DWORD );
	if (RegSetValueEx( hKeySyslog, METHOD_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey (hKeySoftware);
	RegCloseKey (hKeySyslog);
		RegCloseKey( hKeyRemote);
		return;
	}

	dwValue = (IsDlgButtonChecked( IDC_DEBUG) ? 1 : 0);
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySyslog, DEBUG_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	
	dwValue = (IsDlgButtonChecked( IDC_NSCALOG) ? 1 : 0);
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySyslog, NSCALOG_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	
	// Write the backup syslogd server
	if (!csBackup.IsEmpty())
	{
		dwSize = csBackup.GetLength();
		if (RegSetValueEx( hKeySyslog, BACKUP_SYSLOGD_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csBackup), dwSize) != ERROR_SUCCESS)
		{
			AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
			RegCloseKey (hKeySyslog);
			RegCloseKey (hKeySoftware);
			RegCloseKey( hKeyRemote);
			return;
		}

		dwSize = sizeof( DWORD );
		if (RegSetValueEx( hKeySyslog, BACKUP_NSCA_PORT, 0, REG_DWORD, (LPBYTE ) &dwBackupPort, dwSize) != ERROR_SUCCESS)
		{
			AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
			RegCloseKey (hKeySoftware);
			RegCloseKey (hKeySyslog);
			RegCloseKey( hKeyRemote);
			return;
		}

	}
	else {
		RegDeleteValue( hKeySyslog, BACKUP_SYSLOGD_ENTRY);
		RegDeleteValue( hKeySyslog, BACKUP_NSCA_PORT);
	}
	RegCloseKey (hKeySyslog);
	RegCloseKey (hKeySoftware);
	RegCloseKey( hKeyRemote);
	CDialog::OnOK();
}

void CSyslogDaemonDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if (IsModified() && (AfxMessageBox( _T( "Do you really want to discard NSCA Daemons parameters changes ?"),
			MB_YESNO|MB_ICONQUESTION) == IDNO))
			return;
	CDialog::OnCancel();
}

void CSyslogDaemonDlg::SetComputer(LPCTSTR lpstrComputer)
{
	if ((lpstrComputer == NULL) || (_tcsclen( lpstrComputer) == 0))
		m_csComputer.Empty();
	else
		// Set computer in \\computer_name format
		m_csComputer.Format( _T( "\\\\%s"), lpstrComputer);
}

BOOL CSyslogDaemonDlg::IsModified()
{
	if (SendDlgItemMessage( IDC_PRIMARY_SYSLOGD, EM_GETMODIFY, 0, 0L))
		return TRUE;
	if (SendDlgItemMessage( IDC_BACKUP_SYSLOGD, EM_GETMODIFY, 0, 0L))
		return TRUE;
	if (SendDlgItemMessage( IDC_NSCA_PORT, EM_GETMODIFY, 0, 0L))
		return TRUE;
	if (SendDlgItemMessage( IDC_NSCA_PORT1, EM_GETMODIFY, 0, 0L))
		return TRUE;
	if (SendDlgItemMessage( IDC_HOSTNAME, EM_GETMODIFY, 0, 0L))
		return TRUE;
	if (SendDlgItemMessage( IDC_PASSWORD, EM_GETMODIFY, 0, 0L))
		return TRUE;
	if (SendDlgItemMessage( IDC_ENCRYPTION, EM_GETMODIFY, 0, 0L))
		return TRUE;
	return FALSE;

}

void CSyslogDaemonDlg::OnTest() 
{
		int rv;
		char nscamsg[1024];
		int port;
		char server_name[64];
			WSADATA  			 wsaData;
	CString csport;
	CString cssvr;
	char error_msg[1024];


	/* now do a test */
   if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
    {
/*        ntsl_log_error(NTSL_ERROR_WSASTARTUP, WSAGetLastError()); */
		AfxMessageBox( _T("NSCA Send failed: Winsock fail"),MB_OK|MB_ICONERROR);
        WSACleanup();
        return;
    }

	/* now, get from the registry the nagiosname, nsca host, nsca port */

	_snprintf0(nscamsg,sizeof(nscamsg),"This is a test message generated by NagEvLog agent" );

	GetDlgItemText( IDC_NSCA_PORT, csport);
	port =  atoi(csport);
	GetDlgItemText( IDC_PRIMARY_SYSLOGD, cssvr);
	strcpy(server_name, cssvr); 
	error_msg[0] = (char)0;

		/* here, we're calling a C (not c++) function  */
	rv = send_nsca(server_name, port, "Test", 0, nscamsg, error_msg );
	if(rv) { 
		if(*error_msg) {
			AfxMessageBox( error_msg,MB_OK|MB_ICONERROR);
		} else {
			AfxMessageBox( _T("Unable to send NSCA message."),MB_OK|MB_ICONERROR);
		}
		/* ("Send failed: Winsock error: %d", WSAGetLastError()); */
	} else {
		AfxMessageBox( _T("NSCA Send succeeded!\nSent a status 'OK' message to service 'Test' for this host.\nCheck your NSCA/Nagios logs."),
			MB_OK|MB_ICONINFORMATION);
	}

	WSACleanup();

}
