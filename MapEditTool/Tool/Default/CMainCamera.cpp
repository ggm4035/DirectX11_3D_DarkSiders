#include "pch.h"
#include "CMainCamera.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CDInput_Manager.h"

CMainCamera::CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CMainCamera::CMainCamera(const CMainCamera& rhs)
	: CCamera(rhs)
{
}

HRESULT CMainCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMainCamera::Initialize(void* pArg)
{
	if (FAILED(CCamera::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	TOOL->m_pCamera = this;

	return S_OK;
}

void CMainCamera::Tick(_double TimeDelta)
{
	CCamera::Tick(TimeDelta);
	KeyInput(TimeDelta);

	if (m_bFix)
	{
		Fix_Mouse();
		Mouse_Move(TimeDelta);
	}
}

void CMainCamera::Late_Tick(_double TimeDelta)
{
}

HRESULT CMainCamera::Render()
{
	return S_OK;
}

HRESULT CMainCamera::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);
	return S_OK;
}

void CMainCamera::KeyInput(const _double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Pressing(DIK_W))
		m_pTransformCom->Go_Straight(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_S))
		m_pTransformCom->Go_Backward(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_D))
		m_pTransformCom->Go_Right(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_A))
		m_pTransformCom->Go_Left(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_SPACE))
		m_pTransformCom->Go_Up(TimeDelta);

	if (pGameInstance->Key_Down(DIK_O))
		m_bFix = !m_bFix;

	Safe_Release(pGameInstance);
}

void CMainCamera::Mouse_Move(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_long dwMouseMove = 0;

	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CDInput_Manager::DIMS_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), (dwMouseMove) * TimeDelta / 10.f);
	}

	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CDInput_Manager::DIMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (dwMouseMove)*TimeDelta / 10.f);
	}

	Safe_Release(pGameInstance);
}

void CMainCamera::Fix_Mouse()
{
	POINT ptMouse{ 1280 >> 1, 720 >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

CMainCamera* CMainCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMainCamera* pInstance = new CMainCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		AfxMessageBox(L"Failed to Created CMainCamera");
	}
	return pInstance;
}

CMainCamera* CMainCamera::Clone(void* pArg)
{
	CMainCamera* pInstance = new CMainCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		AfxMessageBox(L"Failed to Cloned CMainCamera");
	}
	return pInstance;
}

void CMainCamera::Free()
{
	CCamera::Free();
}
