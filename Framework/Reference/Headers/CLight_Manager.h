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

public:
	HRESULT Add_Light(const CLight::LIGHTDESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* m_pVIBuffer);

private:
	vector<CLight*> m_vecLights;

public:
	virtual void Free() override;
};

END