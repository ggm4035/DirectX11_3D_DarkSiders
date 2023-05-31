#pragma once

#include "CComponent_Manager.h"
#include "CPipeLine.h"
#include "CInput_Device.h"

/* 0. 클라이언트 엔진의 연결 역활한다. */
/* 0.0 게임인스턴스라는 객체를 통해서 엔진의다양한기능을 활요앟ㄹ 수 있게 한다. */
/* 0.1 진짜 필요한 함수만 선택하여 오픈해준다. */
/* 0.2 한꺼번에 초기화 및 삭제처리 가능.  */
/* 0.3 업데이트 순서관리에 용이하다.  */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	explicit CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(_uint iNumLevels, const GRAPHICDESC & GraphicDesc, _Inout_ ID3D11Device * *ppDevice, _Inout_ ID3D11DeviceContext * *ppContext);
	void Tick_Engine(_double TimeDelta);
	void Clear_LevelResources(_uint iLevelIndex); // 레벨 리소스를 지운다.

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
	class CComponent* Clone_Transform(void* pArg = nullptr);

public: /* For.Camera_Manager */
	HRESULT Add_Camera(_uint iLevelIndex, const _tchar * pCameraTag, class CCamera* pCamera);
	HRESULT Remove_Camera(_uint iLevelIndex, const _tchar * pCameraTag);
	HRESULT On_Camera(_uint iLevelIndex, const _tchar * pCameraTag);

public: /* For.Input_Manager */
	_bool	Key_Pressing(_ubyte ubyKey);
	_bool	Key_Down(_ubyte ubyKey);
	_bool	Key_Up(_ubyte ubyKey);
	_bool	Mouse_Down(CInput_Device::MOUSEKEYSTATE eMouseID);
	_bool	Mouse_Pressing(CInput_Device::MOUSEKEYSTATE eMouseID);
	_bool	Mouse_Up(CInput_Device::MOUSEKEYSTATE eMouseID);
	_long	Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID);

public: /* For.PipeLine */
	_matrix Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState);
	_matrix Get_Transform_Inverse_Matrix(CPipeLine::TRANSFORMSTATE eState);
	_float4x4 Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState);
	_float4x4 Get_Trasnform_Inverse_Float4x4(CPipeLine::TRANSFORMSTATE eState);
	_matrix Get_UI_View_Matrix();
	_float4x4 Get_UI_View_Float4x4();
	_matrix Get_UI_Proj_Matrix(const _uint iWinSizeX, const _uint iWinSizeY);
	_float4x4 Get_UI_Proj_Float4x4(const _uint iWinSizeX, const _uint iWinSizeY);
	HRESULT Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix _Matrix);

public: /* For. Imgui */
	void ResizeBuffers(_uint & g_ResizeWidth, _uint & g_ResizeHeight);

private:
	class CGraphic_Device* m_pGraphic_Device = { nullptr };
	class CLevel_Manager* m_pLevel_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CTimer_Manager* m_pTimer_Manager = { nullptr };
	class CComponent_Manager* m_pComponent_Manager = { nullptr };
	class CCamera_Manager* m_pCamera_Manager = { nullptr };
	class CInput_Device* m_pInput_Manager = { nullptr };
	class CPipeLine* m_pPipeLine = { nullptr };
	
public:
	static void Release_Engine();
	virtual void Free() override;
};

END