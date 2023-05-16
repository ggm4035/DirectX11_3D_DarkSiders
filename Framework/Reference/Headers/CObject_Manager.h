#pragma once

#include "CBase.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)

private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:/*실제 사용할 레벨의 갯수만큼 미리 공간을 할당한다. */
	HRESULT Reserve_Containers(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iNumLayer, const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg);
	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);
	void Clear_LevelResources(_uint iLevelIndex);

private:
	/* 원형객체들을 생성하여 보관하고 있는다. */
	unordered_map<const _tchar*, class CGameObject*> m_Prototypes;
	/* 사본객체들을 레이어로 구분하여 보관한다. */
	unordered_map<const _tchar*, class CLayer*>* m_pLayers = { nullptr };
	typedef unordered_map<const _tchar*, class CLayer*> LAYERS;
	
	_uint m_iNumLevels = { 0 };

private:
	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	class CLayer* Find_Layer(_uint iNumLayer, const _tchar* pGameObjectTag);

public:
	virtual void Free() override;
};

END