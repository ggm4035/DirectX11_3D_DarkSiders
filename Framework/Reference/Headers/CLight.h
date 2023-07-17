#pragma once

#include "CBase.h"

BEGIN(Engine)

class CLight final : public CBase
{
public:
	enum TYPE{ TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };
	typedef struct tagLightDesc
	{
		TYPE eType;
		_float4 vPosition;
		_float4 vDirection;
		_float fRange;

		_float4 vDiffuse;
		_float4 vSpecular;
		_float4 vAmbient;
	}LIGHTDESC;

private:
	explicit CLight() = default;
	virtual ~CLight() = default;

public:
	const LIGHTDESC* Get_LightDesc() const { return &m_LightDesc; }

public:
	HRESULT Initialize(const LIGHTDESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	LIGHTDESC m_LightDesc;

public:
	static CLight* Create(const LIGHTDESC& LightDesc);
	virtual void Free() override;
};

END