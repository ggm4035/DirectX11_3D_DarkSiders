﻿#pragma once


// CTabAnimation 대화 상자

class CTabAnimation : public CDialogEx
{
	DECLARE_DYNAMIC(CTabAnimation)

public:
	CTabAnimation(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabAnimation();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TAB_ANIMATION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
