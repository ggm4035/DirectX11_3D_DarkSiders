// CTabTerrain.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CTabTerrain.h"
#include "afxdialogex.h"


// CTabTerrain 대화 상자

IMPLEMENT_DYNAMIC(CTabTerrain, CDialogEx)

CTabTerrain::CTabTerrain(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TAB_TERRAIN, pParent)
{

}

CTabTerrain::~CTabTerrain()
{
}

void CTabTerrain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edXCount[0]);
	DDX_Control(pDX, IDC_EDIT5, m_edXCount[1]);
	DDX_Control(pDX, IDC_EDIT6, m_edXCount[2]);
	DDX_Control(pDX, IDC_EDIT8, m_edXCount[3]);
	DDX_Control(pDX, IDC_EDIT9, m_edXCount[4]);
	DDX_Control(pDX, IDC_EDIT10, m_edXCount[5]);
	DDX_Control(pDX, IDC_EDIT11, m_edXCount[6]);
	DDX_Control(pDX, IDC_EDIT12, m_edXCount[7]);
	DDX_Control(pDX, IDC_EDIT22, m_edXCount[8]);
	DDX_Control(pDX, IDC_EDIT23, m_edXCount[9]);

	DDX_Control(pDX, IDC_SPIN1, m_spXCount[0]);
	DDX_Control(pDX, IDC_SPIN2, m_spXCount[1]);
	DDX_Control(pDX, IDC_SPIN4, m_spXCount[2]);
	DDX_Control(pDX, IDC_SPIN5, m_spXCount[3]);
	DDX_Control(pDX, IDC_SPIN7, m_spXCount[4]);
	DDX_Control(pDX, IDC_SPIN8, m_spXCount[5]);
	DDX_Control(pDX, IDC_SPIN13, m_spXCount[6]);
	DDX_Control(pDX, IDC_SPIN14, m_spXCount[7]);
	DDX_Control(pDX, IDC_SPIN15, m_spXCount[8]);
	DDX_Control(pDX, IDC_SPIN16, m_spXCount[9]);
}


BEGIN_MESSAGE_MAP(CTabTerrain, CDialogEx)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CTabTerrain::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, &CTabTerrain::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN4, &CTabTerrain::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN5, &CTabTerrain::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN7, &CTabTerrain::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN8, &CTabTerrain::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN13, &CTabTerrain::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN14, &CTabTerrain::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN15, &CTabTerrain::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN16, &CTabTerrain::OnDeltaposSpin1)
END_MESSAGE_MAP()

// CTabTerrain 메시지 처리기

BOOL CTabTerrain::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	for (int i = 0; i < 10; ++i)
	{
		m_edXCount[i].SetWindowTextW(L"0");
		m_spXCount[i].SetRange(0, 1000);
		m_spXCount[i].SetPos(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTabTerrain::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int iIndex = 0;

	switch (pNMHDR->idFrom)
	{
	case IDC_SPIN1:
		iIndex = 0;
		break;

	case IDC_SPIN2:
		iIndex = 1;
		break;

	case IDC_SPIN4:
		iIndex = 2;
		break;

	case IDC_SPIN5:
		iIndex = 3;
		break;

	case IDC_SPIN7:
		iIndex = 4;
		break;

	case IDC_SPIN8:
		iIndex = 5;
		break;

	case IDC_SPIN13:
		iIndex = 6;
		break;

	case IDC_SPIN14:
		iIndex = 7;
		break;

	case IDC_SPIN15:
		iIndex = 8;
		break;

	case IDC_SPIN16:
		iIndex = 9;
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
