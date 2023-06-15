#pragma once

#include "IMGUI_Defines.h"
#include "CImWindow.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Imgui)

class CImWindow_Top final : public CImWindow
{
private:
    explicit CImWindow_Top();
    virtual ~CImWindow_Top() = default;

public:
    _bool isPickingMode() {
        return m_isPicking;
    }
    _bool isPickSolidMode() {
        return m_isPickSolidMode;
    }

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Refresh() override;

private:
    string m_strPickMode = { "" };
    _bool m_isPicking = { false };

    _bool m_isPickSolidMode = { false };

private:
    void RenderMode(CGameInstance* pGameInstance);
    void PickingMode(CGameInstance* pGameInstance);
    void Save(CGameInstance* pGameInstance);
    HRESULT Save_Data(const string& strFilePath);
    void Load(CGameInstance* pGameInstance);

public:
    static CImWindow_Top* Create(void* pArg = nullptr);
    virtual void Free() override;
};

END
