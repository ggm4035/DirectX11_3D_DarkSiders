#pragma once

#include "CImWindow.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
class CComponent;
class CAnimation;
END

BEGIN(Imgui)

class CImWindow_Base final : public CImWindow
{
private:
    explicit CImWindow_Base();
    virtual ~CImWindow_Base() = default;

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Refresh() override;

private:
    class CDummyObject3D* m_pModel = { nullptr };
    string m_strFilePath = { "" };
    vector<ANIMATIONDATA> m_vecAnimationDatas;
    list<CComponent*> m_PrototypeList;
    _bool m_isIntoAnimTab = { false };

    /* Export용 데이터임.. */
    vector<MODEL_BINARYDATA> m_vecBinData;
    list<string> m_FilePathList;

private:
    void Hierarchy(CGameInstance* pGameInstance);
    void UI(CGameInstance* pGameInstance);
    void Picking_GameObject(CGameInstance* pGameInstance);
    void Animation(CGameInstance* pGameInstance);
    void Add_Animation(CGameInstance* pGameInstance, const string& strTag);
    void Export_Animation(CGameInstance* pGameInstance);

    void Release_Animation_Data();
    void Release_Export_Data();

public:
    static CImWindow_Base* Create(void* pArg = nullptr);
    virtual void Free() override;
};

END
