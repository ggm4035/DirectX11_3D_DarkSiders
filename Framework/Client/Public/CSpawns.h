#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CSpawns final : public CBehavior
{
private:
	explicit CSpawns(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSpawns(const CSpawns& rhs);
	virtual ~CSpawns() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta);

public:
	void Add_ModelTag(const wstring& wstrModelTag) {
		m_ModelTags.push_back(wstrModelTag);
	}
	void Set_SpawnRange(const _float& fMinX, const _float& fMaxX, const _float& fY, const _float& fMinZ, const _float& fMaxZ) {
		m_fMinX = fMinX;
		m_fMaxX = fMaxX;
		m_fY = fY;
		m_fMinZ = fMinZ;
		m_fMaxZ = fMaxZ;
	}
	void Set_NumMonster(const _uint iNumMonster) {
		m_iNumMonster = iNumMonster;
	}
	void Set_NavigationIndex(const _uint iNavigationIndex) {
		m_iNavigationIndex = iNavigationIndex;
	}

private:
	_float m_fTimeAcc = { 0.f };

	_float m_fMinX = { 0.f };
	_float m_fMaxX = { 0.f };
	_float m_fY = { 0.f };
	_float m_fMinZ = { 0.f };
	_float m_fMaxZ = { 0.f };

	_uint m_iNumMonster = { 0 };
	_uint m_iCount = { 0 };

	_uint m_iNavigationIndex = { 0 };

	vector<wstring> m_ModelTags;

public:
	static CSpawns* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpawns* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END