
// ToolView.cpp: CToolView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "ToolDoc.h"
#include "ToolView.h"
#include "MainFrm.h"

#include "CGameInstance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CToolView 생성/소멸

HWND g_hWnd;

CToolView::CToolView() noexcept
	:m_pGameInstance{ CGameInstance::GetInstance() }
{
	// TODO: 여기에 생성 코드를 추가합니다.
	Safe_AddRef(m_pGameInstance);
}

CToolView::~CToolView()
{
	Safe_Release(m_pGameInstance);
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

#pragma region 불필요
// CToolView 인쇄

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CToolView 진단

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG

#pragma endregion

// CToolView 메시지 처리기

void CToolView::OnInitialUpdate()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CView::OnInitialUpdate();

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	RECT rcWnd{};

	pMainFrame->GetWindowRect(&rcWnd);

	SetRect(&rcWnd, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top);

	RECT rcMainView{};

	GetClientRect(&rcMainView);

	_float fRowFrame = _float(rcWnd.right - rcMainView.right);
	_float fColFrame = _float(rcWnd.bottom - rcMainView.bottom);

	pMainFrame->SetWindowPos(nullptr, 0, 0, _int(g_iWinSizeX + fRowFrame), _int(g_iWinSizeY + fColFrame), SWP_NOZORDER);

	g_hWnd = m_hWnd;
	GRAPHICDESC graphicDesc{};
	graphicDesc.hWnd = g_hWnd;
	graphicDesc.eWinMode = GRAPHICDESC::WM_WIN;
	graphicDesc.iViewportSizeX = g_iWinSizeX;
	graphicDesc.iViewportSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(graphicDesc, &m_pDevice, &m_pContext)))
	{
		AfxMessageBox(L"Device Create Failed");
		return;
	}
}

// CToolView 그리기
void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Present();
}

void CToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	CView::OnLButtonDown(nFlags, point);
}


void CToolView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}