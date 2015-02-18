//-----------------------------------------------------------------------------
//
// SetupServerDlg.cpp : implementation file
//
//    Copyright (c) 2001, Didier LIROULET - All rights reserved
//
//    This program is free software; you can redistribute it and/or
//    modify it under the terms of the GNU General Public License
//    as published by the Free Software Foundation; either version 2
//    of the License, or (at your option) any later version.
//    
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//    
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307
//
//  Revision history:
//	  08-Dec-2001  DL  Module completed
//
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "NTSyslogCtrl.h"
#include "SetupServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupServerDlg dialog


CSetupServerDlg::CSetupServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupServerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bIsInstall = FALSE;
}


void CSetupServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupServerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupServerDlg, CDialog)
	//{{AFX_MSG_MAP(CSetupServerDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupServerDlg message handlers

BOOL CSetupServerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_bIsInstall)
	{
		SetDlgItemText( IDC_STATUS, _T( "Install NTSyslog service..."));
		SetDlgItemText( IDC_INSTALL, _T( "Enter the name of an existing directory where will be copied the NTSyslog binary file."));
	}
	else
	{
		SetDlgItemText( IDC_STATUS, _T( "Uninstall NTSyslog service..."));
		SetDlgItemText( IDC_INSTALL, _T( "The following NTSyslog binary file will be removed..."));
		((CEdit *)GetDlgItem( IDC_COMPUTER))->SetReadOnly( TRUE);
	}
	SetDlgItemText( IDC_COMPUTER, m_csFolder);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupServerDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CSetupServerDlg::OnOK() 
{
	// TODO: Add extra validation here
	int	nSize = GetDlgItemText( IDC_COMPUTER, m_csFolder);

	if (m_bIsInstall)
	{
		if (nSize == 0)
		{
			AfxMessageBox( _T( "You MUST specify an existing directory where to copy the NTSyslog binary file.\n\nDefault is C:\\WinNT\\System32."));
			GetDlgItem( IDC_COMPUTER)->SetFocus();
			return;
		}
		if (m_csFolder.GetAt( nSize-1) == '\\')
			// Remove the backslash at the end of the path
			m_csFolder.SetAt( nSize-1, 0);
	}
	CDialog::OnOK();
}

void CSetupServerDlg::SetAction( LPCTSTR lpstrFolder, BOOL bIsInstall)
{
	m_csFolder = lpstrFolder;
	m_bIsInstall = bIsInstall;
}

LPCTSTR CSetupServerDlg::GetFolder()
{
	return m_csFolder;
}
