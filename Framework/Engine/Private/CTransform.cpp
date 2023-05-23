#include "CTransform.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
	ZeroMemory(&m_vRight, sizeof m_vRight);
	ZeroMemory(&m_vUp, sizeof m_vUp);
	ZeroMemory(&m_vLook, sizeof m_vLook);
	ZeroMemory(&m_vPosition, sizeof m_vPosition);
	ZeroMemory(&m_vScale, sizeof m_vScale);
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_vRight(rhs.m_vRight)
	, m_vUp(rhs.m_vUp)
	, m_vLook(rhs.m_vLook)
	, m_vPosition(rhs.m_vPosition)
	, m_vScale(rhs.m_vScale)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_eMoveType(rhs.m_eMoveType)
{
}

HRESULT CTransform::Initialize_Prototype()
{
	if (FAILED(CComponent::Initialize_Prototype()))
		return E_FAIL;

	m_vRight =		{ 1.f, 0.f, 0.f };
	m_vUp =			{ 0.f, 1.f, 0.f };
	m_vLook =		{ 0.f, 0.f, 1.f };
	m_vPosition =	{ 0.f, 0.f, 0.f };

	m_vScale =		{ 1.f, 1.f, 1.f };

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (FAILED(CComponent::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTransform::Tick(_double TimeDelta)
{
	_matrix ScaleMatrix, RotationMatrix, TransMatrix;
	ScaleMatrix = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);

	_float3 vRight, vUp, vLook;
	_float4x4 matRotation;
	XMStoreFloat4x4(&matRotation, XMMatrixIdentity());

	vRight = { 1.f, 0.f, 0.f };
	vUp = { 0.f, 1.f, 0.f };
	vLook = { 0.f, 0.f, 1.f };
	
	matRotation._11 = vRight.x; matRotation._12 = vRight.y; matRotation._13 = vRight.z;
	matRotation._21 = vUp.x;	matRotation._22 = vUp.y;	matRotation._23 = vUp.z;
	matRotation._31 = vLook.x;	matRotation._32 = vLook.y;	matRotation._33 = vLook.z;
	RotationMatrix = XMLoadFloat4x4(&matRotation);

	TransMatrix = XMMatrixTranslation(m_vPosition.x, m_vPosition.y, m_vPosition.z);
	_matrix World = ScaleMatrix * RotationMatrix * TransMatrix;
	
	XMStoreFloat4x4(&m_WorldMatrix, World);
}

void CTransform::Move_Strafe(const _float& fUnits, const _float& fTimeDelta)
{
}

void CTransform::Move_Fly(const _float& fUnits, const _float& fTimeDelta)
{
}

void CTransform::Move_Walk(const _float& fUnits, const _float& fTimeDelta)
{
}

void CTransform::Rot_Pitch(const _float& fAngle, const _float& fTimeDelta)
{
}

void CTransform::Rot_Yaw(const _float& fAngle, const _float& fTimeDelta)
{
}

void CTransform::Rot_Roll(const _float& fAngle, const _float& fTimeDelta)
{
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
	CComponent::Free();
}
