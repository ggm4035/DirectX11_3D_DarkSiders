#pragma once


// CTabTerrain 대화 상자

class CTabTerrain : public CDialogEx
{
	DECLARE_DYNAMIC(CTabTerrain)

public:
	CTabTerrain(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabTerrain();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TAB_TERRAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);

public:
	CEdit m_edXCount[10];
	CSpinButtonCtrl m_spXCount[10];
};
