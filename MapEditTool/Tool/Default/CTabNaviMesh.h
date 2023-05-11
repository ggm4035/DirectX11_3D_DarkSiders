#pragma once


// CTabNaviMesh 대화 상자

class CTabNaviMesh : public CDialogEx
{
	DECLARE_DYNAMIC(CTabNaviMesh)

public:
	CTabNaviMesh(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabNaviMesh();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TAB_NAVIMESH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
