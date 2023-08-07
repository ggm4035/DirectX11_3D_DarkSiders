#pragma once

#include "CComponent.h"
/* 객체의 월드 변환 상태를 가진다.(월드행렬을 보관한다.) */
/* 월드에서의 변환 기능을 가진다.(Go_Straight(), Go_Backword(), Turn() )*/

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
public:
	typedef struct tagTransformDesc
	{
		tagTransformDesc() = default;
		tagTransformDesc(_double _SpeedPerSec, _double _RotationPerSec)
			: SpeedPerSec(_SpeedPerSec)
			, RotationPerSec(_RotationPerSec)
		{ }
		_double SpeedPerSec = { 0.0 };
		_double RotationPerSec = { 0.0 };
	}TRASNFORMDESC;

private:
	explicit CTransform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	_float4x4 Get_WorldFloat4x4() {
		return m_WorldMatrix;
	}
	const _float4x4* Get_WorldFloat4x4Ptr() {
		return &m_WorldMatrix;
	}
	_matrix Get_WorldMatrix_Inverse();
	_vector Get_MoveDirection() {
		return XMLoadFloat3(&m_vMoveDir);
	}
	_float3 Get_Scaled();
	_float3 Get_Angle() {
		return m_vAngle;
	}
	_vector Get_State(STATE _eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[_eState];
	}
	_bool isJump() const {
		return m_isJump;
	}
	void Set_State(STATE _eState, _fvector _vState);
	void Set_Angle(const _float3& vAngle) {
		m_vAngle = vAngle;
	}
	void Set_On_Navigation(_bool bSwitch) {
		m_isOnNavigation = bSwitch;
	}
	void Reset_Jump();

public:
	/* Imgui Tool Only*/
	void Set_Matrix(const _float4x4& fMatrix) {
		m_WorldMatrix = fMatrix;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;

	HRESULT Bind_Navigation(class CNavigation* pNavigation);

public:
	void Animation_Movement(class CModel* pModel, const _double& TimeDelta);
	void Go(_fvector vDirection, const _double& TimeDelta, const _float& fTurnSpeed = 5.f);
	void Repersive(_fvector vOtherDir, const _double& TimeDelta);
	void Chase(_fvector vTargetPosition, const _double& TimeDelta, const _float& fMinDistance = 0.1f);
	_bool Chase_Lerp(_fvector vTargetPosition, _double TimeDelta, _float fMinDistance);
	_bool Jump(const _float& fForce, const _double& TimeDelta);
	void Reset_TimeAcc() {
		m_fTimeAcc = 0.f;
	}
	void LookAt(_fvector vTargetPosition);
	void Rotation(_fvector vAxis, const _float& fRadian);
	void Rotation(const _float3& rDegrees);
	void Turn_Axis(_fvector vAxis, const _double& TimeDelta);
	void Turn(_fvector vDir, const _double& TimeDelta, const _float& fSpeed);
	void BillBoard(const _double& TimeDelta);
	void Scaled(const _float3& vScale);

	/* 임시 (카메라 이동 용) */
	void Cam_Straight(const _double& TimeDelta);
	void Cam_Backward(const _double& TimeDelta);
	void Cam_Right(const _double& TimeDelta);
	void Cam_Left(const _double& TimeDelta);
	void Cam_Up(const _double& TimeDelta);

private:
	class CNavigation* m_pNavigation = { nullptr };
	_float4x4 m_WorldMatrix;
	_float3 m_vMoveDir; /* Look과 관계없는 객체의 이동방향 */
	_float3 m_vAngle;
	TRASNFORMDESC m_TransformDesc;

private: /* For. Jump */
	_float m_fTimeAcc = { 0.f };
	_bool m_isOnNavigation = { true };
	_bool m_isJump = { false };

private:
	void Move_Stop_Sliding(const _double& TimeDelta);
	void Organize_From_Right();
	void Organize_From_Up();
	void Organize_From_Look();

public:
	static CTransform* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END