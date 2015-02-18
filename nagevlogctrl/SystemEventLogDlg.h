//-----------------------------------------------------------------------------
//
// SystemeventLogDlg.h : header file
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
//	  25-Nov-2001  DL  Module completed
//
//----------------------------------------------------------------------------

#if !defined(AFX_SYSTEMEVENTLOGDLG_H__98503D18_9608_11D5_B2A2_0040055338AF__INCLUDED_)
#define AFX_SYSTEMEVENTLOGDLG_H__98503D18_9608_11D5_B2A2_0040055338AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSystemEventLogDlg dialog

class CSystemEventLogDlg : public CDialog
{
// Construction
public:
	CSystemEventLogDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSystemEventLogDlg)
	enum { IDD = IDD_SYSTEM_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void SetComputer( LPCTSTR lpstrComputer = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemEventLogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_csComputer;
	UINT m_uCurrentState;
	BOOL IsModified();

	// Generated message map functions
	//{{AFX_MSG(CSystemEventLogDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEMEVENTLOGDLG_H__98503D18_9608_11D5_B2A2_0040055338AF__INCLUDED_)
