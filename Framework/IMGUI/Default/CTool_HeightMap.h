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

public:
    _int m_iXCount = { 129 };
    _int m_iZCount = { 129 };
    _float m_fInterval = { 1.f };

private:
    HRESULT Bind_Shader_Resources();
    void Control_Height(CGameInstance* pGameInstance);
    void Draw_Filter(CGameInstance* pGameInstance);

public:
    virtual void Free() override;
};

END
