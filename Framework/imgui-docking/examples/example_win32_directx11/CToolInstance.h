#pragma once

#include "IMGUI_Defines.h"

BEGIN(Imgui)

struct CToolInstance final : public CBase
{
	DECLARE_SINGLETON(CToolInstance)

private:
	explicit CToolInstance();
	virtual ~CToolInstance() = default;

public:
    class CTerrain* m_pTerrain = { nullptr };
    class CCoordnate_Axis* m_pCoordnate = { nullptr };

public:
    D3D11_RASTERIZER_DESC m_RasterizerDesc;

private:
	virtual void Free() override {}
};

END

#define TOOL CToolInstance::GetInstance()
