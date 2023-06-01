#pragma once

#include "CLight.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)

private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const CLight::LIGHTDESC* Get_LightDesc(const _uint& iIndex);

public:
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CLight::LIGHTDESC& LightDesc);

private:
	vector<CLight*> m_vecLights;

public:
	virtual void Free() override;
};

END