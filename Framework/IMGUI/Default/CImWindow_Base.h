#pragma once

#include "CImWindow.h"

BEGIN(Engine)
class CGameInstance;
class CComponent;
END

BEGIN(Imgui)

class CImWindow_Base final : public CImWindow
{
    DECLARE_SINGLETON(CImWindow_Base)

private:
    explicit CImWindow_Base();
    virtual ~CImWindow_Base() = default;

public:
    virtual HRESULT Initialize() override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Refresh() override;

private:
    _bool m_isIntoAnimTab = { false };
    _bool m_PreAnimTab = { false };

private: /* For.Animation */
    class CTool_Animation* m_pToolAnimation = { nullptr };

public:
    class CTool_HeightMap* m_pToolHeightMap = { nullptr };

private:
    void Hierarchy(CGameInstance* pGameInstance);
    void HeightMap(CGameInstance* pGameInstance);
    void UI(CGameInstance* pGameInstance);
    void Create_Object(CGameInstance* pGameInstance);
    void Picking_GameObject(CGameInstance* pGameInstance);

public:
    virtual void Free() override;
};

END
