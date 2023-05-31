#pragma once

#include "CImWindow.h"

BEGIN(Imgui)

class CImWindow_Bottom final : public CImWindow
{
private:
    explicit CImWindow_Bottom();
    virtual ~CImWindow_Bottom() = default;

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(_double TimeDelta) override;

public:
    static CImWindow_Bottom* Create(void* pArg = nullptr);
    virtual void Free() override;
};

END
