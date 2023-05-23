#pragma once

#include "CComponent_Manager.h"

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
	HRESULT Initialize_Engine(_uint iNumLevels, const GRAPHICDESC & GraphicDesc, _Inout_ ID3D11Device * *ppDevice, _Inout_ ID3D11DeviceContext * *ppContext);
	void Tick_Engine(_double TimeDelta);
	void Clear_LevelResources(_uint iLevelIndex); // ���� ���ҽ��� �����.

public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For.Timer_Manager */
	_double	Get_Timer(const _tchar * pTimerTag);
	void	Set_Timer(const _tchar * pTimerTag);
	HRESULT	Ready_Timer(const _tchar * pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iNumLevels, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const _tchar * pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iNumLayer, const _tchar * pPrototypeTag, const _tchar * pLayerTag, void* pArg = nullptr);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void* pArg = nullptr);

public: /* For.Camera_Manager */
	const _matrix* Get_Current_CameraViewMatrix() const;
	const _matrix* Get_Current_CameraProjMatrix() const;
	HRESULT Add_Camera(_uint iLevelIndex, const _tchar * pCameraTag, class CCamera* pCamera);
	HRESULT Remove_Camera(_uint iLevelIndex, const _tchar * pCameraTag);
	HRESULT On_Camera(_uint iLevelIndex, const _tchar * pCameraTag);

private:
	class CGraphic_Device* m_pGraphic_Device = { nullptr };
	class CLevel_Manager* m_pLevel_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CTimer_Manager* m_pTimer_Manager = { nullptr };
	class CComponent_Manager* m_pComponent_Manager = { nullptr };
	class CCamera_Manager* m_pCamera_Manager = { nullptr };
	
public:
	static void Release_Engine();
	virtual void Free() override;
};

END