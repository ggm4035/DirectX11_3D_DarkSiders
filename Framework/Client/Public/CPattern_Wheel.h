#pragma once

#include "CSequence.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CPattern_Wheel final : public CSequence
{
private:
	explicit CPattern_Wheel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPattern_Wheel(const CPattern_Wheel& rhs);
	virtual ~CPattern_Wheel() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta) override;

public:
	HRESULT Assemble_Childs();
	void Set_CoolTime(const _float& fTime) {
		m_fLimit = fTime;
	}

private:
	_float m_fPreTimeAcc = { 0.f };
	_float m_fLimit = { 0.f };

public:
	static CPattern_Wheel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPattern_Wheel* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END