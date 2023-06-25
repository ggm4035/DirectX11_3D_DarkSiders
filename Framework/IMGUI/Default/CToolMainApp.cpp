#include "CToolMainApp.h"

#include "CGameInstance.h"
#include "CToolInstance.h"
#include "CImWindow_Manager.h"
#include "CLevel_Tool.h"

#include "CCoordnate_Axis.h"
#include "CMainCamera.h"
#include "CDummyObject3D.h"

#include "CImWindow_Top.h"
#include "CImWindow_Base.h"
#include "CImWindow_Inspector.h"
#include "CImWindow_Create.h"

CToolMainApp::CToolMainApp()
    : m_pGameInstance(CGameInstance::GetInstance())
    , m_pImWindow_Manager(CImWindow_Manager::GetInstance())
{
    Safe_AddRef(m_pImWindow_Manager);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CToolMainApp::Initialize()
{
    if (FAILED(CoInitializeEx(nullptr, 0)))
        return E_FAIL;

    // Initialize Engine
    GRAPHICDESC GraphicDesc;
    GraphicDesc.eWinMode = GRAPHICDESC::WM_WIN;
    GraphicDesc.hWnd = g_hWnd;
    GraphicDesc.hInst = g_hInst;
    GraphicDesc.iViewportSizeX = g_iWinSizeX;
    GraphicDesc.iViewportSizeY = g_iWinSizeY;
    FAILED_CHECK_RETURN(m_pGameInstance->Initialize_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext), E_FAIL);

    // Initialize Imgui
    m_pImWindow_Manager->Initialize(m_pDevice, m_pContext);

    // Initialize Compoent
    if (FAILED(Ready_Prototype_Component_For_Tool()))
        return E_FAIL;

    // Initialize AnimModels
    if (FAILED(Ready_AnimModels()))
        return E_FAIL;

    // Initialize NonAnimModels
    if (FAILED(Ready_NonAnimModels()))
        return E_FAIL;

    // Initialize GameObject();
    if (FAILED(Ready_Prototype_GameObject_For_Tool()))
        return E_FAIL;

    // Initialize ImWindows
    if (FAILED(Ready_ImWindows()))
        return E_FAIL;

    // Initialize Level
    FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_TOOL, CLevel_Tool::Create(m_pDevice, m_pContext)), E_FAIL);

    return S_OK;
}

void CToolMainApp::Tick(const _double& TimeDelta)
{
    m_pImWindow_Manager->Tick(TimeDelta);
    Update_Demo();
    m_pGameInstance->Tick_Engine(TimeDelta);
}

HRESULT CToolMainApp::Render()
{
    m_pGameInstance->Clear_BackBuffer_View(m_vClear_Color);
    m_pGameInstance->Clear_DepthStencil_View();

    if (m_pRenderer != nullptr)
        m_pRenderer->Draw_RenderGroup();

    m_pImWindow_Manager->Render();
    m_pGameInstance->Present();

    return S_OK;
}

HRESULT CToolMainApp::Open_Level(LEVELID eLevelIndex)
{
    if (nullptr == m_pGameInstance)
        return E_FAIL;

    return m_pGameInstance->Open_Level(eLevelIndex, CLevel_Tool::Create(m_pDevice, m_pContext));
}

HRESULT CToolMainApp::Ready_ImWindows()
{
    if (FAILED(m_pImWindow_Manager->Add_Window(CImWindow_Base::GetInstance())))
        return E_FAIL;

    if (FAILED(m_pImWindow_Manager->Add_Window(CImWindow_Create::GetInstance())))
        return E_FAIL;

    if (FAILED(m_pImWindow_Manager->Add_Window(CImWindow_Inspector::GetInstance())))
        return E_FAIL;

    if (FAILED(m_pImWindow_Manager->Add_Window(CImWindow_Top::GetInstance())))
        return E_FAIL;

    return S_OK;
}

