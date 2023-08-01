#include "CCamera_Free.h"

#include "CGameInstance.h"
#include "CGameManager.h"
#include "CPlayer.h"
#include "CCursor.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CCamera::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	m_vOffsets = _float3(0.f, 15.f, -10.f);

	return S_OK;
}

void CCamera_Free::Tick(const _double& TimeDelta)
{
	Key_Input(TimeDelta);

	switch (m_eCamState)
	{
	case Client::CCamera_Free::CAM_DEFAULT:
		Default(TimeDelta);
		break;

	case Client::CCamera_Free::CAM_SUBBOSS:
		SubBoss(TimeDelta);
		break;

	case Client::CCamera_Free::CAM_FINALBOSS:
		FinalBoss(TimeDelta);
		break;
	}

	CCamera::Tick(TimeDelta);
}

void CCamera_Free::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::FinalBoss(const _double& TimeDelta)
{
	static _float fLimit = { 0.f };

	fLimit += TimeDelta;

	fLimit = 3.f < fLimit ? 3.f : fLimit;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pPlayerTransform = pGameInstance->Get_Player()->Get_Transform();

	_matrix ParentMatrix = pPlayerTransform->Get_WorldMatrix();

	ParentMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	ParentMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	ParentMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	_matrix WorldMatrix = XMMatrixTranslation(m_vOffsets.x, m_vOffsets.y + fLimit * 0.3f, m_vOffsets.z - fLimit) * XMMatrixRotationY(m_fRadian) * ParentMatrix;

	m_pTransformCom->Chase_Lerp(WorldMatrix.r[3], TimeDelta, 0.1f);

	XMStoreFloat4(&m_vAt, pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->LookAt(XMLoadFloat4(&m_vAt));

	Safe_Release(pGameInstance);
}

void CCamera_Free::SubBoss(const _double& TimeDelta)
{
	static _float fLimit = { 0.f };

	fLimit += TimeDelta;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pGameObject = { nullptr };

	pGameObject = pGameInstance->Find_GameObject(LEVEL_GAMEPLAY, L"Layer_Monster", L"SteamRoller");
	if (nullptr == pGameObject)
		return;

	CTransform* pTransform = dynamic_cast<CGameObject3D*>(pGameObject)->Get_Transform();

	_matrix ParentMatrix = pTransform->Get_WorldMatrix();

	ParentMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	ParentMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	ParentMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	_matrix WorldMatrix = XMMatrixTranslation(0.f, 15.f, -10.f) * XMMatrixRotationY(m_fRadian) * ParentMatrix;

	m_pTransformCom->Chase_Lerp(WorldMatrix.r[3], TimeDelta, 0.1f);

	XMStoreFloat4(&m_vAt, pTransform->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->LookAt(XMLoadFloat4(&m_vAt));

	Safe_Release(pGameInstance);

	if (5.f < fLimit)
	{
		m_eCamState = CAM_DEFAULT;
	}
}

void CCamera_Free::Default(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pPlayerTransform = pGameInstance->Get_Player()->Get_Transform();

	_matrix ParentMatrix = pPlayerTransform->Get_WorldMatrix();

	ParentMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	ParentMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	ParentMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	_matrix WorldMatrix = XMMatrixTranslation(m_vOffsets.x, m_vOffsets.y, m_vOffsets.z) * XMMatrixRotationY(m_fRadian) * ParentMatrix;

	m_pTransformCom->Chase_Lerp(WorldMatrix.r[3], TimeDelta, 0.1f);

	XMStoreFloat4(&m_vAt, pPlayerTransform->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->LookAt(XMLoadFloat4(&m_vAt));

	Safe_Release(pGameInstance);
}

HRESULT CCamera_Free::Add_Components()
{
	return S_OK;
}

void CCamera_Free::Key_Input(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCursor* pCursor = CGameManager::GetInstance()->Get_Cursor();

	if (pGameInstance->Mouse_Pressing(CInput_Device::DIM_RB))
	{
		pCursor->Off();
		_long dwMouseMove = 0;

		SetCursorPos(900, 600);

		if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
		{
			m_fRadian += dwMouseMove * TimeDelta / 10.f;
		}
	}
	else
		pCursor->On();

	Safe_Release(pGameInstance);
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCamera_Free");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCamera_Free* CCamera_Free::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CCamera_Free");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Free::Free()
{
	CCamera::Free();
}
