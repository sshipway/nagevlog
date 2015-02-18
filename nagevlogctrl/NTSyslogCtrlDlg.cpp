// NTSyslogCtrlDlg.cpp : implementation file
//
#define _CRT_SECURE_NO_DEPRECATE

#include "stdafx.h"
#include "NTSyslogCtrl.h"
#include "NTService.h"
#include "NTSyslogCtrlDlg.h"
#include "FGenEvent.h"

#include "ConfigLogging.h"
#include "SyslogDaemonDlg.h"
#include "SelectServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnStnClickedVersioninfo();
//	afx_msg void OnStnClickedVersioninfo();
	afx_msg void OnBnClickedOk();
};

//MAYBE HAVE TO COMMENT THIS OUT AND CALL IT
//THE WAY THE OTHER ONE IS CALLED
void CNTSyslogCtrlDlg::OnAppAbout()
{
	CAboutDlg aDlg;

	aDlg.DoModal();
}

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//CDialog(IDD_ABOUTBOX).DoModal();
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;
	CString	csAppTitle;
	DWORD dwHandle,dwBytes;
	DWORD cbTranslate;
	char SubBlock[256];
	char *pBlock, *pBlockA;
	char *lpBuffer;
	char *lpBufferAgent;
	char *lpBufferCtrl;
	char UNKNOWN[] = "UNKNOWN";
	CString csVersion;

	CDialog::OnInitDialog();
	csAppTitle.LoadString( AFX_IDS_APP_TITLE);
	SetDlgItemText( IDC_STATUS, csAppTitle);

	// Read the versions of the parts
	pBlock = pBlockA = (char *)0;
	lpBufferAgent = lpBufferCtrl = UNKNOWN;
	//AfxMessageBox( _T("Starting..."), MB_ICONSTOP);
	dwBytes = GetFileVersionInfoSize ((LPTSTR)"nagevlog.exe", &dwHandle);
	if(!dwBytes) { 
			AfxMessageBox( _T("Error: No version information available for Agent"), MB_ICONSTOP);
	} else {
		pBlockA = (char *)malloc(dwBytes+1);
		if( GetFileVersionInfo((LPTSTR)"nagevlog.exe",dwHandle,dwBytes,pBlockA) ) {
			if( VerQueryValue(pBlockA, 
              TEXT("\\VarFileInfo\\Translation"),
              (LPVOID*)&lpTranslate, (PUINT)&cbTranslate) ) {
				wsprintf( SubBlock, TEXT("\\StringFileInfo\\%04x%04x\\FileVersion"),
				lpTranslate[0].wLanguage,lpTranslate[0].wCodePage);
				if( VerQueryValue(pBlockA, SubBlock, (LPVOID*)&lpBuffer, (PUINT)&dwBytes) ) {
					lpBufferAgent = lpBuffer;
					//csVersion.AppendFormat("Agent: %s",lpBuffer);
				}
			} else {
				AfxMessageBox( _T("Error: No translations available for Agent"), MB_ICONSTOP);
			}
		}
	}


	//AfxMessageBox( _T("Getting NagEvLogCtrl size"), MB_ICONSTOP);
	dwBytes = GetFileVersionInfoSize ((LPTSTR)"nagevlogctrl.exe", &dwHandle);
	if(!dwBytes) { 
			AfxMessageBox( _T("Error: No version information available for Config App"), MB_ICONSTOP);
	} else {
		pBlock = (char *)malloc(dwBytes+1);
		//AfxMessageBox( _T("Retrieved NagEvLogCtrl size"), MB_ICONSTOP);
		if( GetFileVersionInfo((LPTSTR)"nagevlogctrl.exe",dwHandle,dwBytes,pBlock) ) {
		//AfxMessageBox( _T("Retrieved NagEvLogCtrl info"), MB_ICONSTOP);
			if( VerQueryValue(pBlock, TEXT("\\VarFileInfo\\Translation"),
              (LPVOID*)&lpTranslate, (PUINT) &cbTranslate) ) {
				//AfxMessageBox( _T("Retrieved NagEvLogCtrl translations"), MB_ICONSTOP);
				wsprintf( SubBlock, TEXT("\\StringFileInfo\\%04x%04x\\FileVersion"),
					lpTranslate[0].wLanguage,lpTranslate[0].wCodePage);
				if( VerQueryValue(pBlock, SubBlock, (LPVOID*)&lpBuffer, (PUINT)&dwBytes) ) {
					//AfxMessageBox(lpBuffer, MB_ICONSTOP);
					lpBufferCtrl = lpBuffer;
					//csVersion.AppendFormat(_T("  Config: %s"),lpBuffer);
				}
			} else {
				AfxMessageBox( _T("Error: No translations available for Config Tool"), MB_ICONSTOP);
			}
		} else {
			AfxMessageBox( _T("Error: Couldn't read version information for Config Tool"), MB_ICONSTOP);
		}
	}
	csVersion.Format("Versions:  Agent: [%s]  Control App: [%s]",lpBufferAgent,lpBufferCtrl);
	//AfxMessageBox( csVersion, MB_ICONSTOP);
	SetDlgItemText( IDC_STATUS, csVersion);
	if(pBlock) { free(pBlock); }
	if(pBlockA) { free(pBlockA); }
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CNTSyslogCtrlDlg dialog

CNTSyslogCtrlDlg::CNTSyslogCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNTSyslogCtrlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNTSyslogCtrlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

