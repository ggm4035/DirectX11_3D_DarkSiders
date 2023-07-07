#pragma once

#include "CImWindow.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Imgui)

class CImWindow_Base final : public CImWindow
{
    DECLARE_SINGLETON(CImWindow_Base)

private:
    explicit CImWindow_Base() = default;
    virtual ~CImWindow_Base() = default;

public:
    virtual HRESULT Initialize() override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Refresh() override;

private:
    _bool m_isIntoAnimTab = { false };
    _bool m_PreAnimTab = { false };

public:
    class CTool_Animation* m_pToolAnimation = { nullptr };
    class CTool_HeightMap* m_pToolHeightMap = { nullptr };
    class CTool_UI* m_pTool_UI = { nullptr };

private:
    void Hierarchy(CGameInstance* pGameInstance);
    void Create_Object(CGameInstance* pGameInstance);
    void Picking_GameObject(CGameInstance* pGameInstance);

public:
    virtual void Free() override;
};

END
