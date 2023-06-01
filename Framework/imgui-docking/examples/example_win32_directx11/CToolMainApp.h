#pragma once

#include "IMGUI_Defines.h"
#include "CGameInstance.h"
#include "CImWindow_Manager.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(Imgui)

class CToolMainApp final : public CBase
{
private:
    explicit CToolMainApp();
    virtual ~CToolMainApp() = default;

public:
    HRESULT Initialize();
    void Tick(const _double& TimeDelta);
    HRESULT Render();

public:
    HRESULT Open_Level(LEVELID eLevelIndex);

public:
    CRenderer* m_pRenderer = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };
    CImWindow_Manager* m_pImWindow_Manager = { nullptr };

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    _float4 m_vClear_Color = { _float4(0.84f, 0.9f, 0.96f, 1.f) };

private:
    HRESULT Ready_ImWindows();
    HRESULT Ready_Prototype_GameObject_For_Tool();
    HRESULT Ready_Prototype_Component_For_Tool();
    void Update_Demo();

public:
    static CToolMainApp* Create();
    virtual void Free() override;
};

END
