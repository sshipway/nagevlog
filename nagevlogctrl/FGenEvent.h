#if !defined(AFX_FGENEVENT_H__F9DA8F71_7CB9_4BBA_8A3A_FFDCBC182CFB__INCLUDED_)
#define AFX_FGENEVENT_H__F9DA8F71_7CB9_4BBA_8A3A_FFDCBC182CFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FGenEvent.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FGenEvent dialog

class FGenEvent : public CDialog
{
// Construction
public:
	FGenEvent(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(FGenEvent)
	enum { IDD = IDD_FGENEVENT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FGenEvent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FGenEvent)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FGENEVENT_H__F9DA8F71_7CB9_4BBA_8A3A_FFDCBC182CFB__INCLUDED_)