//***MUST*** be called ***AFTER*** setting the m_csComputer property for
//CNTSyslogCtrlDlg objects
void CNTSyslogCtrlDlg::ReadMachineEventLogSettings(int good_query)
{
	HKEY			hKeyRemote,hKeySyslog,hKeySoftware,
					hKey;
	CString		csKeyName;
	DWORD		dwSize, dwType, dwValue;
	CStringArray	__tmp;
	TCHAR	szBuffer[255];

	//Sometimes connecting to a remote computer takes a while
	//Especially if that computer doesn't exist on the network
	//or is off-line
	CWaitCursor		cWait;

	maxfilter = -1;

	//reset the EventlogSelect property & free memory to avoid memory leaks
	m_csaEventlogSelect.RemoveAll();
	m_csaEventlogSelect.FreeExtra();
	m_cuiaFilterMap.RemoveAll();
	m_cuiaFilterMap.FreeExtra();

	//set the EventlogSelect size to the default
	m_csaEventlogSelectSize = 1;

	//Check if query service status was successful
	//if it wasn't skip attempting to connect to the registry
	//Attempting to connect if the computer is unreachable
	//adds about 15-30 seconds on loading time each time
	//an unreachable computer is selected
	if(good_query == TRUE)
	{
		// Connect to the selected computer's registry on HKLM
		if (RegConnectRegistry( (char*)(LPCTSTR)m_csComputer, HKEY_LOCAL_MACHINE, &hKeyRemote) == ERROR_SUCCESS)
		{

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


//			csKeyName.Format( _T( "%s\\%s"), NTSYSLOG_SOFTWARE_KEY, NTSYSLOG_SYSLOG_KEY);
//			if (RegOpenKeyEx( hKeyRemote, csKeyName, 0, KEY_READ, &hKeySyslog) == ERROR_SUCCESS)	
//			{
				dwSize = sizeof( DWORD);
				maxfilter = -1;

				if (RegQueryValueEx( hKeySyslog, MAXFILTER_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS)
				{ maxfilter = (int)dwValue; }

				m_csaEventlogSelectSize = 0;
				//loop until done reading all subkeys
				for(int index = 0; index <= maxfilter; index++ )
				{
					csKeyName.Format( _T("Filter%d"),index);
					if (RegOpenKeyEx( hKeySyslog, csKeyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS)	
					{

						dwSize = 255*sizeof( TCHAR);
						memset( szBuffer, 0, 255*sizeof( TCHAR));
						if (RegQueryValueEx( hKey, FILTERDESC_ENTRY, 0, &dwType, (LPBYTE) szBuffer, &dwSize) == ERROR_SUCCESS)
						{
							m_csaEventlogSelect.SetAtGrow(m_csaEventlogSelectSize,szBuffer);
						} else {
							m_csaEventlogSelect.SetAtGrow(m_csaEventlogSelectSize,csKeyName);
						}
						m_cuiaFilterMap.SetAtGrow(m_csaEventlogSelectSize,(unsigned int)index);
						m_csaEventlogSelectSize++;
						RegCloseKey(hKey);
					} // filter exists
				}
				//don't need the handles to the Registry anymore
				RegCloseKey(hKeySyslog);
				RegCloseKey(hKeyRemote);

//			}//end if (RegOpenKeyEx( hKeyRemote, EVENTLOG_REG_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		}//end if (RegConnectRegistry( (char*)((LPCTSTR)m_csComputer), HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
	}//end if(good_query == TRUE)
	else
	{
		if( m_csComputer.GetLength() > 0 ) {
		//Generate an error message
		CString __Fmt;
		__Fmt.Format( _T("Error while connecting to the registry on %s!\n\nPlease verify permissions on %s."),m_csComputer,m_csComputer);
		AfxMessageBox(__Fmt, MB_ICONSTOP);
		}
		//Add extra notification to the soon to be disabled combobox
		m_csaEventlogSelect.Add("Error Connecting to Registry");
		
	}//end else clause of if(good_query == TRUE)

	//Now need to populate the combo box with data
	// Get pointer to the combo boxe.
	CComboBox *peType = (CComboBox *)GetDlgItem(IDC_EVENTLOG_SELECT);
	peType->ResetContent();

	// If it doesn't exist stop rather than crashing.  Should never happen.
	if(peType == NULL)
	{
		AfxMessageBox("Program error: Dialog now missing controls.  This "
				"message should never appear.", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	for(DWORD i = 0; i < m_csaEventlogSelectSize; i++)
	{
		peType->AddString( m_csaEventlogSelect.GetAt(i));
	}

	//add something here to set the combobox to the first value in the list
	if(m_csaEventlogSelectSize > 0 ) {
		peType->SelectString(0,m_csaEventlogSelect.GetAt(0));
	}

}//end NYSyslogCtrlDlg::ReadMachineEventLogSettings()

void CNTSyslogCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNTSyslogCtrlDlg)
	DDX_Control(pDX, IDC_STATUS_LIGHT, m_StatusIcon);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNTSyslogCtrlDlg, CDialog)
	//{{AFX_MSG_MAP(CNTSyslogCtrlDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SELECT_COMPUTER, OnSelectComputer)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SYSLOGD, OnSyslogd)
	ON_BN_CLICKED(IDC_EVENTLOG, OnEventLog)
	ON_BN_CLICKED(IDC_START, OnStartService)
	ON_BN_CLICKED(IDC_STOP, OnStopService)
	ON_BN_CLICKED(IDC_CREATE, OnCreate)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CLEARALL, OnClearall)
	ON_BN_CLICKED(IDC_MOVEUP, OnMoveup)
	ON_BN_CLICKED(IDC_MOVEDOWN, OnMovedown)
	ON_BN_CLICKED(IDC_GENEVENT, OnGenevent)
	ON_BN_CLICKED(IDC_ABOUTBOX, OnAppAbout)
	ON_EN_KILLFOCUS(IDC_DELAY, OnKillfocusDelay)
	//}}AFX_MSG_MAP
	ON_EN_KILLFOCUS(IDC_HBSVC, OnEnKillfocusHbsvc)
	ON_EN_KILLFOCUS(IDC_HBFREQ, OnEnKillfocusHbfreq)
	ON_BN_CLICKED(IDC_EXPORT, OnBnClickedExport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNTSyslogCtrlDlg message handlers

BOOL CNTSyslogCtrlDlg::OnInitDialog()
{
	HKEY		hKeyRemote, hKeySoftware, hKey;
	DWORD		dwSize,	dwType, dwValue;
	TCHAR		szBuffer[255];
	CString 	csMessage;

	CDialog::OnInitDialog();

		// Open the registry on HKLM
	dwValue = 0;
	if (RegConnectRegistry( (char*)((LPCTSTR)m_csComputer), HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
	{
		csMessage.Format( _T( "Error while connecting to the registry !\n\nEnsure that\n\n%s\n%s"),
						  _T( "1) Network Registry access is enabled if this is a remote computer."),
						  _T( "2) You have Administrator privilieges on the computer."));
		AfxMessageBox( csMessage, MB_ICONSTOP);
	} else {
		// Create the SOFTWARE\Cheshire Cat key or open it if it exists
		if (RegOpenKeyEx( hKeyRemote, NTSYSLOG_SOFTWARE_KEY, 0, KEY_READ, &hKeySoftware) == ERROR_SUCCESS)
		{
			if (RegOpenKeyEx( hKeySoftware, NTSYSLOG_SYSLOG_KEY, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
			{	
				dwSize = sizeof(DWORD);
				if (RegQueryValueEx( hKey, DELAY_ENTRY, 0, &dwType, (unsigned char *)&dwValue, &dwSize) != ERROR_SUCCESS) {	
					dwValue = 0;
				}
			}
		}
	}
	if((dwValue<1) || (dwValue > 65535)) { dwValue = 30; }
	memset( szBuffer, 0, 255 );
	sprintf ( szBuffer,"%d",dwValue );
	SetDlgItemText( IDC_DELAY, szBuffer);
	if( hKeySoftware ) {
		dwSize = sizeof(DWORD);
		if (RegQueryValueEx( hKey, HBFREQ_ENTRY, 0, &dwType, (unsigned char *)&dwValue, &dwSize) != ERROR_SUCCESS) {	
			dwValue = 10;
		}
	} else {
		dwValue = 10;
	}
	memset( szBuffer, 0, 255 );
	sprintf ( szBuffer,"%d",dwValue );
	SetDlgItemText( IDC_HBFREQ, szBuffer);
	if( hKeySoftware ) {
		dwSize = sizeof(szBuffer)-1;
		if (RegQueryValueEx( hKey, HBSVC_ENTRY, 0, &dwType, (unsigned char *)&szBuffer, &dwSize) != ERROR_SUCCESS) {	
			memset( szBuffer, 0, 255 );			
		} else {
			szBuffer[dwSize] = (char)0;
		}
		SetDlgItemText( IDC_HBSVC, szBuffer);
	} else {
		SetDlgItemText( IDC_HBSVC,"" );
	}

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	// Load last computer from the registry
	m_csComputer = AfxGetApp()->GetProfileString( COMPUTERS_SECTION, LAST_COMPUTER_ENTRY);

	SetComputerName();
	int queryserviceresults = ( QueryServiceStatus() ) ? 1 : 0;
	SetMainDialogControls(queryserviceresults);
	ReadMachineEventLogSettings(queryserviceresults);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNTSyslogCtrlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNTSyslogCtrlDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNTSyslogCtrlDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CNTSyslogCtrlDlg::OnSelectComputer() 
{
	// TODO: Add your control notification handler code here
	CSelectServerDlg	cDlg;

	cDlg.SetCurrentComputer( m_csComputer);
	if (cDlg.DoModal() == IDCANCEL)
		return;
	//Set a wait cursor -- PERHAPS THIS IS CONFUSING THE ISSUE
	CWaitCursor				 cWait;
	m_csComputer = cDlg.GetNewComputer();
	// Write computer name to the registry
	AfxGetApp()->WriteProfileString( COMPUTERS_SECTION, LAST_COMPUTER_ENTRY, m_csComputer);
	SetComputerName(); 
	KillTimer( 1);
	//Check for service on remote machine and set dialog buttons
	int queryserviceresults = (QueryServiceStatus()) ? 1 : 0;
	SetMainDialogControls(queryserviceresults);
	//Setup the dialog with the new machine settings, if any
	ReadMachineEventLogSettings(queryserviceresults);
}

//This function either enables or disables the contained buttons
//from the main dialog.  Use 1 to enable, 0 to disable.  Any other
//buttons added later which require enabling or disabling should
//be placed here
void CNTSyslogCtrlDlg::SetMainDialogControls(int _i_)
{
	if(_i_)
		SetTimer( 1, 1000, NULL);

	GetDlgItem( IDC_SYSLOGD)->EnableWindow( _i_);
	GetDlgItem( IDC_EVENTLOG)->EnableWindow( _i_);
	GetDlgItem( IDC_EVENTLOG_SELECT)->EnableWindow( _i_);
	GetDlgItem( IDC_CREATE)->EnableWindow( _i_);
	GetDlgItem( IDC_DELETE)->EnableWindow( _i_);
	GetDlgItem( IDC_CLEARALL)->EnableWindow( _i_);
	GetDlgItem( IDC_MOVEUP)->EnableWindow( _i_);
	GetDlgItem( IDC_MOVEDOWN)->EnableWindow( _i_);
}

void CNTSyslogCtrlDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnTimer(nIDEvent);
	if (QueryServiceStatus())		
		SetTimer( 1, 1000, NULL);
}


//Check the status of the NTSyslog service on the selected computer
BOOL CNTSyslogCtrlDlg::QueryServiceStatus()
{
	CNTScmService			 myService;
	CNTServiceControlManager mySCM;
	SERVICE_STATUS			 myServiceStatus;
	CString					 csComputer;

	if (!m_csComputer.IsEmpty())
		// Set computer in \\computer_name format
		csComputer.Format( _T( "\\\\%s"), m_csComputer);
	else
		csComputer.Empty();
	if (!mySCM.Open( csComputer, SC_MANAGER_ALL_ACCESS|GENERIC_READ))
	{
		DisplayStatus( IDI_ERROR_ICON);
		return FALSE;
	}
	if (!mySCM.OpenService( NTSYSLOG_SERVICE_NAME, SERVICE_QUERY_STATUS, myService))
	{
		DisplayStatus( IDI_ERROR_ICON);
		mySCM.Close();
		return FALSE;
	}
	if (!myService.QueryStatus( &myServiceStatus))
	{
		DisplayStatus( IDI_ERROR_ICON);
		myService.Close();
		mySCM.Close();
		return FALSE;
	}
	myService.Close();
	mySCM.Close();
	switch (myServiceStatus.dwCurrentState)
	{
	case SERVICE_START_PENDING:
		DisplayStatus( IDI_YELLOW_ICON, myServiceStatus.dwCurrentState);
		break;
	case SERVICE_RUNNING:
		DisplayStatus( IDI_GREEN_ICON);
		break;
	case SERVICE_STOP_PENDING:
		DisplayStatus( IDI_YELLOW_ICON, myServiceStatus.dwCurrentState);
		break;
	case SERVICE_STOPPED:
		DisplayStatus( IDI_RED_ICON);
		break;
	case SERVICE_CONTINUE_PENDING:
		DisplayStatus( IDI_YELLOW_ICON, myServiceStatus.dwCurrentState);
		break;
	case SERVICE_PAUSE_PENDING:
		DisplayStatus( IDI_YELLOW_ICON, myServiceStatus.dwCurrentState);
		break;
	case SERVICE_PAUSED:
		DisplayStatus( IDI_YELLOW_ICON, myServiceStatus.dwCurrentState);
		break;
	default:
		DisplayStatus( IDI_ERROR_ICON);
		break;
	}
	return TRUE;
}

BOOL CNTSyslogCtrlDlg::DisplayStatus(UINT nIconID, DWORD dwServiceState)
{
	HICON	hIcon;

	if ((hIcon = AfxGetApp()->LoadIcon( nIconID)) == NULL)
		return FALSE;
	m_StatusIcon.SetIcon( hIcon);
	switch (nIconID)
	{
	case IDI_GREEN_ICON: // Service started
		SetDlgItemText( IDC_STATUS, _T( "Service is running."));
		GetDlgItem( IDC_START)->EnableWindow( FALSE);
		GetDlgItem( IDC_STOP)->EnableWindow( TRUE);
		break;
	case IDI_YELLOW_ICON: // Service pending
		switch (dwServiceState)
		{
		case SERVICE_START_PENDING:
			SetDlgItemText( IDC_STATUS, _T( "Service is starting."));
			break;
		case SERVICE_STOP_PENDING:
			SetDlgItemText( IDC_STATUS, _T( "Service is stopping."));
			break;
		case SERVICE_CONTINUE_PENDING:
			SetDlgItemText( IDC_STATUS, _T( "Service is leaving pause."));
			break;
		case SERVICE_PAUSE_PENDING:
			SetDlgItemText( IDC_STATUS, _T( "Service is entering in pause."));
			break;
		case SERVICE_PAUSED:
			SetDlgItemText( IDC_STATUS, _T( "Service is paused."));
			break;
		default:
			SetDlgItemText( IDC_STATUS, _T( "Unknown state !."));
			break;
		}
		GetDlgItem( IDC_START)->EnableWindow( FALSE);
		GetDlgItem( IDC_STOP)->EnableWindow( FALSE);
		break;
	case IDI_RED_ICON: // Service stoppped
		SetDlgItemText( IDC_STATUS, _T( "Service is stopped."));
		GetDlgItem( IDC_START)->EnableWindow( TRUE);
		GetDlgItem( IDC_STOP)->EnableWindow( FALSE);
		break;
	case IDI_ERROR_ICON: // Error
	default:
		SetDlgItemText( IDC_STATUS, _T( "Service is not installed or error encountered !"));
		GetDlgItem( IDC_START)->EnableWindow( FALSE);
		GetDlgItem( IDC_STOP)->EnableWindow( FALSE);
		break;
	}
	return TRUE;
}

void CNTSyslogCtrlDlg::OnSyslogd() 
{
	// TODO: Add your control notification handler code here
	CSyslogDaemonDlg	cDlg;

	cDlg.SetComputer( m_csComputer);
	cDlg.DoModal();
}

void CNTSyslogCtrlDlg::OnEventLog()
{
	CString filter;
	CConfigLogging cDlg;

	// Get pointers to the combo boxes.
	CComboBox *peType = (CComboBox *)GetDlgItem(IDC_EVENTLOG_SELECT);

	// If doesn't exist, stop rather than crashing.  Should never happen.
	if(peType == NULL)
	{
		AfxMessageBox("Strange error: Selection box does not exist??", MB_ICONSTOP);
		return;
	}

	// Get the selection.  Depends on the dialog having the right items in it.
	int cur_index = peType->GetCurSel();
	
	// Again, make sure we got values back.
	if(cur_index == CB_ERR)
	{
		AfxMessageBox("There are no filters currently defined.", MB_ICONSTOP);
		return;
	}

//	cDlg.SetupDialog(m_csaEventlogSelect.GetAt(cur_index), m_csComputer);
	filter.Format( _T("Filter%d"),m_cuiaFilterMap.GetAt(cur_index));
	cDlg.SetupDialog(filter, m_csComputer);

	cDlg.DoModal();
	//reload the filter list in case names have changed etc
	ReadMachineEventLogSettings(TRUE);
	peType->SetCurSel(cur_index);

}

void CNTSyslogCtrlDlg::SetComputerName()
{
	CString	csMessage;

	csMessage.Format( _T( "Service status on computer <%s>..."),
					 (m_csComputer.IsEmpty() ? _T( "Local Machine") : m_csComputer));
	SetDlgItemText( IDC_COMPUTER, csMessage);
}

void CNTSyslogCtrlDlg::OnStartService() 
{
	// TODO: Add your control notification handler code here
	CNTScmService			 myService;
	CNTServiceControlManager mySCM;
	CString					 csComputer;
	CWaitCursor				 cWait;

	if (!m_csComputer.IsEmpty())
		// Set computer in \\computer_name format
		csComputer.Format( _T( "\\\\%s"), m_csComputer);
	else
		csComputer.Empty();
	if (!mySCM.Open( csComputer, SC_MANAGER_ALL_ACCESS|GENERIC_READ))
	{
		AfxMessageBox( _T( "Unable to contact Service Control Manager !"), MB_ICONSTOP);
		return;
	}
	if (!mySCM.OpenService( NTSYSLOG_SERVICE_NAME, SERVICE_START, myService))
	{
		mySCM.Close();
		AfxMessageBox( _T( "Unable to send command to Service Control Manager !"), MB_ICONSTOP);
		return;
	}
	if (!myService.Start( 0, NULL))
	{
		myService.Close();
		mySCM.Close();
		AfxMessageBox( _T( "Error while sending command to Service Control Manager !"), MB_ICONSTOP);
		return;
	}
	myService.Close();
	mySCM.Close();
	QueryServiceStatus();
}

void CNTSyslogCtrlDlg::OnStopService() 
{
	// TODO: Add your control notification handler code here
	CNTScmService			 myService;
	CNTServiceControlManager mySCM;
	CString					 csComputer;
	CWaitCursor				 cWait;

	if (!m_csComputer.IsEmpty())
		// Set computer in \\computer_name format
		csComputer.Format( _T( "\\\\%s"), m_csComputer);
	else
		csComputer.Empty();
	if (!mySCM.Open( csComputer, SC_MANAGER_ALL_ACCESS|GENERIC_READ))
	{
		AfxMessageBox( _T( "Unable to contact Service Control Manager !"), MB_ICONSTOP);
		return;
	}
	if (!mySCM.OpenService( NTSYSLOG_SERVICE_NAME, SERVICE_STOP, myService))
	{
		mySCM.Close();
		AfxMessageBox( _T( "Unable to send command to Service Control Manager !"), MB_ICONSTOP);
		return;
	}
	if (!myService.Stop())
	{
		myService.Close();
		mySCM.Close();
		AfxMessageBox( _T( "Error while sending command to Service Control Manager !"), MB_ICONSTOP);
		return;
	}
	myService.Close();
	mySCM.Close();
	QueryServiceStatus();
}

void CNTSyslogCtrlDlg::OnCreate() 
{
		HKEY		hKeyRemote,
				hKeySoftware,
				hKeySyslog,
				hKeySection;
	DWORD		dwSize,
				dwValue;
	CString csRegPath, csItem;
	CConfigLogging cDlg;

	// create a new filter::

	// increment maxfilter and save
	maxfilter++;
	csRegPath.Format(_T("Filter%d"),maxfilter);
	
	// Connect to the registry on HKLM
	if (RegConnectRegistry( (char*)((LPCTSTR)m_csComputer), HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error while connecting to the registry !\n\nPlease retry."), MB_ICONSTOP);
		return;
	}
	// Create the SOFTWARE\Cheshire Cat key or open it if it exists
	if (RegCreateKeyEx( hKeyRemote, NTSYSLOG_SOFTWARE_KEY, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
						KEY_WRITE|KEY_READ, NULL, &hKeySoftware, &dwValue) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writing Developer subkey!\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeyRemote);
		return;
	}
	// Create the Syslog subkey or open it if it exists
	if (RegCreateKeyEx( hKeySoftware, NTSYSLOG_SYSLOG_KEY, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
						KEY_WRITE|KEY_READ, NULL, &hKeySyslog, &dwValue) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writing Nagios subkey!\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	dwValue = maxfilter;
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySyslog, MAXFILTER_ENTRY, 0, REG_DWORD, (LPBYTE) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writing new maxfilter counter!\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	// Create the appropriate subkey or open it if it exists
	if (RegCreateKeyEx( hKeySyslog, csRegPath, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
						KEY_WRITE|KEY_READ, NULL, &hKeySection, &dwValue) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error creating new filter subkey!\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	// create new reg key
	csItem.Format(_T("New Item %d"),maxfilter);
	dwSize = csItem.GetLength();
	if (RegSetValueEx( hKeySection, FILTERDESC_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csItem), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writing new filter parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
		return;
	}

	// new blank record

	dwSize = sizeof (DWORD);
	dwValue = 0;
	if (RegSetValueEx( hKeySection, EVENTLOG_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS) 
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
		return;
	}
		csItem = "Unknown";
		dwSize = csItem.GetLength();
		if (RegSetValueEx( hKeySection, EVENTLOGNAME_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csItem), dwSize) != ERROR_SUCCESS) 
		{
			AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
			RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
			return;
		}


	csItem = "Service Name";
	dwSize = csItem.GetLength();
	if (RegSetValueEx( hKeySection, SERVICENAME_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csItem), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
		return;
	}
	csItem = "";
	dwSize = csItem.GetLength();
	if (RegSetValueEx( hKeySection, ID_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csItem), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
		return;
	}
	csItem="";
	dwSize = csItem.GetLength();
	if (RegSetValueEx( hKeySection, MATCH_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csItem), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
		return;
	}
	csItem="";
	dwSize = csItem.GetLength();
	if (RegSetValueEx( hKeySection, SOURCE_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csItem), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
		return;
	}

	dwValue = 0;
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, INFORMATION_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	dwValue = 0;
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, STATUS_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	dwValue = 0;
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, WARNING_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	dwValue = 0;
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, ERROR_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	dwValue = 0;
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, AUDIT_SUCCESS_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	dwValue = 0;
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, AUDIT_FAILURE_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	dwValue = 0;
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, NOTEVENT_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	dwValue = 0;
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, NOTMATCH_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}
	dwValue = 0;
	dwSize = sizeof (DWORD);
	if (RegSetValueEx( hKeySection, NOTSOURCE_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	RegCloseKey( hKeySection);
	RegCloseKey( hKeySyslog);
	RegCloseKey( hKeySoftware);
	RegCloseKey( hKeyRemote);

//	AfxMessageBox( _T( "Created new filter."), MB_ICONINFORMATION);


	cDlg.SetupDialog(csRegPath, m_csComputer);
	cDlg.DoModal();

	// reload the filters
	ReadMachineEventLogSettings(TRUE);
	CComboBox *peType = (CComboBox *)GetDlgItem(IDC_EVENTLOG_SELECT);
	peType->SetCurSel(m_csaEventlogSelectSize - 1);

}

void CNTSyslogCtrlDlg::OnDelete() 
{
	// delete currently selected filter (after confirmation of course)
	CString csTitle = _T("Confirm Delete"); //NEW
	CString csFmt((LPCTSTR)IDS_CONFIRM_DELETE);
	CString csUpdate;
	csUpdate.Format(csFmt, (LPCTSTR)csTitle);
	CString	csFilter;
		HKEY		hKeyRemote,
				hKeySoftware,
				hKeySyslog;
	DWORD			dwValue;

	if ((AfxMessageBox( csUpdate, MB_YESNO|MB_ICONWARNING) == IDNO))
	{ return; }

	//delete the thing
		// Connect to the registry on HKLM
	if (RegConnectRegistry( (char*)((LPCTSTR)m_csComputer), HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error while connecting to the registry !\n\nPlease retry."), MB_ICONSTOP);
		return;
	}
	// Create the SOFTWARE\Cheshire Cat key or open it if it exists
	if (RegCreateKeyEx( hKeyRemote, NTSYSLOG_SOFTWARE_KEY, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
						KEY_WRITE|KEY_READ, NULL, &hKeySoftware, &dwValue) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeyRemote);
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

		// Get pointers to the combo boxes.
	CComboBox *peType = (CComboBox *)GetDlgItem(IDC_EVENTLOG_SELECT);

	// If doesn't exist, stop rather than crashing.  Should never happen.
	if(peType == NULL)
	{
		AfxMessageBox("Program error: Dialog now missing controls.  This "
				"message should never appear.", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	// Get the selection.  Depends on the dialog having the right items in it.
	int cur_index = peType->GetCurSel();
	
	// Again, make sure we got values back.
	if(cur_index == CB_ERR)
	{
		AfxMessageBox("There is no filter to delete.", MB_ICONSTOP);
		return;
	}

//	cDlg.SetupDialog(m_csaEventlogSelect.GetAt(cur_index), m_csComputer);
	csFilter.Format( _T("Filter%d"),m_cuiaFilterMap.GetAt(cur_index));

	RegDeleteKey( hKeySyslog, csFilter );
	RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);

	//reload the filter list.
	ReadMachineEventLogSettings(TRUE);
}

void CNTSyslogCtrlDlg::OnClearall() 
{
	CString csTitle = _T("Confirm Delete"); 
	CString csFmt((LPCTSTR)IDS_CONFIRM_DELETE);
	CString csUpdate;
	csUpdate.Format(csFmt, (LPCTSTR)csTitle);
	CString	csKeyName;
	HKEY		hKeyRemote, hKeySyslog;
	DWORD		dwValue, dwType, dwSize;
	int		maxfilter;

	if ((AfxMessageBox( csUpdate, MB_YESNO|MB_ICONWARNING) == IDNO))
	{ return; }

	// loop through all deleting them
	if (RegConnectRegistry( (char*)(LPCTSTR)m_csComputer, HKEY_LOCAL_MACHINE, &hKeyRemote) == ERROR_SUCCESS)
	{
		csKeyName.Format( _T( "%s\\%s"), NTSYSLOG_SOFTWARE_KEY, NTSYSLOG_SYSLOG_KEY);
		if (RegOpenKeyEx( hKeyRemote, csKeyName, 0, KEY_WRITE|KEY_READ, &hKeySyslog) == ERROR_SUCCESS)	
		{
			dwSize = sizeof( DWORD);
			maxfilter = -1;

			if (RegQueryValueEx( hKeySyslog, MAXFILTER_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS)
			{ maxfilter = (int)dwValue; }


			//loop until done reading all subkeys
			for(int index = 0; index <= maxfilter; index++ )
			{
				csKeyName.Format( _T("Filter%d"),index);
				RegDeleteKey( hKeySyslog, csKeyName );
			}

			// set maxfilter
			dwValue = 0L;
			dwSize = sizeof (DWORD);
			if (RegSetValueEx( hKeySyslog, MAXFILTER_ENTRY, 0, REG_DWORD, (LPBYTE) &dwValue, dwSize) != ERROR_SUCCESS)
			{
				AfxMessageBox( _T( "Error writing new maxfilter count!\n\nPlease retry."), MB_ICONSTOP);
			}
			
			//don't need the handles to the Registry anymore
			RegCloseKey(hKeySyslog);
		}//end if (RegOpenKeyEx( hKeyRemote, EVENTLOG_REG_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		RegCloseKey(hKeyRemote);
	} else {
		AfxMessageBox( _T( "Unable to connect to registy!\n\nPlease retry."), MB_ICONSTOP);
	}
	//reload the filter list.
	ReadMachineEventLogSettings(TRUE);

}

void CNTSyslogCtrlDlg::SwapFilters(int a, int b)
{
	CString csPrvFilter;
	CString	csFilter;
	CString csItem;
	CString csRegKeyA, csRegKeyB;
	HKEY		hKeya,hKeyb,hKeyRemote;
	DWORD			dwTypea,dwTypeb,dwLength,dwSizea, dwSizeb;
	long	i;
	char buffer[2048], databuffer[1024], databuffer2[1024];


	// Connect to the registry on HKLM
	if (RegConnectRegistry( (char*)((LPCTSTR)m_csComputer), HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error while connecting to the registry !\n\nPlease retry."), MB_ICONSTOP);
		return;
	}
	csRegKeyA.Format(_T("%s\\%s\\Filter%d"),NTSYSLOG_SOFTWARE_KEY, NTSYSLOG_SYSLOG_KEY, a );
	csRegKeyB.Format(_T("%s\\%s\\Filter%d"),NTSYSLOG_SOFTWARE_KEY, NTSYSLOG_SYSLOG_KEY, b );

	if( RegOpenKeyEx( hKeyRemote, csRegKeyA, 0, KEY_READ|KEY_WRITE, &hKeya) == ERROR_SUCCESS) 
	{
		if (RegOpenKeyEx( hKeyRemote, csRegKeyB, 0, KEY_READ|KEY_WRITE, &hKeyb) == ERROR_SUCCESS) 
		{
			// we need to swap ALL values in these keys.
			// this will not be a problem unless one of them is newly created and
			// therefore has no options set yet.  We can fix this by making a create go
			// straight to an update...
			i=0;
			dwLength = sizeof(buffer);
			dwSizea = sizeof(databuffer);
			while (RegEnumValue(hKeya, i++, buffer, &dwLength,0,&dwTypea,(unsigned char *)databuffer,&dwSizea)
				== ERROR_SUCCESS) {
//				if (RegQueryValueEx( hKeya, buffer, 0, &dwTypea, (LPBYTE) databuffer, &dwSizea) != ERROR_SUCCESS) {
//					databuffer2[0] = (char)0; dwSizea = 0;
//				}
				dwTypeb = dwTypea; dwSizeb = sizeof(databuffer2);
				if (RegQueryValueEx( hKeyb, buffer, 0, &dwTypeb, (LPBYTE) databuffer2, &dwSizeb) != ERROR_SUCCESS) {
					databuffer2[0] = (char)0; dwSizeb = 0;
				}
				if( dwTypea != dwTypeb ) {
					AfxMessageBox( _T( "Registry type mismatch!\n\nFilter definitions may be corrupted."), MB_ICONSTOP);
					return;				}
				if (RegSetValueEx( hKeya, buffer, 0, dwTypeb, (LPBYTE) databuffer2, dwSizeb) != ERROR_SUCCESS) {
					AfxMessageBox( _T( "Error while updating registry!\n\nFilter definitions may be corrupted."), MB_ICONSTOP);
					RegCloseKey ( hKeyRemote );
					RegCloseKey ( hKeya );
					RegCloseKey ( hKeyb );
					return;
				}
				if (RegSetValueEx( hKeyb, buffer, 0, dwTypea, (LPBYTE) databuffer,  dwSizea) != ERROR_SUCCESS) {
					AfxMessageBox( _T( "Error while updating registry!\n\nFilter definitions may be corrupted."), MB_ICONSTOP);
					RegCloseKey ( hKeyRemote );
					RegCloseKey ( hKeya );
					RegCloseKey ( hKeyb );
					return;
				}
				dwLength= sizeof(buffer);dwSizea = sizeof(databuffer); // dont forget to reset this!
				databuffer[0] = (char)0;
			}
			// close down
			RegCloseKey ( hKeyb );
		} else {
			AfxMessageBox( _T( "Error while accessing registry !\n\nPlease retry."), MB_ICONSTOP);
		}
		RegCloseKey ( hKeya );
	} else {
		AfxMessageBox( _T( "Error while accessing registry !\n\nPlease retry."), MB_ICONSTOP);
	}
	RegCloseKey ( hKeyRemote );
}

void CNTSyslogCtrlDlg::OnMoveup() 
{

		// Get pointers to the combo boxes.
	CComboBox *peType = (CComboBox *)GetDlgItem(IDC_EVENTLOG_SELECT);

	// If doesn't exist, stop rather than crashing.  Should never happen.
	if(peType == NULL)
	{
		AfxMessageBox("Program error: Dialog now missing controls.  This "
				"message should never appear.", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	// Get the selection.  Depends on the dialog having the right items in it.
	int cur_index = peType->GetCurSel();
	
	// Again, make sure we got values back.
	if(cur_index == CB_ERR)
	{
		AfxMessageBox("There is no filter to move.", MB_ICONSTOP);
		return;
	}
	if( cur_index == 0 ) {
		AfxMessageBox("Filter is already at top of list.", MB_ICONSTOP);
		return;
	}

	SwapFilters( m_cuiaFilterMap.GetAt(cur_index), m_cuiaFilterMap.GetAt(cur_index-1));
	//reload the filter list.
	ReadMachineEventLogSettings(TRUE);	
	peType->SetCurSel(cur_index-1);

}

void CNTSyslogCtrlDlg::OnMovedown() 
{
			// Get pointers to the combo boxes.
	CComboBox *peType = (CComboBox *)GetDlgItem(IDC_EVENTLOG_SELECT);

	// If doesn't exist, stop rather than crashing.  Should never happen.
	if(peType == NULL)
	{
		AfxMessageBox("Program error: Dialog now missing controls.  This "
				"message should never appear.", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	// Get the selection.  Depends on the dialog having the right items in it.
	int cur_index = peType->GetCurSel();
	
	// Again, make sure we got values back.
	if(cur_index == CB_ERR)
	{
		AfxMessageBox("There is no filter to move.", MB_ICONSTOP);
		return;
	}
	if( cur_index > ((int)m_csaEventlogSelectSize - 2) ) {
		AfxMessageBox("Filter is already at bottom of list.", MB_ICONSTOP);
		return;
	}

	SwapFilters( m_cuiaFilterMap.GetAt(cur_index), m_cuiaFilterMap.GetAt(cur_index+1) );

	//reload the filter list.
	ReadMachineEventLogSettings(TRUE);	
	peType->SetCurSel(cur_index+1);
	
}

void CNTSyslogCtrlDlg::OnGenevent() 
{
	FGenEvent cDlg;
	if( cDlg.DoModal() != IDCANCEL ) {
		AfxMessageBox("EventLog message created!", MB_ICONINFORMATION);
	}
}

void CNTSyslogCtrlDlg::OnKillfocusDelay() 
{
	// Save the value now it has been changed, if it is valid.
	HKEY		hKeyRemote, hKeySyslog,
				hKeySoftware;
	DWORD		dwSize, dwValue, dwDelay, dwType;
	CString		csDelay;

	if (GetDlgItemText( IDC_DELAY, csDelay) == 0)	{
		dwDelay = 30;
	} else {
		dwDelay = atoi(csDelay);
		if( dwDelay < 10 ) { 
			AfxMessageBox( _T( "Delay parameter too low.  Resetting to default."), MB_ICONWARNING);
			dwDelay = 30; 
		}
	}
	csDelay.Format(_T("%d"),dwDelay);
	SetDlgItemText(IDC_DELAY,csDelay);

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
		AfxMessageBox( _T( "Error writing registry key!\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey (hKeyRemote);
		return;
	}
		// Create the Syslog subkey or open it if it exists
	if (RegCreateKeyEx( hKeySoftware, NTSYSLOG_SYSLOG_KEY, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
						KEY_WRITE|KEY_READ, NULL, &hKeySyslog, &dwValue) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writing registry subkey!\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	dwSize = sizeof(DWORD);
	if (RegQueryValueEx( hKeySyslog, DELAY_ENTRY, 0, &dwType, (LPBYTE)&dwValue, &dwSize) != ERROR_SUCCESS) {	
		dwValue = -1;			
	}
	if( dwValue != dwDelay ) {
		dwSize = sizeof(DWORD);
		dwValue = dwDelay;
		if(RegSetValueEx( hKeySyslog, DELAY_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
		{
			AfxMessageBox( _T( "Error writing new parameters!\n\nPlease retry."), MB_ICONSTOP);
				RegCloseKey (hKeySyslog);
			RegCloseKey (hKeySoftware);
			RegCloseKey( hKeyRemote);
			return;
		}
		AfxMessageBox( _T( "Delay parameter updated in registry.\nYou now need to restart the daemon for it to take effect."), MB_ICONINFORMATION);
	}
}




void CAboutDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CNTSyslogCtrlDlg::UpdateHBEntry()
{
	// Save the value now it has been changed, if it is valid.
	HKEY		hKeyRemote, hKeySyslog,
				hKeySoftware;
	DWORD		dwSize, dwValue, dwFreq, dwFreqold, dwType;
	CString		csMsg,csFreq, csSvc, csSvcold;
	TCHAR		szBuffer[255];
	short		didsomething = 0;

	if (GetDlgItemText( IDC_HBFREQ, csFreq) == 0)
	{
		dwFreq = 10;
	} else {
		dwFreq = atoi(csFreq);
		if( dwFreq < 1 ) { 
			AfxMessageBox( _T( "Frequency parameter too low.  Resetting to default."), MB_ICONWARNING);
			dwFreq = 10; 
		}
		if( dwFreq > 999 ) { 
			AfxMessageBox( _T( "Warning - frequency parameter is very high.  To disable, set the Service name to blank."), MB_ICONWARNING);
		}
	}
	csFreq.Format(_T("%d"),dwFreq);
	SetDlgItemText(IDC_HBFREQ,csFreq);
	if (GetDlgItemText( IDC_HBSVC, csSvc) == 0)	{
		csSvc.Format(_T(""));
	}

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
		AfxMessageBox( _T( "Error writing registry key!\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey (hKeyRemote);
		return;
	}
		// Create the Nagios subkey or open it if it exists
	if (RegCreateKeyEx( hKeySoftware, NTSYSLOG_SYSLOG_KEY, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
						KEY_WRITE|KEY_READ, NULL, &hKeySyslog, &dwValue) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writing registry subkey!\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}

	dwSize = sizeof(DWORD);
	if (RegQueryValueEx( hKeySyslog, HBFREQ_ENTRY, 0, &dwType, (LPBYTE)&dwValue, &dwSize) != ERROR_SUCCESS) {	
		dwFreqold = -1;			
	} else {
		dwFreqold = dwValue;
	}
	if( dwFreq != dwFreqold ) {
		dwSize = sizeof(DWORD);
		dwValue = dwFreq;
		if (RegSetValueEx( hKeySyslog, HBFREQ_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS)
		{
			AfxMessageBox( _T( "Error writing new parameters!\n\nPlease retry."), MB_ICONSTOP);
				RegCloseKey (hKeySyslog);
			RegCloseKey (hKeySoftware);
			RegCloseKey( hKeyRemote);
			return;
		}
//		csMsg.Format(_T("Frequency was %d, now %d"),dwFreqold,dwFreq);
//		AfxMessageBox( csMsg, MB_ICONSTOP);

		didsomething = 1;
	}
	dwSize = sizeof(szBuffer)-1;
	if (RegQueryValueEx( hKeySyslog, HBSVC_ENTRY, 0, &dwType, (unsigned char *) szBuffer, &dwSize) != ERROR_SUCCESS) {	
		memset(szBuffer,0,sizeof(szBuffer));
	}
	if( strcmp(szBuffer,csSvc) ) {
		dwSize = csSvc.GetLength();
		if (RegSetValueEx( hKeySyslog, HBSVC_ENTRY, 0, REG_SZ, (LPBYTE ) LPCSTR(csSvc), dwSize) != ERROR_SUCCESS)
		{
			AfxMessageBox( _T( "Error writing new parameters!\n\nPlease retry."), MB_ICONSTOP);
				RegCloseKey (hKeySyslog);
			RegCloseKey (hKeySoftware);
			RegCloseKey( hKeyRemote);
			return;
		}
//		csMsg.Format(_T("Servicename was '%s', now '%s'"),szBuffer,csSvc);
//		AfxMessageBox( csMsg, MB_ICONSTOP);
		didsomething = 1;
	}
	if(didsomething) {
		AfxMessageBox( _T( "Heartbeat configuration updated in registry.\nYou now need to restart the daemon for it to take effect."), MB_ICONINFORMATION);
	}
}


void CNTSyslogCtrlDlg::OnEnKillfocusHbsvc()
{
	UpdateHBEntry();
	return;
}

void CNTSyslogCtrlDlg::OnEnKillfocusHbfreq()
{
	UpdateHBEntry();
	return;
}

void CNTSyslogCtrlDlg::exportreg(int doall,CString filename)
{
	FILE *fp;
	int maxfilter, i, f;
	CString	csFilter;
	CString csItem;
	CString csRegKey;
	CString csMsg;
	HKEY		hKey,hKeyRemote;
	DWORD			dwValue,dwType,dwLength,dwSize;
	char buffer[2048], databuffer[1024];

	fp = fopen(filename,"w");
	if(!fp) {
		csMsg.Format(_T("Unable to write to '%s'"),filename);
		AfxMessageBox( csMsg, MB_ICONSTOP);
		return;
	}

	// open registry
	if (RegConnectRegistry( (char*)((LPCTSTR)m_csComputer), HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error while connecting to the registry !\n\nPlease retry."), MB_ICONSTOP);
		return;
	}
	
	// print headers
	fprintf(fp,"Nagios EventLog Agent configuration v1.8\n\n");
	fprintf(fp,"[HKEY_LOCAL_MACHINE\\%s\\%s]\n",NTSYSLOG_SOFTWARE_KEY,NTSYSLOG_SYSLOG_KEY);
	
	// connect to branch
	csRegKey.Format(_T("%s\\%s"),NTSYSLOG_SOFTWARE_KEY, NTSYSLOG_SYSLOG_KEY);
	if( RegOpenKeyEx( hKeyRemote, csRegKey, 0, KEY_READ|KEY_WRITE, &hKey) != ERROR_SUCCESS) { 
		AfxMessageBox( _T( "Unable to find registry settings."), MB_ICONSTOP);
		fclose(fp);
		return;
	}
	// fetch main value
	dwSize = sizeof(DWORD);
	if (RegQueryValueEx( hKey, MAXFILTER_ENTRY, 0, &dwType, (LPBYTE)&dwValue, &dwSize) != ERROR_SUCCESS) {	
		maxfilter = -1;			
	} else {
		maxfilter = dwValue;
	}
	RegCloseKey(hKey);
	fprintf(fp,"\"maxFilter\"=dword:%08x\n",(long)maxfilter);

	// loop through filters
	for( f=0; f<=maxfilter; f++ ) {
		// read filter
		csRegKey.Format(_T("%s\\%s\\Filter%d"),NTSYSLOG_SOFTWARE_KEY, NTSYSLOG_SYSLOG_KEY, f );
		if( RegOpenKeyEx( hKeyRemote, csRegKey, 0, KEY_READ|KEY_WRITE, &hKey) != ERROR_SUCCESS) { continue; }	
		// print filter
		fprintf(fp,"\n[HKEY_LOCAL_MACHINE\\%s\\%s\\Filter%d]\n",NTSYSLOG_SOFTWARE_KEY,NTSYSLOG_SYSLOG_KEY,f);
		i=0;
		dwLength = sizeof(buffer);
		dwSize = sizeof(databuffer);
		while (RegEnumValue(hKey, i++, buffer, &dwLength,0,&dwType,(unsigned char *)databuffer,&dwSize)	== ERROR_SUCCESS) {
			// print leaf
			if( dwType == REG_DWORD ) {
				fprintf(fp,"\"%s\"=dword:%08x\n",buffer,(long)*((DWORD *)databuffer));
			} else {
				fprintf(fp,"\"%s\"=\"%s\"\n",buffer,databuffer);
			}
			dwLength = sizeof(buffer);
			dwSize = sizeof(databuffer);
			buffer[0] = databuffer[0] = (char)0;
		}
		RegCloseKey(hKey);
	}

	// tidy up
	fprintf(fp,"\n\n");
	fclose(fp);
	csMsg.Format(_T("Saved %s registry data to '%s'"),(doall?"configuration":"filters"),filename);
	AfxMessageBox( csMsg, MB_ICONINFORMATION);
}

void CNTSyslogCtrlDlg::OnBnClickedExport()
{
// Export current filters to a .reg file.
// First prompt user to select a file.
	char szFilters[]= "Registry Export Files (*.reg)|*.reg|All Files (*.*)|*.*||";
	CString pathName, csMsg;

   // Create an Open dialog; the default file name extension is ".my".
   CFileDialog fileDlg (FALSE, "reg", "filters.reg", OFN_OVERWRITEPROMPT| OFN_HIDEREADONLY, szFilters);
   
   if( fileDlg.DoModal()==IDOK )
   {
		pathName=fileDlg.GetPathName();
		exportreg(0,pathName);
   }

}
