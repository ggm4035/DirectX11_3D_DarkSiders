#pragma once

#include "IMGUI_Defines.h"
#include "CGameObject3D.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Coordnate;
END

BEGIN(Imgui)

class CCoordnate_Axis final : public CGameObject3D
{
private:
	explicit CCoordnate_Axis(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCoordnate_Axis(const CCoordnate_Axis& rhs);
	virtual ~CCoordnate_Axis() = default;

public:
    static void Set_PrototypeTag(wstring PrototypeTag) { m_PrototypeTag = PrototypeTag; }
    static wstring Get_PrototypeTag() { return m_PrototypeTag; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLayerIndex, CComponent* pOwner, void* pArg) override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual void Late_Tick(const _double& TimeDelta) override {}
	virtual HRESULT Render() override;

public:
	void RenderOn() { m_bIsRender = true; }
	void RenderOff() { m_bIsRender = false; }

private:
	CRenderer* m_pRenderer = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Coordnate* m_pBufferCom = { nullptr };

private:
	_bool m_bIsRender = { true };

private:
    static wstring m_PrototypeTag;

private:
	virtual HRESULT Add_Components() override;
	HRESULT Set_ShaderResources();

public:
	static CCoordnate_Axis* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCoordnate_Axis* Clone(const _uint& iLayerIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END
