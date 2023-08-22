#pragma once

#include "CSequence.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CPattern_Roll final : public CSequence
{
private:
	explicit CPattern_Roll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPattern_Roll(const CPattern_Roll& rhs);
	virtual ~CPattern_Roll() = default;

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
	class CRoll_Effect* m_pEffect = { nullptr };

public:
	static CPattern_Roll* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPattern_Roll* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END