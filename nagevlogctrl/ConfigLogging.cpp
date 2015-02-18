// ConfigLogging.cpp : implementation file
//

#include "stdafx.h"
#include "NTSyslogCtrl.h"
#include "NTService.h"
#include "ConfigLogging.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigLogging dialog

// Constructor.  Set up sensible values.
CConfigLogging::CConfigLogging(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigLogging::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigLogging)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//--OLD--m_idTitle = -1;
	//--OLD--m_idRegpath = -1;

	m_uCurrentState = CHECK_NOT_ENABLED;
//	m_uInfoPriority = DEFAULT_PRIORITY;
//	m_uWarningPriority = DEFAULT_PRIORITY;
//	m_uErrorPriority = DEFAULT_PRIORITY;
//	m_uAuditSuccessPriority = DEFAULT_PRIORITY;
//	m_uAuditFailurePriority = DEFAULT_PRIORITY;
	m_uStatus = DEFAULT_STATUS;

}


void CConfigLogging::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigLogging)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigLogging, CDialog)
	//{{AFX_MSG_MAP(CConfigLogging)

	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigLogging message handlers

BOOL CConfigLogging::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWaitCursor	cWait;
	HKEY		hKeyRemote,
				hKey;
	DWORD		dwSize,
				dwType,
				dwValue;
	CString		csKeyName;
	CStringArray __tmp		;
	TCHAR		szBuffer[255];

	// Check to see we were set up properly...
	//--OLD--if(m_idTitle == NULL || m_idRegpath == NULL)
	if(m_idTitle.IsEmpty() || m_idRegpath.IsEmpty() ) //NEW
	{
			AfxMessageBox("Program error: Dialog incorrectly set up.  This "
					"message should never appear. (1)", MB_ICONSTOP);
			CDialog::OnCancel();
			return TRUE;
	}

	// Get the human readable title, and load it in to the right places in
	// the dialog...
	//////////////////////////////////////////////////////////////////////////////////////
	//Need to change this to accept CStrings instead of type INT to allow for dynamic
	//processing of custom EventLogs because type INT won't do it the way
	//I want it to
	//--OLD--CString csTitle((LPCTSTR)m_idTitle);
	CString csTitle = m_idTitle; //NEW
	CString csFmt;
	CString csUpdate;

	//GetWindowText(csFmt);
	//csUpdate.Format(csFmt, (LPCSTR)csTitle);
	//SetWindowText(csUpdate);

	CWnd *pLabel = GetDlgItem(IDC_LABEL);
	pLabel->GetWindowText(csFmt);
	csUpdate.Format(csFmt, (LPCTSTR)csTitle);
	pLabel->SetWindowText(csUpdate);

	// Set everything up with empty, default values.
	m_uCurrentState = CHECK_NOT_ENABLED;

	// Load the name of the key we're working from.
	//--OLD--CString csRegPath((LPCTSTR)m_idRegpath);
	CString csRegPath = m_idRegpath; //NEW
	
	// Open the registry on HKLM
	//Not sure if this is necessary anymore because registry connection
	//is verified upon selecting the server in the select server dialog
	if (RegConnectRegistry( (char*)((LPCTSTR)m_csComputer), HKEY_LOCAL_MACHINE, &hKeyRemote) != ERROR_SUCCESS)
	{
		csKeyName.Format( _T( "Error while connecting to the registry !\n\nEnsure that\n\n%s\n%s"),
						  _T( "1) Network Registry access is enabled if this is a remote computer."),
						  _T( "2) You have Administrator privilieges on the computer."));
		AfxMessageBox( csKeyName, MB_ICONSTOP);
		CDialog::OnCancel();
		return TRUE;
	}

	// Get pointer to the combo boxe.
	CComboBox *peType = (CComboBox *)GetDlgItem(IDC_EVENTLOG);
	peType->ResetContent();

	// If it doesn't exist stop rather than crashing.  Should never happen.
	if(peType == NULL)
	{
		AfxMessageBox("Program error: Dialog now missing controls.  This "
				"message should never appear.", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	//reset the EventlogSelect property & free memory to avoid memory leaks
	m_csaEventlogSelect.RemoveAll();
	m_csaEventlogSelect.FreeExtra();

	//set the EventlogSelect size to the default
	m_csaEventlogSelectSize = 1;

	__tmp.Add(_T("All Logs"));

	//Open the key to where Windows stores EventLog info
	if (RegOpenKeyEx( hKeyRemote, EVENTLOG_REG_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		//Read the subkeys in HKLM\System\CurrentControlSet\Services\Eventlog
		DWORD no_subkeys, subkey_max_len;
		if(::RegQueryInfoKey(hKey,NULL,NULL,NULL,&no_subkeys,&subkey_max_len,NULL,NULL,NULL,NULL,NULL,NULL) == ERROR_SUCCESS )
		{
			subkey_max_len++;
			m_csaEventlogSelectSize = no_subkeys+1;

			//loop until done reading all subkeys
			for(DWORD index = 0; index < no_subkeys; index++ )
			{
				CString buffer;
				DWORD buffer_size = subkey_max_len;

				LONG retval = RegEnumKeyEx( hKey, index, buffer.GetBuffer(buffer_size), &buffer_size, NULL, NULL, NULL, NULL);
				if(retval == ERROR_SUCCESS && retval != ERROR_NO_MORE_ITEMS)
				{
					__tmp.Add((LPCSTR)buffer);
				}//end if(retval == ERROR_SUCCESS && retval != ERROR_NO_MORE_ITEMS)
			}//end for(DWORD index = 0; index < no_subkeys; index++ )
		}//end if(ReqQueryInfoKey(hKeyRemote,NULL,NULL,NULL,&no_subkeys,&subkey_max_len,NULL,NULL,NUL.NULL,NULL) == ERROR_SUCCESS)
		//don't need the handles to the Registry anymore
		RegCloseKey(hKey);

		

		//populate the m_csaEventlogSelect CString array
		//no apparent need to sort this as RegEnumKeyEx appears
		//to be returning the registry key names in alphabetical order
		for(DWORD i = 0; i <= no_subkeys; i++)
		{
			m_csaEventlogSelect.Add(__tmp.GetAt(i));
		}//end for(DWORD i = 0; i < no_subkeys; i++)
	}//end if (RegOpenKeyEx( hKeyRemote, EVENTLOG_REG_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS)

	//Now need to populate the combo box with data

	for(DWORD i = 0; i < m_csaEventlogSelectSize; i++)
	{
		peType->AddString( m_csaEventlogSelect.GetAt(i));
	}

	// Open the appropriate SOFTWARE\Cheshire Cat\Nagios key
	m_uEventlog = 0; // default

	csKeyName.Format( _T( "%s\\%s\\%s"), NTSYSLOG_SOFTWARE_KEY, NTSYSLOG_SYSLOG_KEY, csRegPath);
	if (RegOpenKeyEx( hKeyRemote, csKeyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		dwSize = 255*sizeof( TCHAR);
		memset( szBuffer, 0, 255*sizeof( TCHAR));
		if (RegQueryValueEx( hKey, FILTERDESC_ENTRY, 0, &dwType, (LPBYTE) szBuffer, &dwSize) == ERROR_SUCCESS)
			SetDlgItemText( IDC_FILTERDESC, szBuffer);
		dwSize = 255*sizeof( TCHAR);
		memset( szBuffer, 0, 255*sizeof( TCHAR));
		if (RegQueryValueEx( hKey, MATCH_ENTRY, 0, &dwType, (LPBYTE) szBuffer, &dwSize) == ERROR_SUCCESS)
			SetDlgItemText( IDC_MATCHSTRING, szBuffer);
		dwSize = 255*sizeof( TCHAR);
		memset( szBuffer, 0, 255*sizeof( TCHAR));
		if (RegQueryValueEx( hKey, SERVICENAME_ENTRY, 0, &dwType, (LPBYTE) szBuffer, &dwSize) == ERROR_SUCCESS)
			SetDlgItemText( IDC_SERVICE_NAME, szBuffer);
				dwSize = 255*sizeof( TCHAR);
		memset( szBuffer, 0, 255*sizeof( TCHAR));
		if (RegQueryValueEx( hKey, ID_ENTRY, 0, &dwType, (LPBYTE) szBuffer, &dwSize) == ERROR_SUCCESS)
			SetDlgItemText( IDC_EVENTID, szBuffer);
				dwSize = 255*sizeof( TCHAR);
		memset( szBuffer, 0, 255*sizeof( TCHAR));
		if (RegQueryValueEx( hKey, SOURCE_ENTRY, 0, &dwType, (LPBYTE) szBuffer, &dwSize) == ERROR_SUCCESS)
			SetDlgItemText( IDC_SOURCE, szBuffer);

		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, EVENTLOG_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS))
		{ m_uEventlog = dwValue; }
		dwSize = 255*sizeof( TCHAR);
		memset( szBuffer, 0, 255*sizeof( TCHAR));
		if (RegQueryValueEx( hKey, EVENTLOGNAME_ENTRY, 0, &dwType, (LPBYTE) szBuffer, &dwSize) == ERROR_SUCCESS)
		{ peType->SelectString(-1,szBuffer); }

		// NSCA event status
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, STATUS_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS))
		{
			m_uStatus = dwValue;
		}
		
		// Forward Information events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, INFORMATION_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			m_uCurrentState += CHECK_INFORMATION;
		}

		// Forward Warning events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, WARNING_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			m_uCurrentState += CHECK_WARNING;
		}


		// Forward Error events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, ERROR_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			m_uCurrentState += CHECK_ERROR;
		}

		// Forward Audit Success events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, AUDIT_SUCCESS_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			m_uCurrentState += CHECK_AUDIT_SUCCESS;
		}


		// Forward Audit Failure events
		dwSize = sizeof( DWORD);
		if ((RegQueryValueEx( hKey, AUDIT_FAILURE_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize) == ERROR_SUCCESS) &&
			(dwValue == 1))
		{
			m_uCurrentState += CHECK_AUDIT_FAILURE;
		}

		dwSize = sizeof( DWORD); dwValue = 0;
		RegQueryValueEx( hKey, NOTEVENT_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize);
		CheckDlgButton( IDC_NOTEVENT, dwValue);

		dwSize = sizeof( DWORD); dwValue = 0;
		RegQueryValueEx( hKey, NOTSOURCE_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize);
		CheckDlgButton( IDC_NOTSOURCE, dwValue);

		dwSize = sizeof( DWORD); dwValue = 0;
		RegQueryValueEx( hKey, NOTMATCH_ENTRY, 0, &dwType, (LPBYTE) &dwValue, &dwSize);
		CheckDlgButton( IDC_NOTMATCH, dwValue);

		RegCloseKey(hKey);
	}
	else	// If the key does not exist, then use the default values.
	{
		m_uCurrentState = m_uDefaultChecks;
		m_uEventlog = 0;
		peType->SetCurSel(0);
		AfxMessageBox("Warning: Registry key did not exist: Using defaults:" + csKeyName, MB_ICONSTOP);
	}


	RegCloseKey(hKeyRemote);
	// Update the dialog box to match the settings loaded from the registry.
	CheckDlgButton( IDC_INFORMATION_CHECK, (m_uCurrentState & CHECK_INFORMATION));
	CheckDlgButton( IDC_WARNING_CHECK, (m_uCurrentState & CHECK_WARNING));
	CheckDlgButton( IDC_ERROR_CHECK, (m_uCurrentState & CHECK_ERROR));
	CheckDlgButton( IDC_AUDIT_SUCCESS_CHECK, (m_uCurrentState & CHECK_AUDIT_SUCCESS));
	CheckDlgButton( IDC_AUDIT_FAILURE_CHECK, (m_uCurrentState & CHECK_AUDIT_FAILURE));

	SetDialogFromPriority(IDC_STATUS, m_uStatus);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	
}

