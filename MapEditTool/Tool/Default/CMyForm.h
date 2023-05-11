#pragma once

#include "CTabTerrain.h"
#include "CTabMesh.h"
#include "CTabNaviMesh.h"
#include "CTabCamera.h"
#include "CTabCollider.h"

// CMyForm 폼 보기

class CMyForm : public CFormView
{
	DECLARE_DYNCREATE(CMyForm)

protected:
	CMyForm();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMyForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CMyForm };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnInitialUpdate();

public:
	CTabCtrl m_Tab;
	CTabTerrain* m_dlgTerrain;
	CTabMesh* m_dlgMesh;
	CTabNaviMesh* m_dlgNaviMesh;
	CTabCamera* m_dlgCamera;
	CTabCollider* m_dlgCollider;
};


