#include "CCamera.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera& rhs)
	: CComponent(rhs)
	, m_ViewElements(rhs.m_ViewElements)
	, m_ProjElements(rhs.m_ProjElements)
	, m_bSwitch(rhs.m_bSwitch)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	if (FAILED(CComponent::Initialize_Prototype()))
		return E_FAIL;

	m_ViewElements.vEye = XMVectorSet(10.f, 10.f, -20.f, 1.f);
	m_ViewElements.vAt = XMVectorSet(1.f, 0.f, 1.f, 0.f);
	m_ViewElements.vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	m_ProjElements.fFov = XMConvertToRadians(60.f);
	m_ProjElements.fAspect = 1280.f / 720.f;
	m_ProjElements.fNear = 0.3f;
	m_ProjElements.fFar = 1000.f;

	m_ViewElements.LookAtLH();
	m_ProjElements.PerspectiveLH();

	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (FAILED(CComponent::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera::Tick(_double TimeDelta)
{
	if (false == m_bSwitch)
		return;

	m_ViewElements.LookAtLH();
	m_ProjElements.PerspectiveLH();
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
