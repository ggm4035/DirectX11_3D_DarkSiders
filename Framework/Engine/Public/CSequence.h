#pragma once

#include "CBehavior.h"

BEGIN(Engine)

class ENGINE_DLL CSequence : public CBehavior
{
protected:
	explicit CSequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSequence(const CSequence& rhs);
	virtual ~CSequence() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta) final;

public:
	static CSequence* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSequence* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
	virtual void Free() override;
};

END