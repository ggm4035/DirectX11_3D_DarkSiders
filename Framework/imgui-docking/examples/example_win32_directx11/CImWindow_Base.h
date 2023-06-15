#pragma once

#include "CImWindow.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
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
    _bool m_isFirst = { true };

private:
    void Hierarchy(CGameInstance* pGameInstance);
    void UI(CGameInstance* pGameInstance);
    void Picking_GameObject(CGameInstance* pGameInstance);
    void Animation(CGameInstance* pGameInstance);

public:
    static CImWindow_Base* Create(void* pArg = nullptr);
    virtual void Free() override;
};

END
