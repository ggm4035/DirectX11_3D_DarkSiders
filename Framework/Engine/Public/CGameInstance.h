#pragma once

#include "CBase.h"

/* 0. Ŭ���̾�Ʈ ������ ���� ��Ȱ�Ѵ�. */
/* 0.0 �����ν��Ͻ���� ��ü�� ���ؼ� �����Ǵپ��ѱ���� Ȱ��ۤ� �� �ְ� �Ѵ�. */
/* 0.1 ��¥ �ʿ��� �Լ��� �����Ͽ� �������ش�. */
/* 0.2 �Ѳ����� �ʱ�ȭ �� ����ó�� ����.  */
/* 0.3 ������Ʈ ���������� �����ϴ�.  */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_double TimeDelta);

public: /* For.Graphic_Device */
	/* ����۸� �����. */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);

	/* ���̹��� + ���ٽǹ��۸� �����. */
	HRESULT Clear_DepthStencil_View();

	/* �ĸ� ���۸� ������۷� ��ü�Ѵ�.(����۸� ȭ�鿡 ���� �����ش�.) */
	HRESULT Present();

private:
	class CGraphic_Device* m_pGraphic_Device;
	
public:
	static void Release_Engine();
	virtual void Free() override;
};

END