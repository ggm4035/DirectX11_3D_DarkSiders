#include "pch.h"
#include "CMainCamera.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CDInput_Manager.h"

CMainCamera::CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMainCamera::CMainCamera(const CMainCamera& rhs)
	:CGameObject(rhs)
{
}

HRESULT CMainCamera::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainCamera::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 10.f, 0.f, 1.f));
	m_pTransform->LookAt(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	TOOL->m_pCamera = this;

	return S_OK;
}

void CMainCamera::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
	KeyInput(TimeDelta);

	if (m_bFix)
	{
		Fix_Mouse();
		Mouse_Move(TimeDelta);
	}

	m_pCameraCom->Set_View(XMLoadFloat4x4(&m_pTransform->Get_WorldMatrix()));
	m_pCameraCom->Set_Projection(XMConvertToRadians(60.f), 1280.f / 720.f, 0.3f, 1000.f);
}

void CMainCamera::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
}

HRESULT CMainCamera::Render()
{
	if (FAILED(CGameObject::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainCamera::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_Camera",
		L"Com_Camera", (CComponent**)&m_pCameraCom)))
		return E_FAIL;
	pGameInstance->Add_Camera(LEVEL_TOOLMAP, L"Camera_Main", m_pCameraCom);
	pGameInstance->On_Camera(LEVEL_TOOLMAP, L"Camera_Main");

	if (FAILED(Add_Component(LEVEL_TOOLMAP, L"Prototype_Component_Transform",
		L"Com_Transform", (CComponent**)&m_pTransform, &CTransform::TRASNFORMDESC(10.f, XMConvertToRadians(90.f)))))
		return E_FAIL;

	Safe_Release(pGameInstance);
	return S_OK;
}

void CMainCamera::KeyInput(const _double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Pressing(DIK_W))
		m_pTransform->Go_Straight(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_S))
		m_pTransform->Go_Backword(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_D))
		m_pTransform->Go_Right(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_A))
		m_pTransform->Go_Left(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_SPACE))
		m_pTransform->Go_Up(TimeDelta);

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
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), (dwMouseMove) * TimeDelta / 10.f);
	}

	if (dwMouseMove = pGameInstance->Get_DIMouseMove(CDInput_Manager::DIMS_X))
	{
		m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (dwMouseMove)*TimeDelta / 10.f);
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

CGameObject* CMainCamera::Clone(void* pArg)
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
	Safe_Release(m_pTransform);
	Safe_Release(m_pCameraCom);
	CGameObject::Free();
}
