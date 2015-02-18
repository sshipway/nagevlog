// NTSyslogCtrlDlg.h : header file
//

#if !defined(AFX_NTSYSLOGCTRLDLG_H__9FB33EE7_E0E8_11D5_B306_0040055338AF__INCLUDED_)
#define AFX_NTSYSLOGCTRLDLG_H__9FB33EE7_E0E8_11D5_B306_0040055338AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define COMPUTERS_SECTION	_T( "Computers")
#define LAST_COMPUTER_ENTRY	_T( "Last")

/////////////////////////////////////////////////////////////////////////////
// CNTSyslogCtrlDlg dialog

class CNTSyslogCtrlDlg : public CDialog
{
// Construction
public:
	CNTSyslogCtrlDlg(CWnd* pParent = NULL);	// standard constructor
	void OnAppAbout();
// Dialog Data
	//{{AFX_DATA(CNTSyslogCtrlDlg)
	enum { IDD = IDD_NTSYSLOGCTRL_DIALOG };
	CStatic	m_StatusIcon;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNTSyslogCtrlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetComputerName();

	//added to reduce redundant coding
	void SetMainDialogControls(int _i_);
	void SwapFilters(int a, int b);
	void exportreg(int flag, CString filename);
	void ReadMachineEventLogSettings(int good_query);
	CStringArray m_csaEventlogSelect;
	CUIntArray m_cuiaFilterMap;
	DWORD m_csaEventlogSelectSize;
	int	maxfilter;

	BOOL DisplayStatus( UINT nIconID, DWORD dwServiceState = 0);
	BOOL QueryServiceStatus();
	HICON m_hIcon;
	CString m_csComputer;
	void UpdateHBEntry();
	// Generated message map functions
	//{{AFX_MSG(CNTSyslogCtrlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelectComputer();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSyslogd();
	afx_msg void OnEventLog();
	afx_msg void OnStartService();
	afx_msg void OnStopService();
	afx_msg void OnCreate();
	afx_msg void OnDelete();
	afx_msg void OnClearall();
	afx_msg void OnMoveup();
	afx_msg void OnMovedown();
	afx_msg void OnGenevent();
	afx_msg void OnKillfocusDelay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnKillfocusHbsvc();
	afx_msg void OnEnKillfocusHbfreq();
	afx_msg void OnBnClickedExport();
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NTSYSLOGCTRLDLG_H__9FB33EE7_E0E8_11D5_B306_0040055338AF__INCLUDED_)
