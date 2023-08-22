#include "CTransform.h"

#include "CModel.h"
#include "CNavigation.h"
#include "CGameObject3D.h"
#include "CCollider.h"
#include "CPipeLine.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_TransformDesc()
{
	ZeroMemory(&m_vAngle, sizeof(_float3));
	ZeroMemory(&m_vMoveDir, sizeof(_float3));
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_TransformDesc(rhs.m_TransformDesc)
	, m_vAngle(rhs.m_vAngle)
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

void CTransform::Set_Scales(const _float3& _vScale)
{
	_float3 vScale = Get_Scaled();

	_vector vRight = Get_State(STATE_RIGHT) / vScale.x * _vScale.x;
	_vector vUp = Get_State(STATE_UP) / vScale.y * _vScale.y;
	_vector vLook = Get_State(STATE_LOOK) / vScale.z * _vScale.z;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Set_State(STATE _eState, _fvector _vState)
{
	_float4 vState;
	XMStoreFloat4(&vState, _vState);
	memcpy(&m_WorldMatrix.m[_eState][0], &vState, sizeof vState);

	/*switch (_eState)
	{
	case Engine::CTransform::STATE_RIGHT:
		Organize_From_Right();
		break;

	case Engine::CTransform::STATE_UP:
		Organize_From_Up();
		break;

	case Engine::CTransform::STATE_LOOK:
		Organize_From_Look();
		break;
	}*/
}

void CTransform::Reset_Jump()
{
	m_isOnNavigation = true;
	m_fTimeAcc = 0.f;
	m_isJump = false;
}

HRESULT CTransform::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComponent::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_TransformDesc.SpeedPerSec = reinterpret_cast<TRASNFORMDESC*>(pArg)->SpeedPerSec;
		m_TransformDesc.RotationPerSec = reinterpret_cast<TRASNFORMDESC*>(pArg)->RotationPerSec;
	}

	return S_OK;
}

HRESULT CTransform::Bind_Navigation(CNavigation* pNavigation)
{
	m_pNavigation = pNavigation;
	Safe_AddRef(m_pNavigation);

	return S_OK;
}

