// IpcScanDlg.h : header file
//

#pragma once



// CIpcScanDlg dialog
class CIpcScanDlg : public CDialogEx
{
// Construction
public:
	CIpcScanDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IPCSCAN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	int iState;
	
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_slider;
//	CEdit m_thread;
	CIPAddressCtrl m_addrStart;
	CIPAddressCtrl m_addrStop;
	CListBox m_process;
	CListCtrl m_result;
	CButton m_start;
	CButton m_stop;
	CButton m_save;
	CButton m_clear;
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	int m_thread;
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonSave();
};
