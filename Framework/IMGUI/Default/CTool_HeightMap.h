#pragma once

#include "IMGUI_Defines.h"
#include "CBase.h"

BEGIN(Engine)
class CGameInstance;
class CTexture;
END

BEGIN(Imgui)

class CTool_HeightMap final : public CBase
{
public:
    explicit CTool_HeightMap();
    virtual ~CTool_HeightMap() = default;

public:
    const class CDummyObject3D* Get_Terrain() const {
        return m_pTerrain;
    }
    void Set_Terrain(class CDummyObject3D* pTerrain) {
        m_pTerrain = pTerrain;
    }

public:
    void Initialize();
    void Tick(CGameInstance* pGameInstance);

private:
    class CDummyObject3D* m_pTerrain = { nullptr };
    CTexture* m_pBrushTextureCom = { nullptr };
    _float4 m_vBrushPos;
    _float m_fRadius = { 10.f };
    _float m_fMaxDepth = { 10.f };
    _float m_fStandard = { 0.f };
    _float m_fSharpness = { 0.f };
    _float m_fDir = { 1.f };

private: /* For. Navigation Map */
    TRIANGLE m_Triangle;
    _uint m_iCount = { 0 };
    _int m_iSelectOption = { 0 };
    _int m_iPickCell = { 0 };
    vector<pair<_uint, _int>> m_vecPickSphereInfo;/* first : CellIndex, second : Sphere Index */
    vector<TRIANGLE> m_vecPoints; /* ÀúÀå¿ë */

public:
    _int m_iXCount = { 257 };
    _int m_iZCount = { 257 };
    _float m_fInterval = { 1.f };

private:
    HRESULT Bind_Shader_Resources();
    void Setting_Terrain(CGameInstance* pGameInstance);
    void Control_Height(CGameInstance* pGameInstance);
    void Draw_Filter(CGameInstance* pGameInstance);
    void Make_Navigation(CGameInstance* pGameInstance);
    void Pick_Navigation(CGameInstance* pGameInstance);
    void Export_Navigation();
    void Load_Navigation();

public:
    virtual void Free() override;
};

END
