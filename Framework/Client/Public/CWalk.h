#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CWalk final : public CBehavior
{
private:
	explicit CWalk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWalk(const CWalk& rhs);
	virtual ~CWalk() = default;

public:
	void Set_LimitTime(const _float& fTime) {
		m_fLimit = fTime;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

private:
	CTransform* m_pTransform = { nullptr };
	CModel* m_pModel = { nullptr };
	_float m_fTimeAcc = { 0.f };
	_float m_fLimit = { 0.f };

public:
	static CWalk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWalk* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END