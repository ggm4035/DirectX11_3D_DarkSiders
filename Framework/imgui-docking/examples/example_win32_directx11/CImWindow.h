#pragma once

#include "Imgui_Defines.h"
#include "CBase.h"

BEGIN(Imgui)

class CImWindow abstract : public CBase
{
protected:
    explicit CImWindow();
    virtual ~CImWindow() = default;

public:
    virtual HRESULT Initialize(void* pArg) = 0;
    virtual void Tick(_double TimeDelta) = 0;

public:
    virtual void Free(void) override;
};

END
