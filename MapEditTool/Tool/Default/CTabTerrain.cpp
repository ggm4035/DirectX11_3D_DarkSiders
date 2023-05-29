// CTabTerrain.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CTabTerrain.h"
#include "afxdialogex.h"

#include "CToolInstance.h"

#include "CTerrain.h"
#include "CCoordnate_Axis.h"

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
	DDX_Control(pDX, IDC_EDIT1, m_edit[0]);
	DDX_Control(pDX, IDC_EDIT5, m_edit[1]);
	DDX_Control(pDX, IDC_EDIT6, m_edit[2]);
	DDX_Control(pDX, IDC_EDIT8, m_edit[3]);
	DDX_Control(pDX, IDC_EDIT9, m_edit[4]);
	DDX_Control(pDX, IDC_EDIT10, m_edit[5]);
	DDX_Control(pDX, IDC_EDIT11, m_edit[6]);
	DDX_Control(pDX, IDC_EDIT12, m_edit[7]);
	DDX_Control(pDX, IDC_EDIT22, m_edit[8]);
	DDX_Control(pDX, IDC_EDIT23, m_edit[9]);

	DDX_Control(pDX, IDC_SPIN1, m_spEdit[0]);
	DDX_Control(pDX, IDC_SPIN2, m_spEdit[1]);
	DDX_Control(pDX, IDC_SPIN4, m_spEdit[2]);
	DDX_Control(pDX, IDC_SPIN5, m_spEdit[3]);
	DDX_Control(pDX, IDC_SPIN7, m_spEdit[4]);
	DDX_Control(pDX, IDC_SPIN8, m_spEdit[5]);
	DDX_Control(pDX, IDC_SPIN13, m_spEdit[6]);
	DDX_Control(pDX, IDC_SPIN14, m_spEdit[7]);
	DDX_Control(pDX, IDC_SPIN15, m_spEdit[8]);
	DDX_Control(pDX, IDC_SPIN16, m_spEdit[9]);
	DDX_Control(pDX, IDC_RADIO1, m_RenderStyle[0]);
	DDX_Control(pDX, IDC_RADIO2, m_RenderStyle[1]);
	DDX_Control(pDX, IDC_RADIO3, m_RenderCoordnate_Axis[0]);
	DDX_Control(pDX, IDC_RADIO4, m_RenderCoordnate_Axis[1]);
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
	ON_BN_CLICKED(IDC_BUTTON1, &CTabTerrain::OnBnClickedApply)
	ON_BN_CLICKED(IDC_RADIO1, &CTabTerrain::OnBnClickedRenderStyle)
	ON_BN_CLICKED(IDC_RADIO2, &CTabTerrain::OnBnClickedRenderStyle)
	ON_BN_CLICKED(IDC_RADIO3, &CTabTerrain::OnBnClickedRenderCoordnate_Axis)
	ON_BN_CLICKED(IDC_RADIO4, &CTabTerrain::OnBnClickedRenderCoordnate_Axis)
	ON_EN_CHANGE(IDC_EDIT9, &CTabTerrain::OnEnChangePosition)
	ON_EN_CHANGE(IDC_EDIT10, &CTabTerrain::OnEnChangePosition)
	ON_EN_CHANGE(IDC_EDIT11, &CTabTerrain::OnEnChangePosition)
	ON_EN_CHANGE(IDC_EDIT12, &CTabTerrain::OnEnChangeRotation)
	ON_EN_CHANGE(IDC_EDIT22, &CTabTerrain::OnEnChangeRotation)
	ON_EN_CHANGE(IDC_EDIT23, &CTabTerrain::OnEnChangeRotation)
END_MESSAGE_MAP()

// CTabTerrain 메시지 처리기

BOOL CTabTerrain::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	for (int i = 0; i < 10; ++i)
	{
		m_edit[i].SetWindowTextW(L"0");
		m_spEdit[i].SetRange(0, 1000);
		m_spEdit[i].SetPos(0);
	}
	m_RenderStyle[1].SetCheck(TRUE);
	m_RenderCoordnate_Axis[0].SetCheck(TRUE);

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

	int iVal = pNMUpDown->iPos + pNMUpDown->iDelta;

	if ((4 > iIndex) && (0 <= iVal) && (1000 >= iVal))
	{
		CString sValue;
		sValue.Format(L"%d\n", iVal);
		m_edit[iIndex].SetWindowTextW(sValue);
	}
	if (4 <= iIndex)
	{
		CString sValue;
		sValue.Format(L"%d\n", iVal);
		m_edit[iIndex].SetWindowTextW(sValue);
	}

	*pResult = 0;
}


void CTabTerrain::OnBnClickedApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strXCount = L"";
	CString strZCount = L"";
	CString strInterval = L"";
	CString strDetail = L"";

	m_edit[0].GetWindowTextW(strXCount); // XCount
	m_edit[2].GetWindowTextW(strZCount); // ZCount
	m_edit[1].GetWindowTextW(strInterval); // Interval
	m_edit[3].GetWindowTextW(strDetail); // Detail

	_uint iXCount = _ttoi(strXCount);
	_uint iZCount = _ttoi(strZCount);
	_float fInterval = _ttof(strInterval);
	_float fDetail = _ttof(strDetail);

	if(TOOL->m_pTerrain)
		TOOL->m_pTerrain->Reset_Data(iXCount, iZCount, fInterval, fDetail);
}

// RenderStyle, RenderCoordnate_Axis
void CTabTerrain::OnBnClickedRenderStyle()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	D3D11_RASTERIZER_DESC RasterizerDesc;
	ZeroMemory(&RasterizerDesc, sizeof RasterizerDesc);

	RasterizerDesc.CullMode = { D3D11_CULL_BACK };
	RasterizerDesc.FrontCounterClockwise = { false };

	if (m_RenderStyle[0].GetCheck())
		RasterizerDesc.FillMode = { D3D11_FILL_SOLID };

	if (m_RenderStyle[1].GetCheck())
		RasterizerDesc.FillMode = { D3D11_FILL_WIREFRAME };

	TOOL->m_pTerrain->Reset_Rasterizer(RasterizerDesc);
}

void CTabTerrain::OnBnClickedRenderCoordnate_Axis()
{
	if (m_RenderCoordnate_Axis[0].GetCheck())
		TOOL->m_pCoordnate_Axis->RenderOn();

	if (m_RenderCoordnate_Axis[1].GetCheck())
		TOOL->m_pCoordnate_Axis->RenderOff();
}


void CTabTerrain::OnEnChangePosition()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strPosX = L"";
	CString strPosY = L"";
	CString strPosZ = L"";

	m_edit[4].GetWindowTextW(strPosX);
	m_edit[5].GetWindowTextW(strPosY);
	m_edit[6].GetWindowTextW(strPosZ);

	_float4 vPosition = _float4(_ttof(strPosX), _ttof(strPosY), _ttof(strPosZ), 1.f);

	if (TOOL->m_pTerrain)
		TOOL->m_pTerrain->Set_Position(vPosition);
}

void CTabTerrain::OnEnChangeRotation()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strRotX = L"";
	CString strRotY = L"";
	CString strRotZ = L"";

	m_edit[7].GetWindowTextW(strRotX);
	m_edit[8].GetWindowTextW(strRotY);
	m_edit[9].GetWindowTextW(strRotZ);

	_float3 vDegrees = _float3(_ttof(strRotX), _ttof(strRotY), _ttof(strRotZ));

	if (TOOL->m_pTerrain)
		TOOL->m_pTerrain->Set_Rotation(vDegrees);
}