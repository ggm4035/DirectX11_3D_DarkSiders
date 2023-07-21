#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CRest final : public CBehavior
{
private:
	explicit CRest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRest(const CRest& rhs);
	virtual ~CRest() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	void Add_AnimTag(const string& strAnimTag) {
		m_AnimTagList.push_back(strAnimTag);
		m_iterAnimTag = m_AnimTagList.begin();
	}

private:
	_bool m_isFirst = { true };
	list<string> m_AnimTagList;
	list<string>::iterator m_iterAnimTag;

	CTransform* m_pTransform = { nullptr };
	CModel* m_pModel = { nullptr };

public:
	static CRest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRest* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END