#pragma once

#include "CLight.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)

private:
	CLight_Manager() = default;
	virtual ~CLight_Manager() = default;

public:
	const CLight::LIGHTDESC* Get_LightDesc(const _uint& iIndex);
	_float4x4 Get_LightViewFloat4x4(const _uint& iIndex);
	_float4x4 Get_LightProjFloat4x4(ID3D11DeviceContext* pContext);

public:
	HRESULT Add_Light(const CLight::LIGHTDESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* m_pVIBuffer);

private:
	vector<CLight*> m_vecLights;

public:
	virtual void Free() override;
};

END