#pragma once

#include "CBase.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)

private:
	explicit CObject_Manager();
	virtual ~CObject_Manager() = default;

public:/*실제 사용할 레벨의 갯수만큼 미리 공간을 할당한다. */
	HRESULT Reserve_Containers(const _uint& iNumLevels);
	HRESULT Add_Prototype(const wstring& pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(const _uint& iLevelIndex, const wstring& PrototypeTag, const wstring& GameObjectTag, const wstring& LayerTag, void* pArg);
	class CGameObject* Clone_GameObject(const _uint& iLevelIndex, const wstring& PrototypeTag, const wstring& GameObjectTag, class CComponent* pOwner, void* pArg);
	void Tick(const _double& TimeDelta);
	void Late_Tick(const _double& TimeDelta);
	void Clear_LevelResources(const _uint& iLevelIndex);

public: /* For. MapEditMode */
	list<CGameObject*> Get_All_GameObject();
	HRESULT Remove_GameObject(const wstring& GameObjectTag);

private:
	typedef unordered_map<wstring, class CLayer*> LAYERS;
	typedef unordered_map<wstring, class CGameObject*> PROTOTYPES;
	/* 원형객체들을 생성하여 보관하고 있는다. */
	PROTOTYPES m_Prototypes;
	/* 사본객체들을 레이어로 구분하여 보관한다.
		여기에 저장되는 객체들은 루트컴포넌트들인거임 */
	LAYERS* m_pLayers = { nullptr };
	
	_uint m_iNumLevels = { 0 };

private:
	class CGameObject* Find_Prototype(const wstring& PrototypeTag);
	class CLayer* Find_Layer(const _uint& iNumLayer, const wstring& GameObjectTag);

public:
	virtual void Free() override;
};

END