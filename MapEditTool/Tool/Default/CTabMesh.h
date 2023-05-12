#pragma once


// CTabMesh 대화 상자

class CTabMesh : public CDialogEx
{
	DECLARE_DYNAMIC(CTabMesh)

public:
	CTabMesh(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabMesh();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TAB_MESH };
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
