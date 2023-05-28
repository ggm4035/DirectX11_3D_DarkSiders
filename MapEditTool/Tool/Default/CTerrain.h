#pragma once

#include "Tool_Defines.h"
#include "CGameObject3D.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Terrain;
class CRenderer;
class CTexture;
class CTransform;
END

class CTerrain final : public CGameObject3D
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

public:
	HRESULT Reset_Data(const _uint& iXCount, const _uint& iZCount, 
		const _float& fInterval, const _float& fDetail);
	HRESULT Reset_Rasterizer(const D3D11_RASTERIZER_DESC& RasterizerDesc);
	HRESULT Set_Position(const _float4& rPosition);
	HRESULT Set_Rotation(const _float3& rDegrees);

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Terrain* m_pBufferCom = { nullptr };
	CRenderer* m_pRenderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	_float m_fDetail = { 0.f };
	ID3D11RasterizerState* m_pRasterizer = { nullptr };

private:
	virtual HRESULT Add_Components() override;
	HRESULT Set_Shader_Resources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CTerrain* Clone(void* pArg) override;
	virtual void Free() override;
};