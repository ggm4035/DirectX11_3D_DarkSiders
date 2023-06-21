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
    void Tick(CGameInstance* pGameInstance, list<class CDummyObject3D*>& ObjectList);

private:
    class CDummyObject3D* m_pModel = { nullptr };

    vector<ANIMATIONDATA> m_vecAnimationDatas;
    list<CComponent*> m_PrototypeList;
    string m_strFilePath = { "" };

    /* Export용 데이터 */
    vector<MODEL_BINARYDATA> m_vecBinData;
    list<string> m_FilePathList;

private:
    void Make_New_Model(CGameInstance* pGameInstance, list<class CDummyObject3D*>& ObjectList);
    void Add_Animation(CGameInstance* pGameInstance, const string& strTag);
    void Export_Animation(CGameInstance* pGameInstance);
    void Release_Animation_Data();
    void Release_Export_Data();

public:
    virtual void Free() override;
};

END
