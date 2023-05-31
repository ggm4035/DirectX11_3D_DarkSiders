#pragma once

#include "CImWindow.h"

BEGIN(Imgui)

class CImWindow_Top final : public CImWindow
{
private:
    explicit CImWindow_Top();
    virtual ~CImWindow_Top() = default;

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(_double TimeDelta) override;

public:
    static CImWindow_Top* Create(void* pArg = nullptr);
    virtual void Free() override;
};

END
