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
    virtual void Tick(const _double& TimeDelta) override;
    virtual HRESULT Render() override;

private:
    HRESULT Ready_Layer(wstring LayerTag);

public:
    static CLevel_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

END
