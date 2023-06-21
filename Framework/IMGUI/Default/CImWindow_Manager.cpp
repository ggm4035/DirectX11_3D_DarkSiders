#include "CImWindow_Manager.h"

IMPLEMENT_SINGLETON(CImWindow_Manager)

CImWindow_Manager::CImWindow_Manager()
{
}

HRESULT CImWindow_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // Show the window
    ::ShowWindow(g_hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(g_hWnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    ImGui::GetIO().ConfigWindowsResizeFromEdges = true;

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(pDevice, pContext);
    ImGui::StyleColorsDark();

    m_pDevice = pDevice;
    m_pContext = pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    return S_OK;
}

void CImWindow_Manager::Tick(const _double& TimeDelta)
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    for (auto ImWindow : m_ImWindowList)
        ImWindow->Tick(TimeDelta);
}

void CImWindow_Manager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

HRESULT CImWindow_Manager::Add_Window(CImWindow* pWindow)
{
    if (nullptr == pWindow)
        return E_FAIL;

    pWindow->Initialize();
    m_ImWindowList.push_back(pWindow);
    Safe_AddRef(pWindow);

    return S_OK;
}

void CImWindow_Manager::Refresh_All_Window()
{
    for (auto& window : m_ImWindowList)
        window->Refresh();
}

void CImWindow_Manager::Free(void)
{
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    for (auto& ImWindow : m_ImWindowList)
        Safe_Release(ImWindow);

    m_ImWindowList.clear();
}
