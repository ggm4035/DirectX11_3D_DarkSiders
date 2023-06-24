#pragma once

#include "IMGUI_Defines.h"
#include "CImWindow.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Imgui)

class CImWindow_Top final : public CImWindow
{
    DECLARE_SINGLETON(CImWindow_Top)

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
    _bool isSolidMode() {
        return m_isSolidMode;
    }

public:
    virtual HRESULT Initialize() override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Refresh() override;

private:
    string m_strPickMode = { "" };
    _bool m_isPicking = { false };

    _bool m_isPickSolidMode = { false };
    _bool m_isSolidMode = { false };

private:
    void RenderMode(CGameInstance* pGameInstance);
    void PickingMode(CGameInstance* pGameInstance);
    void Save(CGameInstance* pGameInstance);
    void Load(CGameInstance* pGameInstance);
    void Write_BinData(HANDLE hFile, MODEL_BINARYDATA& Data, _ulong dwByte);

public:
    virtual void Free() override;
};

END