void CConfigLogging::OnOK() 
{
	// Get and format the confirmation dialog message.
	//--OLD--CString csTitle((LPCTSTR)m_idTitle);

	CString csTitle = m_idTitle; //NEW
	CString csFmt((LPCTSTR)IDS_CONFIRM_SAVE);
	CString csUpdate;
	CString csItem;
	csUpdate.Format(csFmt, (LPCTSTR)csTitle);
	
	// Check for changes, and confirm them.
	if (AfxMessageBox( csUpdate, MB_YESNO|MB_ICONQUESTION) == IDNO)
	{		return;	}

	CWaitCursor	cWait;	// Hourglass.

	// Save the changes to the appropriate registry.
	HKEY		hKeyRemote,
				hKeySoftware,
				hKeySyslog,
				hKeySection;
	DWORD		dwSize,
				dwValue;

	// Get the path from the string table.
	//--OLD--CString csRegPath((LPCTSTR)m_idRegpath);
	CString csRegPath = m_idRegpath;

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
	// Create the appropriate subkey or open it if it exists
	if (RegCreateKeyEx( hKeySyslog, csRegPath, 0, REG_NONE, REG_OPTION_NON_VOLATILE,
						KEY_WRITE|KEY_READ, NULL, &hKeySection, &dwValue) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySyslog);
		RegCloseKey( hKeySoftware);
		RegCloseKey( hKeyRemote);
		return;
	}


	// Get pointers to the combo boxes.
	CComboBox *peType = (CComboBox *)GetDlgItem(IDC_EVENTLOG);

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
		AfxMessageBox("Program error: Dialog now returns errors.  This "
				"message should never appear.", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	dwSize = sizeof (DWORD);
	dwValue = cur_index;
	if (RegSetValueEx( hKeySection, EVENTLOG_ENTRY, 0, REG_DWORD, (LPBYTE ) &dwValue, dwSize) != ERROR_SUCCESS) 
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
		return;
	}
