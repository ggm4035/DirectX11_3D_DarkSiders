#pragma once

#include "IMGUI_Defines.h"
#include "CLevel.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Imgui)

class CLevel_Tool final : public CLevel
{
private:
    explicit CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLevel_Tool() = default;

public:
    virtual HRESULT Initialize() override;
    virtual void Tick(_double TimeDelta) override;
    virtual HRESULT Render() override;

private:
    HRESULT Ready_Layer_Environments(const _tchar* pLayerTag);
    HRESULT Ready_Layer_Cameras(const _tchar* pLayerTag);
    HRESULT Ready_Layer_Mesh(const _tchar* pLayerTag);

public:
    static CLevel_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

END
