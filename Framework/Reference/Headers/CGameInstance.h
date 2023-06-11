#pragma once

#include "CComponent_Manager.h"
#include "CPipeLine.h"
#include "CInput_Device.h"
#include "CLight_Manager.h"

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
	HRESULT Initialize_Engine(const _uint& iNumLevels, const GRAPHICDESC & GraphicDesc, _Inout_ ID3D11Device * *ppDevice, _Inout_ ID3D11DeviceContext * *ppContext);
	void Tick_Engine(const _double& TimeDelta);
	void Clear_LevelResources(const _uint& iLevelIndex); // 레벨 리소스를 지운다.

public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(const _float4& vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For.Timer_Manager */
	_double	Get_Timer(wstring TimerTag);
	void	Set_Timer(wstring TimerTag);
	HRESULT	Ready_Timer(wstring TimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(const _uint& iNumLevels, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(wstring pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(const _uint& iNumLayer, wstring PrototypeTag, wstring GameObjectTag, wstring LayerTag, void* pArg = nullptr);

	/* For. MapEditTool */
	list<CGameObject*> Get_All_GameObject();
	HRESULT Remove_GameObject(const wstring GameObjectTag);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(const _uint& iLevelIndex, wstring PrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(const _uint& iLevelIndex, wstring PrototypeTag, class CComponent * pOwner, void* pArg = nullptr);
	class CComponent* Clone_Transform(class CComponent * pOwner, void* pArg = nullptr);

	/* For. MapEditTool */
	list<CComponent*> Get_All_Prototypes();

public: /* For.Camera_Manager */
	HRESULT Add_Camera(const _uint& iLevelIndex, wstring CameraTag, class CCamera* pCamera);
	HRESULT Remove_Camera(const _uint& iLevelIndex, wstring CameraTag);
	HRESULT On_Camera(const _uint& iLevelIndex, wstring CameraTag);

public: /* For.Input_Manager */
	_bool	Key_Pressing(const _ubyte& ubyKey);
	_bool	Key_Down(const _ubyte& ubyKey);
	_bool	Key_Up(const _ubyte& ubyKey);
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
	_matrix Get_UI_Proj_Matrix(const _uint& iWinSizeX, const _uint& iWinSizeY);
	_float4x4 Get_UI_Proj_Float4x4(const _uint& iWinSizeX, const _uint& iWinSizeY);
	HRESULT Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix _Matrix);
	_float4 Get_Camera_Position() const;

public: /* For.FileInfo */
	wstring strToWStr(string str);
	string wstrToStr(wstring wstr);
	HRESULT Extraction_Data(const string & strPath, const _char * pExt, OUT list<string>&FilePathList);
	void ReadNonAnimModels(const string & strFilePath, OUT list<string>&FilePathList, OUT vector<NONANIM_MODEL_BINARYDATA>&vecData);

public: /* For.Light_Manager*/
	const CLight::LIGHTDESC* Get_LightDesc(const _uint & iIndex);
	HRESULT Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const CLight::LIGHTDESC & LightDesc);

public: /* For.Calculator */
	_vector Picking_On_Triangle(HWND hWnd, class CVIBuffer* pBuffer, class CTransform* pTransform);

public: /* For. Imgui */
	void ResizeBuffers(_uint& g_ResizeWidth, _uint& g_ResizeHeight);

private:
	class CGraphic_Device* m_pGraphic_Device = { nullptr };
	class CLevel_Manager* m_pLevel_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CTimer_Manager* m_pTimer_Manager = { nullptr };
	class CComponent_Manager* m_pComponent_Manager = { nullptr };
	class CCamera_Manager* m_pCamera_Manager = { nullptr };
	class CInput_Device* m_pInput_Manager = { nullptr };
	class CPipeLine* m_pPipeLine = { nullptr };
	class CFileInfo* m_pFileInfo = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };
	class CCalculator* m_pCalculator = { nullptr };
	
public:
	static void Release_Engine();
	virtual void Free() override;
};

END