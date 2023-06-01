#pragma once

#include "Client_Defines.h"
#include "CLevel.h"

BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	explicit CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Prototype_Component_For_Logo();
	HRESULT Ready_Layer_BackGround(wstring pLayerTag);

public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END