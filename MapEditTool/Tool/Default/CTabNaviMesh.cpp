// CTabNaviMesh.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CTabNaviMesh.h"
#include "afxdialogex.h"


// CTabNaviMesh 대화 상자

IMPLEMENT_DYNAMIC(CTabNaviMesh, CDialogEx)

CTabNaviMesh::CTabNaviMesh(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TAB_NAVIMESH, pParent)
{

}

CTabNaviMesh::~CTabNaviMesh()
{
}

void CTabNaviMesh::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT30, m_edXCount[0]);
	DDX_Control(pDX, IDC_EDIT31, m_edXCount[1]);
	DDX_Control(pDX, IDC_EDIT32, m_edXCount[2]);

	DDX_Control(pDX, IDC_SPIN5, m_spXCount[0]);
	DDX_Control(pDX, IDC_SPIN7, m_spXCount[1]);
	DDX_Control(pDX, IDC_SPIN9, m_spXCount[2]);
}


BEGIN_MESSAGE_MAP(CTabNaviMesh, CDialogEx)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN5, &CTabNaviMesh::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN7, &CTabNaviMesh::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN9, &CTabNaviMesh::OnDeltaposSpin1)
END_MESSAGE_MAP()


// CTabNaviMesh 메시지 처리기
BOOL CTabNaviMesh::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	for (int i = 0; i < 3; ++i)
	{
		m_edXCount[i].SetWindowTextW(L"0");
		m_spXCount[i].SetRange(0, 1000);
		m_spXCount[i].SetPos(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTabNaviMesh::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int iIndex = 0;

	switch (pNMHDR->idFrom)
	{
	case IDC_SPIN5:
		iIndex = 0;
		break;

	case IDC_SPIN7:
		iIndex = 1;
		break;

	case IDC_SPIN9:
		iIndex = 2;
		break;

	default:
		return;
	}

	float fVal = float(pNMUpDown->iPos + pNMUpDown->iDelta);

	if ((0.f <= fVal) && (1000.f >= fVal))
	{
		CString sValue;
		sValue.Format(L"%f\n", fVal);
		m_edXCount[iIndex].SetWindowTextW(sValue);
	}

	*pResult = 0;
}
