#pragma once

#include "Engine_Defines.h"

class CToolView;
class CTerrain
{
private:
	CTerrain();
	virtual ~CTerrain() = default;

public:
	HRESULT Initialize();
	void Tick(_double TimeDelta);
	HRESULT Render();

public:
	void Set_mainView(CToolView* pMainView) { m_pMainView = pMainView; }
	void Set_Ratio(_float4x4* pOut, const _float fRatioX, const _float fRatioY);

private:
	CToolView* m_pMainView = { nullptr };
};