#pragma once

#include "Client_Defines.h"
#include "CBehavior.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CWait final : public CBehavior
{
private:
	explicit CWait(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWait(const CWait& rhs);
	virtual ~CWait() = default;

public:
	void Set_LimitTime(const _float& fTime) {
		m_fLimit = fTime;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

private:
	CModel* m_pModel = { nullptr };
	_float m_fTimeAcc = { 0.f };
	_float m_fLimit = { 0.f };

public:
	static CWait* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWait* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END