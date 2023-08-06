#pragma once

#include "IMGUI_Defines.h"
#include "CImWindow.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Imgui)

class CImWindow_Create final : public CImWindow
{
    DECLARE_SINGLETON(CImWindow_Create)

public:
    enum TYPE { TEXTURE, SHADER, BUFFER, MODEL, COLLIDER, TYPE_END };

private:
    explicit CImWindow_Create() = default;
    virtual ~CImWindow_Create() = default;

public:
    _char m_szObjectName[256] = { "" };
    _char m_szPrototypeTag[TYPE_END][256] = { "" };
    _char m_szSearchTag[256] = { "" };
    _bool m_bIsOpen = { false };

public:
    virtual HRESULT Initialize() override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Refresh() override;

public:
    void Create_Object(const _uint& iLayerIndex, CGameInstance* pGameInstance);
    void Create_Trigger(CGameInstance* pGameInstance);
    void Create_Object_Pick(CGameInstance* pGameInstance);

private:
    _bool m_bIsPicking = { false };
    _uint m_iCloneNum = { 0 };
    _char* m_Types[TYPE_END] = { nullptr };
    _tchar* m_LayerTag[LAYER_END] = { nullptr };
    _bool m_bTypes[TYPE_END] = { false };

    LAYERTYPE m_eCurLayer = { LAYER_END };

private: /* Trigger Data */
    _float m_arrExtents[3] = { 0.f };
    _float m_arrPosition[3] = { 0.f };

private:
    _vector Get_PickPos(CGameInstance* pGameInstance);

public:
    virtual void Free() override;
};

END
