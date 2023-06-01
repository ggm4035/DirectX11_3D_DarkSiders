#include "stdafx.h"
#include "CCamera_Free.h"

#include "CGameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& rhs)
	:CCamera(rhs)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(CComponent* pOwner, void* pArg)
{
	CCamera::CAMERADESC CameraDesc;
	CameraDesc.vEye = _float4(0.f, 20.f, -10.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

	CameraDesc.fFov = XMConvertToRadians(60.f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.3f;
	CameraDesc.fFar = 300.f;

	CameraDesc.TransformDesc.SpeedPerSec = 10.f;
	CameraDesc.TransformDesc.RotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(CCamera::Initialize(pOwner, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Tick(const _double& TimeDelta)
{
	Key_Input(TimeDelta);

	CCamera::Tick(TimeDelta);
}

void CCamera_Free::Late_Tick(const _double& TimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

HRESULT CCamera_Free::Add_Components()
{
	return S_OK;
}

void CCamera_Free::Key_Input(const _double& TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Pressing(DIK_W))
		m_pTransformCom->Go_Straight(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_S))
		m_pTransformCom->Go_Backward(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_A))
		m_pTransformCom->Go_Left(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_D))
		m_pTransformCom->Go_Right(TimeDelta);

	if (pGameInstance->Key_Pressing(DIK_SPACE))
		m_pTransformCom->Go_Up(TimeDelta);

	if (pGameInstance->Mouse_Pressing(CInput_Device::DIM_RB))
	{
		_long dwMouseMove = 0;

		if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), (dwMouseMove)*TimeDelta / 10.f);
		}

		if (dwMouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (dwMouseMove)*TimeDelta / 10.f);
		}
	}

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

CGameObject3D* CCamera_Free::Clone(CComponent* pOwner, void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
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
