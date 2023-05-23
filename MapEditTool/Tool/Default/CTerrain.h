#pragma once

#include "Tool_Defines.h"
#include "CGameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Terrain;
class CRenderer;
END

class CTerrain final : public CGameObject
{
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Terrain* m_pBufferCom = { nullptr };
	CRenderer* m_pRenderCom = { nullptr };

	_matrix WVPMatrix;

private:
	virtual HRESULT Add_Components() override;

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};