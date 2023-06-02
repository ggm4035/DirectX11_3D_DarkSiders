#pragma once

#include "IMGUI_Defines.h"
#include "CImWindow.h"

BEGIN(Imgui)

class CImWindow_Create final : public CImWindow
{
private:
    explicit CImWindow_Create();
    virtual ~CImWindow_Create() = default;

public:
    _bool m_bIsOpen = { false };

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Refresh() override;

public:
    void Create_Object();

public:
    static CImWindow_Create* Create(void* pArg = nullptr);
    virtual void Free() override;
};

END
