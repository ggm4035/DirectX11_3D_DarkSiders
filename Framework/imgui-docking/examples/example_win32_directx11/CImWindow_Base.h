#pragma once

#include "CImWindow.h"

BEGIN(Imgui)

class CImWindow_Base final : public CImWindow
{
private:
    explicit CImWindow_Base();
    virtual ~CImWindow_Base() = default;

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(_double TimeDelta) override;

private: /* For.Terrain_Values */
    _float3 m_vTerrain_Position;
    _float3 m_vTerrain_Rotation;
    _bool m_bIsSolid = { false };

private: /* For.UI_Values */


private:
    void Hierarchy();
    void Tab_Terrain();

public:
    static CImWindow_Base* Create(void* pArg = nullptr);
    virtual void Free() override;
};

END
