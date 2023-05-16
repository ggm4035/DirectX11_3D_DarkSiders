#pragma once

#include "CBase.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)

private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:/*���� ����� ������ ������ŭ �̸� ������ �Ҵ��Ѵ�. */
	HRESULT Reserve_Containers(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iNumLayer, const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg);
	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);
	void Clear_LevelResources(_uint iLevelIndex);

private:
	/* ������ü���� �����Ͽ� �����ϰ� �ִ´�. */
	unordered_map<const _tchar*, class CGameObject*> m_Prototypes;
	/* �纻��ü���� ���̾�� �����Ͽ� �����Ѵ�. */
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