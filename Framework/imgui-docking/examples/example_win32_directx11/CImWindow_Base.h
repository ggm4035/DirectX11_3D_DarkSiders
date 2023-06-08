#pragma once

#include "CImWindow.h"

BEGIN(Engine)
class CGameObject;
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
    class CImWindow_Create* m_pCreateWindow = { nullptr };
    class CImWindow_Inspector* m_pInspectorWindow = { nullptr };

private:
    _float3 m_vTerrain_Position;
    _float3 m_vTerrain_Rotation;
    _bool m_bIsSolid = { false };
    string m_strCurNode = { "" };
    class CDummyObject3D* m_pTerrain = { nullptr };

private:
    void Hierarchy();
    void HeightMap();
    void Create_3D_Object();

public:
    static CImWindow_Base* Create(void* pArg = nullptr);
    virtual void Free() override;
};

END