//	if( cur_index ) {
		GetDlgItemText(IDC_EVENTLOG,csItem);
		dwSize = csItem.GetLength();
		if (RegSetValueEx( hKeySection, EVENTLOGNAME_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csItem), dwSize) != ERROR_SUCCESS) 
		{
			AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
			RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
			return;
		}
//	} else {
//		RegDeleteValue(hKeySection,EVENTLOGNAME_ENTRY);
//	}

	GetDlgItemText( IDC_FILTERDESC, csItem);
	dwSize = csItem.GetLength();
	if (RegSetValueEx( hKeySection, FILTERDESC_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csItem), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
		return;
	}

	GetDlgItemText( IDC_SERVICE_NAME, csItem);
	dwSize = csItem.GetLength();
	if (RegSetValueEx( hKeySection, SERVICENAME_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csItem), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
		return;
	}
	GetDlgItemText( IDC_EVENTID, csItem);
	dwSize = csItem.GetLength();
	if (RegSetValueEx( hKeySection, ID_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csItem), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
		return;
	}
	GetDlgItemText( IDC_MATCHSTRING, csItem);
	dwSize = csItem.GetLength();
	if (RegSetValueEx( hKeySection, MATCH_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csItem), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
		return;
	}
		GetDlgItemText( IDC_SOURCE, csItem);
	dwSize = csItem.GetLength();
	if (RegSetValueEx( hKeySection, SOURCE_ENTRY, 0, REG_SZ, (LPBYTE ) LPCTSTR( csItem), dwSize) != ERROR_SUCCESS)
	{
		AfxMessageBox( _T( "Error writting new parameters !\n\nPlease retry."), MB_ICONSTOP);
		RegCloseKey( hKeySection);RegCloseKey( hKeySyslog);RegCloseKey( hKeySoftware);RegCloseKey( hKeyRemote);
		return;
	}



	// Write if we have have to forward Information events
	dwValue = (IsDlgButtonChecked( IDC_INFORMATION_CHECK) ? 1 : 0);
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
	// Write nsca status code
	dwValue = PriorityFromDialog(IDC_STATUS);
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

	// Write if we have have to forward Warning events
	dwValue = (IsDlgButtonChecked( IDC_WARNING_CHECK) ? 1 : 0);
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
	
	
	// Write if we have have to forward Error events
	dwValue = (IsDlgButtonChecked( IDC_ERROR_CHECK) ? 1 : 0);
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

	// Write if we have have to forward Audit Success events
	dwValue = (IsDlgButtonChecked( IDC_AUDIT_SUCCESS_CHECK) ? 1 : 0);
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

	// Write if we have have to forward Audit Failure events
	dwValue = (IsDlgButtonChecked( IDC_AUDIT_FAILURE_CHECK) ? 1 : 0);
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

	dwValue = (IsDlgButtonChecked( IDC_NOTEVENT) ? 1 : 0);
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
	dwValue = (IsDlgButtonChecked( IDC_NOTMATCH) ? 1 : 0);
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
	dwValue = (IsDlgButtonChecked( IDC_NOTSOURCE) ? 1 : 0);
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

	CDialog::OnOK();
}

