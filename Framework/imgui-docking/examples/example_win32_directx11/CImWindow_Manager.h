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
    CImWindow* Get_ImWindow(wstring tag);

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Tick(const _double& TimeDelta);
    void Render();
    HRESULT Add_Window(wstring tag, CImWindow* pWindow);
    void Refresh_All_Window();

private:
    CImWindow* Find_Window(wstring tag);

private:
    unordered_map<wstring, CImWindow*>    m_ImWindows;

public:
    virtual void Free(void) override;
};

END

#define WINDOWMGR CImWindow_Manager::GetInstance()
