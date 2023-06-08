#pragma once

#include "IMGUI_Defines.h"
#include "CImWindow.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Imgui)

class CImWindow_Create final : public CImWindow
{
public:
    enum TYPE { TEXTURE, SHADER, BUFFER, TYPE_END };

private:
    explicit CImWindow_Create();
    virtual ~CImWindow_Create() = default;

public:
    _char m_szObjectName[256] = { "" };
    _char m_szPrototypeTag[TYPE_END][256] = { "" };
    _bool m_bIsOpen = { false };

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Refresh() override;

public:
    void Create_Object();
    void Create_Object_Pick(CGameInstance* pGameInstance);

private:
    _bool m_bIsPicking = { false };
    _uint m_iCloneNum = { 0 };
    _char* m_Types[TYPE_END] = { nullptr };
    _bool m_bTypes[TYPE_END] = { false };

public:
    static CImWindow_Create* Create(void* pArg = nullptr);
    virtual void Free() override;
};

END