void CConfigLogging::OnCancel() 
{
	// Load the text from the string table, and format it.
	//--OLD--CString csTitle((LPCTSTR)m_idTitle);

	CString csTitle = m_idTitle; //NEW
	CString csFmt((LPCTSTR)IDS_CONFIRM_DISCARD);
	CString csUpdate;
	csUpdate.Format(csFmt, (LPCTSTR)csTitle);

	// If the data changed, confirm the cancel.
	if (IsModified() && (AfxMessageBox( csUpdate, MB_YESNO|MB_ICONQUESTION) == IDNO))
	{
		return;
	}
	
	CDialog::OnCancel();
}


BOOL CConfigLogging::IsModified()
{
	// Start with sensible values.
	UINT uNewState = CHECK_NOT_ENABLED;

	if (SendDlgItemMessage( IDC_EVENTID, EM_GETMODIFY, 0, 0L))
		return TRUE;
	if (SendDlgItemMessage( IDC_MATCHSTRING, EM_GETMODIFY, 0, 0L))
		return TRUE;
	if (SendDlgItemMessage( IDC_SOURCE, EM_GETMODIFY, 0, 0L))
		return TRUE;
	if (SendDlgItemMessage( IDC_SERVICE_NAME, EM_GETMODIFY, 0, 0L))
		return TRUE;


	// Load values for the checkboxes from the dialog.
	if (IsDlgButtonChecked( IDC_INFORMATION_CHECK))
		uNewState += CHECK_INFORMATION;
	if (IsDlgButtonChecked( IDC_WARNING_CHECK))
		uNewState += CHECK_WARNING;
	if (IsDlgButtonChecked( IDC_ERROR_CHECK))
		uNewState += CHECK_ERROR;
	if (IsDlgButtonChecked( IDC_AUDIT_SUCCESS_CHECK))
		uNewState += CHECK_AUDIT_SUCCESS;
	if (IsDlgButtonChecked( IDC_AUDIT_FAILURE_CHECK))
		uNewState += CHECK_AUDIT_FAILURE;

	return !(uNewState == m_uCurrentState);
}


