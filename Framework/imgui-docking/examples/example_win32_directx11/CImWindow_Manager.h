#pragma once

#include "IMGUI_Defines.h"
#include "CImWindow.h"

BEGIN(Imgui)

class CImWindow_Manager final : public CBase
{
    DECLARE_SINGLETON(CImWindow_Manager)

private:
    explicit CImWindow_Manager();
    ~CImWindow_Manager() = default;

public:
    CImWindow* Get_ImWindow(const _tchar* tag);

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Tick(_double TimeDelta);
    void Render();
    HRESULT Add_Window(const _tchar* tag, CImWindow* pWindow);

private:
    CImWindow* Find_Window(const _tchar* tag);

private:
    unordered_map<const _tchar*, CImWindow*>    m_ImWindows;

public:
    virtual void Free(void) override;
};

END
