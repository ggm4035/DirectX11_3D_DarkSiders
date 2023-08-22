#pragma once

#include "CComponent_Manager.h"
#include "CPipeLine.h"
#include "CInput_Device.h"
#include "CLight_Manager.h"
#include "CSound_Manager.h"

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

public:
	class CGameObject3D* Get_Player() {
		return m_pPlayer;
	}
	void Set_Player(class CGameObject3D* pPlayer) {
		m_pPlayer = pPlayer;
		Safe_AddRef(pPlayer);
	}
public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(const _float4& vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For.Timer_Manager */
	_double	Get_Timer(const wstring TimerTag);
	void	Set_Timer(const wstring TimerTag);
	HRESULT	Ready_Timer(const wstring TimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(const _uint& iNumLevels, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	/* 자세하게는 못찾아줌 보스 가져오는 용도로 제작 */
	class CGameObject* Find_GameObject(const _uint& iLevelIndex, const wstring& wstrLayerTag, const wstring& wstrGameObjectTag);
	HRESULT Add_Prototype(const wstring pPrototypeTag, class CGameObject* pPrototype);
	class CGameObject* Clone_GameObject(const _uint& iLevelIndex, const wstring& PrototypeTag, const wstring& GameObjectTag, class CComponent* pOwner, void* pArg);

	/* 게임오브젝트에서 iLevelIndex는 오브젝트의 레이어 레벨로 사용 */
	HRESULT Add_GameObject(const _uint& iNumLayer, const wstring PrototypeTag, const wstring GameObjectTag, const wstring LayerTag, void* pArg = nullptr);

	/* For. MapEditTool */
	list<CGameObject*> Get_GameObject_Layer(const wstring & wstrLayerTag);
	HRESULT Remove_GameObject(const wstring GameObjectTag);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(const _uint& iLevelIndex, const wstring& PrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(const _uint& iLevelIndex, const wstring& PrototypeTag, class CComponent * pOwner, void* pArg = nullptr);
	class CComponent* Clone_Transform(class CComponent * pOwner, void* pArg = nullptr);

	/* For. MapEditTool */
	list<CComponent*> Get_All_Prototypes();

public: /* For.Input_Manager */
	_bool	Key_Pressing(const _ubyte& ubyKey);
	_bool	Key_Down(const _ubyte& ubyKey);
	_bool	Key_Up(const _ubyte& ubyKey);
	_bool	Mouse_Down(CInput_Device::MOUSEKEYSTATE eMouseID);
	_bool	Mouse_Pressing(CInput_Device::MOUSEKEYSTATE eMouseID);
	_bool	Mouse_Up(CInput_Device::MOUSEKEYSTATE eMouseID); 
	_ubyte	Peek_Message();
	_ubyte	Pop_Message();
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
	wstring strToWStr(const string str);
	string wstrToStr(const wstring wstr);
	HRESULT Extraction_Data(const string & strPath, const _char * pExt, OUT list<string>&FilePathList);
	void WriteModels(const string & strFilePath, const list<string>&FilePathList, const vector<MODEL_BINARYDATA>&vecData);
	void ReadModel(const string & strFilePath, OUT string & FilePath, OUT MODEL_BINARYDATA & Data);
	HRESULT Load(const string & strFilePath, OUT FILEDATA& OutData);

public: /* For.Light_Manager*/
	const CLight::LIGHTDESC* Get_LightDesc(const _uint & iIndex);
	HRESULT Add_Light(const CLight::LIGHTDESC & LightDesc);
	_float4x4 Get_LightViewFloat4x4(const _uint& iIndex);
	_float4x4 Get_LightProjFloat4x4(ID3D11DeviceContext* pContext);

public: /* For.Calculator */
	_vector Picking_On_Triangle(const POINT & ptMouse, class CVIBuffer* pBuffer, class CTransform* pTransform);
	_vector Picking_On_Triangle(const POINT & ptMouse, class CModel* pModel, class CTransform* pTransform);
	_fvector Get_RandomVectorInSphere(const _float& fRadius);

public: /* For.Font_Manager */
	HRESULT Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & wstrFontTag, const wstring & wstrFontFilePath);
	HRESULT Render_Font(const wstring & wstrFontTag, const wstring & wstrText, const _float2 & vPosition, _fvector vColor = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fRotation = 0.f, const _float2 & vOrigin = _float2(), _float fScale = 1.f);

public: /* For.Frustum */
	void Transform_To_LocalSpace(_fmatrix WorldMatrix);
	_bool isIn_WorldSpace(_fvector vWorldPosition, _float fRange);
	_bool isIn_LocalSpace(_fvector vLocalPosition, _float fRange);

public: /* For.Collider_Manager */

#if defined(_USE_IMGUI) || defined(_DEBUG)
	_vector Picking_On_Spheres(const POINT & ptMouse, class CNavigation* pNavigation, class CTransform* pTransform);
	vector<pair<_uint, _int>> Pick_Spheres(const POINT & ptMouse, class CNavigation* pNavigation, class CTransform* pTransform);
#endif

public: /* For. Sound_Manager */
	HRESULT isPlaying(CSound_Manager::SOUNDCHANNEL eChannel, OUT _bool& Out);
	HRESULT Play_Sound(const _tchar* pSoundTag, CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume, _bool bForcePlay = false);
	HRESULT Play_BGM(const _tchar* pSoundTag, _float fVolume, CSound_Manager::SOUNDCHANNEL eChannel = CSound_Manager::SOUND_BGM);
	HRESULT Stop_Sound(CSound_Manager::SOUNDCHANNEL eChannel);
	HRESULT Pause_Sound(CSound_Manager::SOUNDCHANNEL eChannel);
	HRESULT Restart_Sound(CSound_Manager::SOUNDCHANNEL eChannel);
	HRESULT Stop_AllSound();
	HRESULT Set_ChannelVolume(CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume);

public: /* For. Imgui */
	void ResizeBuffers(_uint& g_ResizeWidth, _uint& g_ResizeHeight);

private:
	class CGameObject3D* m_pPlayer = { nullptr };

private:
	class CGraphic_Device* m_pGraphic_Device = { nullptr };
	class CLevel_Manager* m_pLevel_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CTimer_Manager* m_pTimer_Manager = { nullptr };
	class CComponent_Manager* m_pComponent_Manager = { nullptr };
	class CInput_Device* m_pInput_Manager = { nullptr };
	class CPipeLine* m_pPipeLine = { nullptr };
	class CFileInfo* m_pFileInfo = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };
	class CCalculator* m_pCalculator = { nullptr };
	class CFont_Manager* m_pFont_Manager = { nullptr };
	class CFrustum* m_pFrustum = { nullptr };
	class CCollider_Manager* m_pCollider_Manager = { nullptr };
	class CTarget_Manager* m_pTarget_Manager = { nullptr };
	class CSound_Manager* m_pSound_Manager = { nullptr };
	
public:
	static void Release_Engine();
	virtual void Free() override;
};

END