void CTransform::Animation_Movement(class CModel* pModel, const _double& TimeDelta)
{
	if (true == pModel->isFinishedAnimation())
		return;

	_vector vLook = Get_State(STATE_LOOK);

	_vector vPosition = pModel->ComputeAnimMovement(&m_vMoveDir);

	CNavigation::RETURNDESC RetDesc;
	RetDesc.eMoveType = CNavigation::TYPE_MOVE;

	/* Move Check */
	if (nullptr != m_pNavigation)
		RetDesc = m_pNavigation->is_Move(vPosition);

	/* Sliding */
	if (CNavigation::TYPE_SLIDING == RetDesc.eMoveType)
	{
		_float fForce = XMVector3Dot(XMVector3Normalize(vLook), XMVector3Normalize(XMLoadFloat3(&RetDesc.vSlide))).m128_f32[0];
		_vector vDir = XMLoadFloat3(&RetDesc.vSlide);

		vPosition = Get_State(STATE_POSITION);

		if (90.f < acosf(fForce))
			vDir *= -1.f;

		vPosition += vDir * m_TransformDesc.SpeedPerSec * fForce * TimeDelta;
		XMStoreFloat3(&m_vMoveDir, XMVector3Normalize(vDir));
	}

	/* Stop */
	else if (CNavigation::TYPE_STOP == RetDesc.eMoveType)
		vPosition = Get_State(STATE_POSITION);

	if (nullptr != m_pNavigation &&
		true == m_isOnNavigation)
	{
		_float fNaviY = m_pNavigation->is_OnNavigation(vPosition);

		if (vPosition.m128_f32[1] != fNaviY)
			vPosition.m128_f32[1] = fNaviY;
	}

	XMStoreFloat3(&m_vMoveDir, vLook);
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go(_fvector vDirection, const _double& TimeDelta, const _float& fTurnSpeed)
{
	if (nullptr == m_pNavigation)
		return;

	Turn(vDirection, TimeDelta, fTurnSpeed);

	Move_Stop_Sliding(TimeDelta);

	if (true == m_isOnNavigation)
	{
		_vector vPosition = Get_State(STATE_POSITION);
		vPosition.m128_f32[1] = m_pNavigation->is_OnNavigation(vPosition);
		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Repersive(_fvector vOtherDir, const _double& TimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vDirection = XMVector3Normalize(vOtherDir);
	_float fDistance = XMVectorGetX(XMVector3Length(vOtherDir));

	vPosition += vDirection * m_TransformDesc.SpeedPerSec * TimeDelta * fDistance;

	CNavigation::RETURNDESC RetDesc;
	RetDesc.eMoveType = CNavigation::TYPE_MOVE;

	if (nullptr != m_pNavigation)
		RetDesc = m_pNavigation->is_Move(vPosition);

	if (CNavigation::TYPE_SLIDING == RetDesc.eMoveType)
	{
		/* 슬라이딩 */
		_float fForce = XMVector3Dot(XMVector3Normalize(vDirection), XMVector3Normalize(XMLoadFloat3(&RetDesc.vSlide))).m128_f32[0];
		_vector vDir = XMLoadFloat3(&RetDesc.vSlide);

		vPosition = Get_State(STATE_POSITION);

		if (90.f < acosf(fForce))
			vDir *= -1.f;

		vPosition += vDir * m_TransformDesc.SpeedPerSec * fForce * TimeDelta;
	}

	else if (CNavigation::TYPE_STOP == RetDesc.eMoveType)
		return;

	if (nullptr != m_pNavigation &&
		true == m_isOnNavigation)
	{
		_float fNaviY = m_pNavigation->is_OnNavigation(vPosition);

		if (vPosition.m128_f32[1] != fNaviY)
			vPosition.m128_f32[1] = fNaviY;
	}

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Chase(_fvector vTargetPosition, const _double& TimeDelta, const _float& fMinDistance)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vDirection = vTargetPosition - vPosition;

	m_isOnNavigation = true;

	if (XMVector3Length(vDirection).m128_f32[0] > fMinDistance)
		vPosition += XMVector3Normalize(vDirection) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

_bool CTransform::Chase_Lerp(_fvector vTargetPosition, _double TimeDelta, _float fMinDistance)
{
	//현재 내 위치
	_vector        vPosition = Get_State(STATE_POSITION);
	//거리
	_vector        vDir = vTargetPosition - vPosition;
	if (fMinDistance < XMVectorGetX(XMVector3Length(vDir)))
	{
		vPosition = XMVectorLerp(vPosition, vTargetPosition, 
			TimeDelta * m_TransformDesc.SpeedPerSec);
		Set_State(STATE_POSITION, vPosition);
		return true;
	}

	return false;
}

_bool CTransform::Jump(const _float& fForce, const _double& TimeDelta)
{
	if (nullptr == m_pNavigation)
		return false;

	m_isJump = true;
	m_isOnNavigation = false;
	m_fTimeAcc += TimeDelta;;

	_float fY = fForce * m_fTimeAcc - 0.8f * m_fTimeAcc * m_fTimeAcc;

	_vector vPosition = Get_State(STATE_POSITION);

	vPosition.m128_f32[1] += fY;
	Set_State(STATE_POSITION, vPosition);

	_float fPlaneY = m_pNavigation->is_OnNavigation(vPosition);

	if (0.f > fY && fPlaneY > vPosition.m128_f32[1])
	{
		m_isOnNavigation = true;
		vPosition.m128_f32[1] = fPlaneY;
		Set_State(STATE_POSITION, vPosition);
		m_fTimeAcc = 0.f;
		m_isJump = false;
		return false;
	}

	return true;
}

void CTransform::LookAt(_fvector vTargetPosition)
{
	_float3 Scales = Get_Scaled();

	_vector vPosition = Get_State(STATE_POSITION);

	_vector vLook = XMVector3Normalize(vTargetPosition - vPosition) * Scales.z;
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * Scales.x;
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * Scales.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Rotation(_fvector vAxis, const _float& fRadian)
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

void CTransform::Rotation(const _float3& rDegrees)
{
	m_vAngle = rDegrees;

	_float3 vScale = Get_Scaled();
	_matrix RotationMatrixX, RotationMatrixY, RotationMatrixZ;

	RotationMatrixX = XMMatrixRotationX(XMConvertToRadians(rDegrees.x));
	RotationMatrixY = XMMatrixRotationY(XMConvertToRadians(rDegrees.y));
	RotationMatrixZ = XMMatrixRotationZ(XMConvertToRadians(rDegrees.z));

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix RotationMatrix = RotationMatrixX * RotationMatrixY * RotationMatrixZ;

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn_Axis(_fvector vAxis, const _double& TimeDelta)
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
	if (0 == vScale.x || 0 == vScale.y || 0 == vScale.z)
		return;

	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * vScale.z);
}

void CTransform::Cam_Straight(const _double& TimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Cam_Backward(const _double& TimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Cam_Right(const _double& TimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Cam_Left(const _double& TimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Cam_Up(const _double& TimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Turn(_fvector Dir, const _double& TimeDelta, const _float& fSpeed)
{
	_vector vDir = XMVector3Normalize(Dir);
	_vector vLook = Get_State(STATE_LOOK);

	_float fDegree = acosf(XMVector3Dot(vDir, vLook).m128_f32[0]);
	_float fForce = fSpeed * fDegree * TimeDelta;

	fDegree = XMConvertToDegrees(fDegree);

	if (0.f > XMVector3Cross(vLook, vDir).m128_f32[1])
		fForce = -fForce;

	if (-0.0f < fDegree && 0.5f > fabs(fForce))
		Turn_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fForce);
}

void CTransform::BillBoard(const _double& TimeDelta)
{
	_float3 vScaled = Get_Scaled();

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_vector vCamPosition = XMLoadFloat4(&pPipeLine->Get_Camera_Position());

	Safe_Release(pPipeLine);

	_vector vLook = XMVector3Normalize(vCamPosition - Get_State(STATE_POSITION));
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	vRight *= vScaled.x;
	vUp *= vScaled.y;
	vLook *= vScaled.z;

	memcpy(&m_WorldMatrix.m[STATE_RIGHT][0], &vRight, sizeof(_float4));
	memcpy(&m_WorldMatrix.m[STATE_UP][0], &vUp, sizeof(_float4));
	memcpy(&m_WorldMatrix.m[STATE_LOOK][0], &vLook, sizeof(_float4));
}

void CTransform::Move_Stop_Sliding(const _double& TimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.SpeedPerSec * TimeDelta;

	CNavigation::RETURNDESC RetDesc;
	RetDesc.eMoveType = CNavigation::TYPE_MOVE;

	if (nullptr != m_pNavigation)
		RetDesc = m_pNavigation->is_Move(vPosition);

	if (CNavigation::TYPE_SLIDING == RetDesc.eMoveType)
	{
		/* 슬라이딩 */
		_float fForce = XMVector3Dot(XMVector3Normalize(vLook), XMVector3Normalize(XMLoadFloat3(&RetDesc.vSlide))).m128_f32[0];
		_vector vDir = XMLoadFloat3(&RetDesc.vSlide);

		vPosition = Get_State(STATE_POSITION);

		if (90.f < acosf(fForce))
			vDir *= -1.f;

		XMStoreFloat3(&m_vMoveDir, vDir);
		vPosition += vDir * m_TransformDesc.SpeedPerSec * fForce * TimeDelta;
	}

	else if (CNavigation::TYPE_STOP == RetDesc.eMoveType)
		return;

	XMStoreFloat3(&m_vMoveDir, vLook);
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Organize_From_Right()
{
	_float3 vScaled = Get_Scaled();
	_vector vRight = XMVector3Normalize(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_RIGHT]);
	_vector vLook = XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	vRight *= vScaled.x;
	vUp *= vScaled.y;
	vLook *= vScaled.z;

	memcpy(&m_WorldMatrix.m[STATE_RIGHT][0], &vRight, sizeof(_float4));
	memcpy(&m_WorldMatrix.m[STATE_UP][0], &vUp, sizeof(_float4));
	memcpy(&m_WorldMatrix.m[STATE_LOOK][0], &vLook, sizeof(_float4));
}

void CTransform::Organize_From_Up()
{
	_float3 vScaled = Get_Scaled();
	_vector vUp = XMVector3Normalize(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_UP]);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, XMVectorSet(0.f, 0.f, 1.f, 0.f)));
	_vector vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	vRight *= vScaled.x;
	vUp *= vScaled.y;
	vLook *= vScaled.z;

	memcpy(&m_WorldMatrix.m[STATE_RIGHT][0], &vRight, sizeof(_float4));
	memcpy(&m_WorldMatrix.m[STATE_UP][0], &vUp, sizeof(_float4));
	memcpy(&m_WorldMatrix.m[STATE_LOOK][0], &vLook, sizeof(_float4));
}

void CTransform::Organize_From_Look()
{
	_float3 vScaled = Get_Scaled();
	_vector vLook = XMVector3Normalize(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_LOOK]);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	vRight *= vScaled.x;
	vUp *= vScaled.y;
	vLook *= vScaled.z;

	memcpy(&m_WorldMatrix.m[STATE_RIGHT][0], &vRight, sizeof(_float4));
	memcpy(&m_WorldMatrix.m[STATE_UP][0], &vUp, sizeof(_float4));
	memcpy(&m_WorldMatrix.m[STATE_LOOK][0], &vLook, sizeof(_float4));
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

CComponent* CTransform::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
	Safe_Release(m_pNavigation);

	CComponent::Free();
}
