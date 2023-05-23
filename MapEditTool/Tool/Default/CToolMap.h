#pragma once

#include "Tool_Defines.h"
#include "CLevel.h"

class CToolMap final : public CLevel
{
private:
	CToolMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CToolMap() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_double TimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Ready_Layer_ToolMap(const _tchar* pLayerTag);

public:
	static CToolMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

