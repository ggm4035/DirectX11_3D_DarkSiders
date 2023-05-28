#include "CCamera.h"

#include "CPipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject3D(pDevice, pContext)
	, m_fFov(0.f), m_fAspect(0.f), m_fNear(0.f), m_fFar(0.f)
	, m_pPipeLine(CPipeLine::GetInstance())
{
	ZeroMemory(&m_vEye, sizeof m_vEye);
	ZeroMemory(&m_vAt, sizeof m_vAt);
	ZeroMemory(&m_vUp, sizeof m_vUp);
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject3D(rhs)
	, m_vEye(rhs.m_vEye), m_vAt(rhs.m_vAt), m_vUp(rhs.m_vUp)
	, m_fFov(rhs.m_fFov), m_fAspect(rhs.m_fAspect)
	, m_fNear(rhs.m_fNear), m_fFar(rhs.m_fFar)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		CAMERADESC CameraDesc = { };
		CameraDesc = *static_cast<CAMERADESC*>(pArg);

		if (FAILED(CGameObject3D::Initialize(&CameraDesc.TransformDesc)))
			return E_FAIL;

		m_vEye = CameraDesc.vEye;
		m_vAt = CameraDesc.vAt;
		m_vUp = CameraDesc.vUp;

		m_fFov = CameraDesc.fFov;
		m_fAspect = CameraDesc.fAspect;
		m_fNear = CameraDesc.fNear;
		m_fFar = CameraDesc.fFar;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&m_vAt));

	return S_OK;
}

void CCamera::Tick(_double TimeDelta)
{
	//if (false == m_bSwitch)
	//	return;						// 어캐할 지 생각해야할 듯 매니저로 관리해야되나

	m_pPipeLine->Set_Transform(CPipeLine::STATE_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	m_pPipeLine->Set_Transform(CPipeLine::STATE_PROJ, XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Late_Tick(_double TimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Free()
{
	Safe_Release(m_pPipeLine);

	CGameObject3D::Free();
}
