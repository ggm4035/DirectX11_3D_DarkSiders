#pragma once

#include "IMGUI_Defines.h"
#include "CBase.h"

BEGIN(Engine)
class CGameInstance;
class CComponent;
END

BEGIN(Imgui)

class CTool_Animation final : public CBase
{
public:
    explicit CTool_Animation();
    virtual ~CTool_Animation() = default;

public:
    void Tick(CGameInstance* pGameInstance, list<class CDummyObject3D*>* pObjectList);

private:
    class CDummyObject3D* m_pModel = { nullptr };

    vector<ANIMATIONDATA> m_vecAnimationDatas;

    list<CComponent*> m_PrototypeList;
    string m_strFilePath = { "" };
    _uint m_iCurAnimIndex = { 0 };
    _uint m_iCurNotifyIndex = { 0 };

private:
    void Make_New_Model(CGameInstance* pGameInstance, list<class CDummyObject3D*>* pObjectList);
    void Add_Animation(CGameInstance* pGameInstance, const string& strTag);
    void Export_Animation(CGameInstance* pGameInstance);
    void KeyFrameSetting(CGameInstance* pGameInstance);
    void NotifySetting(CGameInstance* pGameInstance);

public:
    virtual void Free() override;
};

END
