#pragma once

#include "CComponent.h"

class CTransform final : public CComponent
{
public:
	enum MOVETYPE { LANDOBJECT, AIRCRAFT, MOVE_END };

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	const _matrix Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Tick(_double TimeDelta);

	void Move_Strafe(const _float& fUnits, const _float& fTimeDelta);// Move-Right
	void Move_Fly(const _float& fUnits, const _float& fTimeDelta);	// Move-Up
	void Move_Walk(const _float& fUnits, const _float& fTimeDelta);  // Move-Look

	void Rot_Pitch(const _float& fAngle, const _float& fTimeDelta);// Rot-Right
	void Rot_Yaw(const _float& fAngle, const _float& fTimeDelta);  // Rot-Up
	void Rot_Roll(const _float& fAngle, const _float& fTimeDelta); // Rot-Look

private:
	_float3 m_vRight;
	_float3 m_vUp;
	_float3 m_vLook;
	_float3 m_vPosition;
	_float3 m_vScale;

	_float4x4 m_WorldMatrix;

	MOVETYPE m_eMoveType;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

