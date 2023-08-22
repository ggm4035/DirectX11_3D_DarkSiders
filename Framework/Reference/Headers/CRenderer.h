#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_SHADOW, RENDER_UI, RENDER_END };
	enum POSTSHADERPASS { PASS_POSTPROCESSING, PASS_BLUR, PASS_ZOOMBLUR, PASS_END };

private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	void Set_Pass(POSTSHADERPASS ePass) {
		m_ePass = ePass;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	
public:
	void Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_RenderGroup();

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Light();
	HRESULT Render_Shadow();
	HRESULT Render_Deferred();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_Blur();
	HRESULT Render_PostProcessing();
	HRESULT Render_UI();

private:
	class CTarget_Manager* m_pTarget_Manager = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };

	class CShader* m_pShader = { nullptr };
	class CShader* m_pPostProcessingShader = { nullptr };
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };

	_float4x4 m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	POSTSHADERPASS m_ePass = { PASS_POSTPROCESSING };
	list<class CGameObject*> m_RenderObjects[RENDER_END];

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;

#ifdef _DEBUG
public:
	HRESULT Add_DebugGroup(CComponent* pDebugCom);
	HRESULT Render_Debug();

private:
	list<class CComponent*>	m_DebugObject;
#endif
};

END