HRESULT CToolMainApp::Ready_Prototype_GameObject_For_Tool()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (FAILED(pGameInstance->Add_Prototype(L"Prototype_GameObject_Camera_Main",
        CMainCamera::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_Prototype(L"Prototype_GameObject_Coordnate_Axis",
        CCoordnate_Axis::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_Prototype(L"Prototype_GameObject_Dummy3D",
        CDummyObject3D::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CToolMainApp::Ready_Prototype_Component_For_Tool()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    /* Navigation Component */
    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"Navigation",
        CNavigation::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Shader Component */
    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"Shader_VtxNorTex",
        CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxNorTex.hlsl",
            VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"Shader_Mesh",
        CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxMesh.hlsl",
            VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"Shader_AnimMesh",
        CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl",
            VTXANIMMESH_DECL::Elements, VTXANIMMESH_DECL::iNumElements))))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"Shader_VtxCol",
        CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxCol.hlsl",
            VTXPOSCOL_DECL::Elements, VTXPOSCOL_DECL::iNumElements))))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"Shader_VtxPos",
        CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_Vtx.hlsl",
            VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements))))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"Shader_VtxTex",
        CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_VtxTex.hlsl",
            VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
        return E_FAIL;

    /* VIBuffer Component */
    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"VIBuffer_Terrain",
        CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 129, 257, 1.f))))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"VIBuffer_Terrain_Height",
        CVIBuffer_Terrain::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Terrain/ExportHeightMap.bmp"))))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"VIBuffer_Cube",
        CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"VIBuffer_Rect",
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"VIBuffer_Coordnate",
        CVIBuffer_Coordnate::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Renderer Component */
    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"Renderer",
        m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    Safe_AddRef(m_pRenderer);

    /* Texture Component */
    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"Texture_White",
        CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Terrain/White.png"))))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"Texture_Terrain",
        CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Terrain/Terrain5.png"))))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, L"Texture_Brush",
        CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Terrain/Brush.png"))))
        return E_FAIL;

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CToolMainApp::Ready_NonAnimModels()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    /* Model 경로 설정 */
    list<string> ModelPaths;
    pGameInstance->Extraction_Data("../../ModelDatas/NonAnimModels", ".dat", ModelPaths);

    /* PivotMatrix 설정*/
    _matrix PivotMatrix = XMMatrixIdentity();
    PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(-XMConvertToRadians(90.f));

    /* Model Read 작업 실행*/
    _uint iIndex = 0;
    for (auto& Path : ModelPaths)
    {
        pGameInstance->ReadModels(Path, m_FilePaths, m_vecModelDatas);

        wstring wstrTag = L"";
        wstrTag = wstrTag + L"Model_" + m_vecModelDatas[iIndex].szTag;

        if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, wstrTag.c_str(),
            CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, m_vecModelDatas[iIndex++], PivotMatrix))))
            return E_FAIL;
    }
    TOOL->m_pModelDatas = &m_vecModelDatas;

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CToolMainApp::Ready_AnimModels()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    /* Model 경로 설정 */
    list<string> ModelPaths;
    pGameInstance->Extraction_Data("../../ModelDatas/AnimModels", ".dat", ModelPaths);

    /* PivotMatrix 설정*/
    _matrix PivotMatrix = XMMatrixIdentity();
    PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(-XMConvertToRadians(90.f));

    /* Model Read 작업 실행*/
    _uint iIndex = 0;
    for (auto& Path : ModelPaths)
    {
        wstring wstrTag = L"";
        pGameInstance->ReadModels(Path, m_FilePaths, m_vecAnimModelData);

        wstring wstrName = m_vecAnimModelData[iIndex].szTag;

        if (wstring::npos != wstrName.find(L"_"))
            wstrTag = L"Anim_";
        else
            wstrTag = L"Model_";

        wstrTag += wstrName;

        if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, wstrTag.c_str(),
            CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, m_vecAnimModelData[iIndex++], PivotMatrix))))
            return E_FAIL;
    }

    TOOL->m_pAnimModelDatas = &m_vecAnimModelData;
    TOOL->m_pFilePaths = &m_FilePaths;

    Safe_Release(pGameInstance);

    return S_OK;
}

void CToolMainApp::Update_Demo()
{
    // Our state
    static bool show_demo_window = true;
    static bool show_another_window = false;

    static const float clear_color_with_alpha[4] = { m_vClear_Color.x, m_vClear_Color.y, m_vClear_Color.z, m_vClear_Color.w };

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (_float*)&m_vClear_Color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
}

CToolMainApp* CToolMainApp::Create()
{
    CToolMainApp* pInstance = New CToolMainApp;

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created CMainTool");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CToolMainApp::Free()
{
    for (auto& Model : m_vecModelDatas)
        Safe_Delete_BinaryData(Model);

    for (auto& Model : m_vecAnimModelData)
        Safe_Delete_BinaryData(Model);

    Safe_Release(m_pRenderer);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
    Safe_Release(m_pImWindow_Manager);

    Safe_Release(m_pGameInstance);

    CToolInstance::DestroyInstance();
    CImWindow_Manager::DestroyInstance();

    m_pGameInstance->Release_Engine();
}
