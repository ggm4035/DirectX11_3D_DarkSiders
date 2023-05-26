#include "CCamera.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjectionMatrix, XMMatrixIdentity());
}

CCamera::CCamera(const CCamera& rhs)
	: CComponent(rhs)
	, m_bSwitch(rhs.m_bSwitch)
	, m_ViewMatrix(rhs.m_ViewMatrix)
	, m_ProjectionMatrix(rhs.m_ProjectionMatrix)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	if (FAILED(CComponent::Initialize_Prototype()))
		return E_FAIL;

	Set_Projection(XMConvertToRadians(60.f), 1280.f / 720.f, 1.f, 1000.f);

	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (FAILED(CComponent::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera::Set_View(_fmatrix _WorldMatrix)
{
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixInverse(nullptr, _WorldMatrix));
}

void CCamera::Set_Projection(const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	XMStoreFloat4x4(&m_ProjectionMatrix, XMMatrixPerspectiveFovLH(fFov, fAspect, fNear, fFar));
}

CCamera* CCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera* pInstance = new CCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CCamera::Clone(void* pArg)
{
	CCamera* pInstance = new CCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera::Free()
{
	CComponent::Free();
}
