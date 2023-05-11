// CMyForm.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CMyForm.h"

// CMyForm

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(IDD_CMyForm)
{
}

CMyForm::~CMyForm()
{
	delete m_Tab;
	delete m_dlgTerrain;
	delete m_dlgNaviMesh;
	delete m_dlgMesh;
	delete m_dlgCamera;
	delete m_dlgCollider;
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_Tab);
}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CMyForm::OnSelchangeTabMain)
END_MESSAGE_MAP()


// CMyForm 진단

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyForm 메시지 처리기

void CMyForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CRect rect{};
	m_Tab.GetWindowRect(rect);

	m_Tab.InsertItem(0, L"Terrain");
	m_Tab.InsertItem(1, L"Mesh");
	m_Tab.InsertItem(2, L"NaviMesh");
	m_Tab.InsertItem(3, L"Camera");
	m_Tab.InsertItem(4, L"Collider");

	m_Tab.SetCurSel(0);

	m_dlgTerrain = new CTabTerrain;
	m_dlgTerrain->Create(IDD_DIALOG_TAB_TERRAIN, &m_Tab);
	m_dlgTerrain->MoveWindow(0, 20, rect.Width(), rect.Height());
	m_dlgTerrain->ShowWindow(SW_SHOW);

	m_dlgMesh = new CTabMesh;
	m_dlgMesh->Create(IDD_DIALOG_TAB_MESH, &m_Tab);
	m_dlgMesh->MoveWindow(0, 20, rect.Width(), rect.Height());
	m_dlgMesh->ShowWindow(SW_HIDE);

	m_dlgNaviMesh = new CTabNaviMesh;
	m_dlgNaviMesh->Create(IDD_DIALOG_TAB_NAVIMESH, &m_Tab);
	m_dlgNaviMesh->MoveWindow(0, 20, rect.Width(), rect.Height());
	m_dlgNaviMesh->ShowWindow(SW_HIDE);

	m_dlgCamera = new CTabCamera;
	m_dlgCamera->Create(IDD_DIALOG_TAB_CAMERA, &m_Tab);
	m_dlgCamera->MoveWindow(0, 20, rect.Width(), rect.Height());
	m_dlgCamera->ShowWindow(SW_HIDE);

	m_dlgCollider = new CTabCollider;
	m_dlgCollider->Create(IDD_DIALOG_TAB_COLLIDER, &m_Tab);
	m_dlgCollider->MoveWindow(0, 20, rect.Width(), rect.Height());
	m_dlgCollider->ShowWindow(SW_HIDE);
}


void CMyForm::OnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (IDC_TAB_MAIN == pNMHDR->idFrom)
	{
		int iSelect = m_Tab.GetCurSel();

		switch (iSelect)
		{
		case 0:
			m_dlgTerrain->ShowWindow(SW_SHOW);
			m_dlgMesh->ShowWindow(SW_HIDE);
			m_dlgNaviMesh->ShowWindow(SW_HIDE);
			m_dlgCamera->ShowWindow(SW_HIDE);
			m_dlgCollider->ShowWindow(SW_HIDE);
			break;

		case 1:
			m_dlgTerrain->ShowWindow(SW_HIDE);
			m_dlgMesh->ShowWindow(SW_SHOW);
			m_dlgNaviMesh->ShowWindow(SW_HIDE);
			m_dlgCamera->ShowWindow(SW_HIDE);
			m_dlgCollider->ShowWindow(SW_HIDE);
			break;

		case 2:
			m_dlgTerrain->ShowWindow(SW_HIDE);
			m_dlgMesh->ShowWindow(SW_HIDE);
			m_dlgNaviMesh->ShowWindow(SW_SHOW);
			m_dlgCamera->ShowWindow(SW_HIDE);
			m_dlgCollider->ShowWindow(SW_HIDE);
			break;

		case 3:
			m_dlgTerrain->ShowWindow(SW_HIDE);
			m_dlgMesh->ShowWindow(SW_HIDE);
			m_dlgNaviMesh->ShowWindow(SW_HIDE);
			m_dlgCamera->ShowWindow(SW_SHOW);
			m_dlgCollider->ShowWindow(SW_HIDE);
			break;

		case 4:
			m_dlgTerrain->ShowWindow(SW_HIDE);
			m_dlgMesh->ShowWindow(SW_HIDE);
			m_dlgNaviMesh->ShowWindow(SW_HIDE);
			m_dlgCamera->ShowWindow(SW_HIDE);
			m_dlgCollider->ShowWindow(SW_SHOW);
			break;
		}
	}
}
