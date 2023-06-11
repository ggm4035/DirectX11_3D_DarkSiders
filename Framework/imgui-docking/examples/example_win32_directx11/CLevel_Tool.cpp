#include "CLevel_Tool.h"

#include "CGameInstance.h"

#include "CTerrain.h"
#include "CCoordnate_Axis.h"
#include "CMainCamera.h"

#include "CShader.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer(L"Layer_Tool")))
        return E_FAIL;

    return S_OK;
}

void CLevel_Tool::Tick(const _double& TimeDelta)
{
}

HRESULT CLevel_Tool::Render()
{
    return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer(wstring LayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, L"Prototype_GameObject_Coordnate_Axis",
        L"Coordnate_Axis", LayerTag)))
        return E_FAIL;

    CCamera::CAMERADESC CameraDesc;

    CameraDesc.vEye = _float4(20.f, 20.f, -20.f, 1.f);
    CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);

    CameraDesc.fFov = XMConvertToRadians(60.f);
    CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    CameraDesc.fNear = 0.3f;
    CameraDesc.fFar = 300.f;

    CameraDesc.TransformDesc.SpeedPerSec = 10.f;
    CameraDesc.TransformDesc.RotationPerSec = XMConvertToRadians(90.f);

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, L"Prototype_GameObject_Camera_Main",
        L"Camera_Main", LayerTag, &CameraDesc)))
        return E_FAIL;

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Tool::Ready_Lights()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CLight::LIGHTDESC LightDesc;

    LightDesc.vPosition = _float4(500.f, 500.f, -500.f, 1.f);
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.fRange = 100.f;
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
    LightDesc.eType = CLight::TYPE_DIRECTIONAL;

    if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
        return E_FAIL;

    Safe_Release(pGameInstance);
    return S_OK;
}

CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Create CLevel_Tool");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Tool::Free()
{
    CLevel::Free();
}