UINT CConfigLogging::PriorityFromDialog( int severityId)
{
	// Get pointers to the combo boxes.
	CComboBox *pSeverityWnd = (CComboBox *)GetDlgItem(severityId);

	// If one didn't exist, stop rather than crashing.  Should never happen.
	if( pSeverityWnd == NULL)
	{
		AfxMessageBox("Program error: Dialog now missing controls.  This "
				"message should never appear. (3)", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	// Get the selection.  Depends on the dialog having the right items in it.
	int severity = pSeverityWnd->GetCurSel();

	// Again, make sure we got values back.
	if( severity == CB_ERR)
	{
		AfxMessageBox("Program error: Dialog now returns errors.  This "
				"message should never appear. (4)", MB_ICONSTOP);
		CDialog::OnCancel();
	}

	// Glue the values together in to a priority.
	return  severity;
}

void CConfigLogging::SetDialogFromPriority(int severityId, UINT priority)
{
	if(( priority < 0 ) || (priority > 3 )) { priority = DEFAULT_STATUS; }

	// Get pointers to the combo boxes.
	CComboBox *pSeverityWnd = (CComboBox *)GetDlgItem(severityId);

	// If one didn't exist, stop rather than crashing.  Should never happen.
	if( pSeverityWnd == NULL)
	{
		AfxMessageBox("Program error: Dialog missing controls.  This "
				"message should never appear. (2)", MB_ICONSTOP);
		CDialog::OnCancel();
	}


	// Load values in to the combos.  If one can't, then stop.  This might actually
	// happen if someone has got values too large in the registry.
	if( pSeverityWnd->SetCurSel(priority) == CB_ERR)
	{
		AfxMessageBox("Program error: Cannot set status.\n\nCheck "
				"registry to make sure it contains valid values.", MB_ICONSTOP);
		CDialog::OnCancel();
	}
}

//void CConfigLogging::SetupDialog(CString title, CString regpath, LPCTSTR lpstrComputer, UINT defaultChecks)
void CConfigLogging::SetupDialog(CString selection, LPCTSTR lpstrComputer, UINT defaultChecks)
{
	// Store computer name, or set empty.
	if ((lpstrComputer == NULL) || (_tcsclen( lpstrComputer) == 0))
	{
		m_csComputer.Empty();
	}
	else
	{
		// Set computer in \\computer_name format
		m_csComputer.Format( _T( "\\\\%s"), lpstrComputer);
	}
	m_uDefaultChecks = defaultChecks;

	// Store title and registry path IDs for later.
	m_idTitle = selection;
	m_idRegpath = selection;

//OLD
//	m_idTitle = title;
//	m_idRegpath = regpath;
//OLD
}






void CConfigLogging::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
