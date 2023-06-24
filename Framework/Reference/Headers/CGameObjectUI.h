#pragma once

#include "CGameObject.h"

BEGIN(Engine)

class ENGINE_DLL CGameObjectUI abstract : public CGameObject
{
public:
	typedef struct tagUIDesc
	{
		tagUIDesc() = default;
		tagUIDesc(const _float& _fX, const _float& _fY, const _float& _fSizeX, const _float& _fSizeY)
			:m_fX(_fX), m_fY(_fY), m_fSizeX(_fSizeX), m_fSizeY(_fSizeY) { }
		_float m_fX = { 0.f };
		_float m_fY = { 0.f };
		_float m_fSizeX = { 0.f };
		_float m_fSizeY = { 0.f };
	}UIDESC;

protected:
	explicit CGameObjectUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CGameObjectUI(const CGameObjectUI& rhs);
	virtual ~CGameObjectUI() = default;

public:
	_float2 Get_Scale();
	void Set_Scale(const _float2& _vScale);
	void Set_Position(const _float2& _vPosition);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLayerIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float4x4 m_WorldMatrix;

protected:
	virtual HRESULT Add_Components() = 0;

public:
	virtual CGameObjectUI* Clone(const _uint& iLayerIndex, CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END