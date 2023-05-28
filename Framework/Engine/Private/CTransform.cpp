#include "CTransform.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_TransformDesc()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_TransformDesc(rhs.m_TransformDesc)
{
}

_matrix CTransform::Get_WorldMatrix_Inverse()
{
	return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
}

_float3 CTransform::Get_Scaled()
{
	return _float3(XMVector3Length(Get_State(STATE_RIGHT)).m128_f32[0], 
		XMVector3Length(Get_State(STATE_UP)).m128_f32[0], 
		XMVector3Length(Get_State(STATE_LOOK)).m128_f32[0]);
}

void CTransform::Set_State(STATE _eState, _fvector _vState)
{
	_float4 vState;
	XMStoreFloat4(&vState, _vState);
	memcpy(&m_WorldMatrix.m[_eState][0], &vState, sizeof vState);
}

HRESULT CTransform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memmove(&m_TransformDesc, pArg, sizeof m_TransformDesc);

	return S_OK;
}

void CTransform::Go_Straight(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Up(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Down(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vUp) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Chase(_fvector vTargetPosition, _double TimeDelta, _float fMinDistance)
{
	_vector vPosition = Get_State(STATE_POSITION);

	_vector vDirection = vTargetPosition - vPosition;

	if (XMVector3Length(vDirection).m128_f32[0] > fMinDistance)
		vPosition += XMVector3Normalize(vDirection) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::LookAt(_fvector vTargetPosition)
{
	_vector vPosition = Get_State(STATE_POSITION);

	_vector vLook = XMVector3Normalize(vTargetPosition - vPosition);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3 Scales = Get_Scaled();

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * Scales.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * Scales.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * Scales.z;

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _double TimeDelta)
{
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vUp = Get_State(STATE_UP);
	_vector vLook = Get_State(STATE_LOOK);

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.RotationPerSec * TimeDelta);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Scaled(const _float3& vScale)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * vScale.z);
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
