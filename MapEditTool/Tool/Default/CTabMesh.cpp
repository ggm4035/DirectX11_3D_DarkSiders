// CTabMesh.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CTabMesh.h"
#include "afxdialogex.h"


// CTabMesh 대화 상자

IMPLEMENT_DYNAMIC(CTabMesh, CDialogEx)

CTabMesh::CTabMesh(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TAB_MESH, pParent)
{

}

CTabMesh::~CTabMesh()
{
}

void CTabMesh::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT21, m_edXCount[0]);
	DDX_Control(pDX, IDC_EDIT15, m_edXCount[1]);
	DDX_Control(pDX, IDC_EDIT18, m_edXCount[2]);
	DDX_Control(pDX, IDC_EDIT13, m_edXCount[3]);
	DDX_Control(pDX, IDC_EDIT16, m_edXCount[4]);
	DDX_Control(pDX, IDC_EDIT19, m_edXCount[5]);
	DDX_Control(pDX, IDC_EDIT14, m_edXCount[6]);
	DDX_Control(pDX, IDC_EDIT17, m_edXCount[7]);
	DDX_Control(pDX, IDC_EDIT20, m_edXCount[8]);

	DDX_Control(pDX, IDC_SPIN1, m_spXCount[0]);
	DDX_Control(pDX, IDC_SPIN4, m_spXCount[1]);
	DDX_Control(pDX, IDC_SPIN7, m_spXCount[2]);
	DDX_Control(pDX, IDC_SPIN2, m_spXCount[3]);
	DDX_Control(pDX, IDC_SPIN5, m_spXCount[4]);
	DDX_Control(pDX, IDC_SPIN11, m_spXCount[5]);
	DDX_Control(pDX, IDC_SPIN3, m_spXCount[6]);
	DDX_Control(pDX, IDC_SPIN6, m_spXCount[7]);
	DDX_Control(pDX, IDC_SPIN12, m_spXCount[8]);
}


BEGIN_MESSAGE_MAP(CTabMesh, CDialogEx)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CTabMesh::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN4, &CTabMesh::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN7, &CTabMesh::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, &CTabMesh::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN5, &CTabMesh::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN11, &CTabMesh::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN3, &CTabMesh::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN6, &CTabMesh::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN12, &CTabMesh::OnDeltaposSpin1)
END_MESSAGE_MAP()


// CTabMesh 메시지 처리기

BOOL CTabMesh::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	for (int i = 0; i < 9; ++i)
	{
		m_edXCount[i].SetWindowTextW(L"0");
		m_spXCount[i].SetRange(0, 1000);
		m_spXCount[i].SetPos(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTabMesh::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int iIndex = 0;

	switch (pNMHDR->idFrom)
	{
	case IDC_SPIN1:
		iIndex = 0;
		break;

	case IDC_SPIN4:
		iIndex = 1;
		break;

	case IDC_SPIN7:
		iIndex = 2;
		break;

	case IDC_SPIN2:
		iIndex = 3;
		break;

	case IDC_SPIN5:
		iIndex = 4;
		break;

	case IDC_SPIN11:
		iIndex = 5;
		break;

	case IDC_SPIN3:
		iIndex = 6;
		break;

	case IDC_SPIN6:
		iIndex = 7;
		break;

	case IDC_SPIN12:
		iIndex = 8;
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
