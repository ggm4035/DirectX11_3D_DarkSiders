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
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Tick(const _double& TimeDelta);
    void Render();

    HRESULT Add_Window(CImWindow* pWindow);
    void Refresh_All_Window();

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

private:
    list<CImWindow*>    m_ImWindowList;

public:
    virtual void Free(void) override;
};

END

#define WINDOWMGR CImWindow_Manager::GetInstance()
