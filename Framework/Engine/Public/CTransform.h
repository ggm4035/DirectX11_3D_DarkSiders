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
		tagTransformDesc(_double _SpeedPerSec, _double _RotationPerSec)
			: SpeedPerSec(_SpeedPerSec)
			, RotationPerSec(_RotationPerSec)
		{ }
		_double SpeedPerSec;
		_double RotationPerSec;
	}TRASNFORMDESC;

private:
	CTransform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_float4x4 Get_WorldMatrix() {
		return m_WorldMatrix;
	}
	_float3 Get_Scaled();
	_vector Get_State(STATE _eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[_eState];
	}
	void Set_State(STATE _eState, _fvector _vState);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Go_Straight(_double TimeDelta);
	void Go_Backword(_double TimeDelta);
	void Go_Right(_double TimeDelta);
	void Go_Left(_double TimeDelta);
	void Go_Up(_double TimeDelta);
	void Go_Down(_double TimeDelta);
	void Chase(_fvector vTargetPosition, _double TimeDelta, _float fMinDistance = 0.1f);
	void LookAt(_fvector vTargetPosition);
	void Rotation(_fvector vAxis, _float fRadian);
	void Turn(_fvector vAxis, _double TimeDelta);

	void Scaled(const _float3& vScale);

private:
	_float4x4 m_WorldMatrix;
	TRASNFORMDESC m_TransformDesc;

public:
	static CTransform* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END