#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CDetect final : public CBehavior
{
private:
	explicit CDetect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDetect(const CDetect& rhs);
	virtual ~CDetect() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	void Bind_AnimTag(const string& strAnimTag) {
		m_strAnimTag = strAnimTag;
	}

private:
	_bool m_isFirst = { true };
	string m_strAnimTag = { "" };

	CTransform* m_pTransform = { nullptr };
	CModel* m_pModel = { nullptr };

public:
	static CDetect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CDetect* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END