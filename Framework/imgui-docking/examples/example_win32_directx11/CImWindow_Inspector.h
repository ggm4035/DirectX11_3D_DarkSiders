#pragma once

#include "CImWindow.h"

BEGIN(Imgui)

class CImWindow_Inspector final : public CImWindow
{
private:
    explicit CImWindow_Inspector();
    virtual ~CImWindow_Inspector() = default;

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(_double TimeDelta) override;

public:
    static CImWindow_Inspector* Create(void* pArg = nullptr);
    virtual void Free() override;
};

END
