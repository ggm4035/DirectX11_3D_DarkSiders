#include "CGameInstance.h"
#include "CApplication.h"
#include "CGraphic_Device.h"
#include "CLevel_Manager.h"
#include "CObject_Manager.h"
#include "Timer_Manager.h"
#include "CComponent_Manager.h"
#include "CFileInfo.h"
#include "CCalculator.h"
#include "CFont_Manager.h"
#include "CFrustum.h"
#include "CCollider_Manager.h"
#include "CTarget_Manager.h"
#include "CSound_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device { CGraphic_Device::GetInstance() }
	, m_pLevel_Manager{ CLevel_Manager::GetInstance() }
	, m_pObject_Manager{ CObject_Manager::GetInstance() }
	, m_pTimer_Manager{ CTimer_Manager::GetInstance() }
	, m_pComponent_Manager{ CComponent_Manager::GetInstance() }
	, m_pInput_Manager{ CInput_Device::GetInstance() }
	, m_pPipeLine{ CPipeLine::GetInstance() }
	, m_pFileInfo{ CFileInfo::GetInstance() }
	, m_pLight_Manager{ CLight_Manager::GetInstance() }
	, m_pCalculator{ CCalculator::GetInstance() }
	, m_pFont_Manager{ CFont_Manager::GetInstance() }
	, m_pFrustum{ CFrustum::GetInstance() }
	, m_pCollider_Manager{ CCollider_Manager::GetInstance() }
	, m_pTarget_Manager{ CTarget_Manager::GetInstance() }
	, m_pSound_Manager{ CSound_Manager::GetInstance() }
{
	Safe_AddRef(m_pCalculator);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pFileInfo);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pInput_Manager);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pCollider_Manager);
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pSound_Manager);
}

HRESULT CGameInstance::Initialize_Engine(const _uint& iNumLevels, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	/* 디바이스 초기화. (그래픽, 사운드, 입력장치) */

	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(
		GraphicDesc.hWnd,
		GraphicDesc.eWinMode,
		GraphicDesc.iViewportSizeX,
		GraphicDesc.iViewportSizeY,
		ppDevice, ppContext)))
		return E_FAIL;

	CApplication::m_iWinSizeX = GraphicDesc.iViewportSizeX;
	CApplication::m_iWinSizeY = GraphicDesc.iViewportSizeY;

	if (FAILED(m_pInput_Manager->Ready_DInput(GraphicDesc.hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	if (FAILED(m_pCalculator->Initialize(*ppDevice, *ppContext)))
		return E_FAIL;

	/* 레벨매니져 오브젝트 매니져, 컴포넌트 매니져들은 Reserve 한다.  */
	// 게임 오브젝트의 레벨 개수만큼 동적 배열 생성
	if (FAILED(m_pObject_Manager->Reserve_Containers(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Containers(iNumLevels, *ppDevice, *ppContext)))
		return E_FAIL;

	if (FAILED(m_pSound_Manager->Initialize()))
		return E_FAIL;

	if (FAILED(m_pSound_Manager->Add_Sounds(L"../../Resources/Sounds/")))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(const _double& TimeDelta)
{
	if (nullptr == m_pLevel_Manager)
		return;

	m_pInput_Manager->Update_DInput();

	m_pObject_Manager->Tick(TimeDelta);

	m_pPipeLine->Tick();

	m_pFrustum->Tick();

	m_pObject_Manager->AfterFrustumTick(TimeDelta);

	m_pCollider_Manager->Tick();

	m_pObject_Manager->Late_Tick(TimeDelta);

	m_pCollider_Manager->Clear_ColliderList();

	m_pLevel_Manager->Tick(TimeDelta);
}

void CGameInstance::Clear_LevelResources(const _uint& iLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return;

	// 삭제할 레벨의 컴포넌트 삭제.
	m_pComponent_Manager->Clear_LevelResources(iLevelIndex);

	// 삭제할 레벨의 게임 오브젝트의 리소스 삭제.
	m_pObject_Manager->Clear_LevelResources(iLevelIndex);
}

HRESULT CGameInstance::Clear_BackBuffer_View(const _float4& vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);	
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

_double CGameInstance::Get_Timer(const wstring TimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0;

	return m_pTimer_Manager->Get_Timer(TimerTag);
}

void CGameInstance::Set_Timer(const wstring TimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;
	
	m_pTimer_Manager->Set_Timer(TimerTag);
}

HRESULT CGameInstance::Ready_Timer(const wstring TimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Ready_Timer(TimerTag);
}

HRESULT CGameInstance::Open_Level(const _uint& iNumLevels, CLevel* pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iNumLevels, pNewLevel);
}

HRESULT CGameInstance::Add_Prototype(const wstring PrototypeTag, CGameObject* pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(PrototypeTag, pPrototype);
}

CGameObject* CGameInstance::Clone_GameObject(const _uint& iLevelIndex, const wstring& PrototypeTag, const wstring& GameObjectTag, CComponent* pOwner, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(iLevelIndex, PrototypeTag, GameObjectTag, pOwner, pArg);
}

HRESULT CGameInstance::Add_GameObject(const _uint& iNumLayer, const wstring PrototypeTag, const wstring GameObjectTag, const wstring LayerTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(iNumLayer, PrototypeTag, GameObjectTag, LayerTag, pArg);
}

list<CGameObject*> CGameInstance::Get_GameObject_Layer(const wstring& wstrLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return list<CGameObject*>();

	return m_pObject_Manager->Get_GameObject_Layer(wstrLayerTag);
}

HRESULT CGameInstance::Remove_GameObject(const wstring GameObjectTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Remove_GameObject(GameObjectTag);
}

HRESULT CGameInstance::Add_Prototype(const _uint& iLevelIndex, const wstring& PrototypeTag, CComponent* pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, PrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(const _uint& iLevelIndex, const wstring& PrototypeTag, CComponent* pOwner, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, PrototypeTag, pOwner, pArg);
}

CComponent* CGameInstance::Clone_Transform(CComponent* pOwner, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Transform(pOwner, pArg);
}

list<CComponent*> CGameInstance::Get_All_Prototypes()
{
	if(nullptr == m_pComponent_Manager)
		return list<CComponent*>();

	return m_pComponent_Manager->Get_All_Prototypes();
}

_bool CGameInstance::Key_Pressing(const _ubyte& ubyKey)
{
	if (nullptr == m_pInput_Manager)
		return false;

	return m_pInput_Manager->Key_Pressing(ubyKey);
}

_bool CGameInstance::Key_Down(const _ubyte& ubyKey)
{
	if (nullptr == m_pInput_Manager)
		return false;

	return m_pInput_Manager->Key_Down(ubyKey);
}

_bool CGameInstance::Key_Up(const _ubyte& ubyKey)
{
	if (nullptr == m_pInput_Manager)
		return false;

	return m_pInput_Manager->Key_Up(ubyKey);
}

_bool CGameInstance::Mouse_Down(CInput_Device::MOUSEKEYSTATE eMouseID)
{
	if (nullptr == m_pInput_Manager ||
		eMouseID < 0 || eMouseID >= CInput_Device::DIM_END)
		return false;

	return m_pInput_Manager->Mouse_Down(eMouseID);
}

_bool CGameInstance::Mouse_Pressing(CInput_Device::MOUSEKEYSTATE eMouseID)
{
	if (nullptr == m_pInput_Manager ||
		eMouseID < 0 || eMouseID >= CInput_Device::DIM_END)
		return false;

	return m_pInput_Manager->Mouse_Pressing(eMouseID);
}

_bool CGameInstance::Mouse_Up(CInput_Device::MOUSEKEYSTATE eMouseID)
{
	if (nullptr == m_pInput_Manager ||
		eMouseID < 0 || eMouseID >= CInput_Device::DIM_END)
		return false;

	return m_pInput_Manager->Mouse_Up(eMouseID);
}

_ubyte CGameInstance::Peek_Message()
{
	if (nullptr == m_pInput_Manager)
		return 0xCF;

	return m_pInput_Manager->Peek_Message();
}

_ubyte CGameInstance::Pop_Message()
{
	if (nullptr == m_pInput_Manager)
		return 0xCF;

	return m_pInput_Manager->Pop_Message();
}

_long CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID)
{
	if (nullptr == m_pInput_Manager ||
		eMouseMoveID < 0 || eMouseMoveID >= CInput_Device::DIMS_END)
		return false;

	return m_pInput_Manager->Get_DIMouseMove(eMouseMoveID);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _matrix();

	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_matrix CGameInstance::Get_Transform_Inverse_Matrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _matrix();

	return m_pPipeLine->Get_Transform_Inverse_Matrix(eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_float4x4 CGameInstance::Get_Trasnform_Inverse_Float4x4(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Trasnform_Inverse_Float4x4(eState);
}

_matrix CGameInstance::Get_UI_View_Matrix()
{
	if (nullptr == m_pPipeLine)
		return _matrix();

	return m_pPipeLine->Get_UI_View_Matrix();
}

_float4x4 CGameInstance::Get_UI_View_Float4x4()
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_UI_View_Float4x4();
}

_matrix CGameInstance::Get_UI_Proj_Matrix(const _uint& iWinSizeX, const _uint& iWinSizeY)
{
	if (nullptr == m_pPipeLine)
		return _matrix();

	return m_pPipeLine->Get_UI_Proj_Matrix(iWinSizeX, iWinSizeY);
}

_float4x4 CGameInstance::Get_UI_Proj_Float4x4(const _uint& iWinSizeX, const _uint& iWinSizeY)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_UI_Proj_Float4x4(iWinSizeX, iWinSizeY);
}

HRESULT CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix _Matrix)
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	return m_pPipeLine->Set_Transform(eState, _Matrix);
}

_float4 CGameInstance::Get_Camera_Position() const
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_Camera_Position();
}

wstring CGameInstance::strToWStr(const string str)
{
	if (nullptr == m_pFileInfo)
		return L"";

	return m_pFileInfo->strToWStr(str);
}

string CGameInstance::wstrToStr(const wstring wstr)
{
	if (nullptr == m_pFileInfo)
		return "";

	return m_pFileInfo->wstrToStr(wstr);
}

HRESULT CGameInstance::Extraction_Data(const string& strPath, const _char* pExt, OUT list<string>& FilePathList)
{
	if (nullptr == m_pFileInfo)
		return E_FAIL;

	return m_pFileInfo->Extraction_Data(strPath, pExt, FilePathList);
}

void CGameInstance::WriteModels(const string& strFilePath, const list<string>& FilePathList, const vector<MODEL_BINARYDATA>& vecData)
{
	if (nullptr == m_pFileInfo)
		return;

	return m_pFileInfo->WriteModels(strFilePath, FilePathList, vecData);
}

void CGameInstance::ReadModel(const string& strFilePath, OUT string& FilePath, OUT MODEL_BINARYDATA& Data)
{
	if (nullptr == m_pFileInfo)
		return;

	return m_pFileInfo->ReadModel(strFilePath, FilePath, Data);
}

HRESULT CGameInstance::Load(const string& strFilePath, OUT FILEDATA& OutData)
{
	if (nullptr == m_pFileInfo)
		return E_FAIL;

	return m_pFileInfo->Load(strFilePath, OutData);
}

const CLight::LIGHTDESC* CGameInstance::Get_LightDesc(const _uint& iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(const CLight::LIGHTDESC& LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(LightDesc);
}

_vector CGameInstance::Picking_On_Triangle(const POINT& ptMouse, class CVIBuffer* pBuffer, class CTransform* pTransform)
{
	if (nullptr == m_pCalculator)
		return _vector();

	return m_pCalculator->Picking_On_Triangle(ptMouse, pBuffer, pTransform);
}

_vector CGameInstance::Picking_On_Triangle(const POINT& ptMouse, CModel* pModel, CTransform* pTransform)
{
	if (nullptr == m_pCalculator)
		return _vector();

	return m_pCalculator->Picking_On_Triangle(ptMouse, pModel, pTransform);
}

HRESULT CGameInstance::Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& wstrFontTag, const wstring& wstrFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Font(pDevice, pContext, wstrFontTag, wstrFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& wstrFontTag, const wstring& wstrText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, _float fScale)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font(wstrFontTag, wstrText, vPosition, vColor, fRotation, vOrigin, fScale);
}

void CGameInstance::Transform_To_LocalSpace(_fmatrix WorldMatrix)
{
	if (nullptr == m_pFrustum)
		return;

	m_pFrustum->Transform_To_LocalSpace(WorldMatrix);
}

_bool CGameInstance::isIn_WorldSpace(_fvector vWorldPosition, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isIn_WorldSpace(vWorldPosition, fRange);
}

_bool CGameInstance::isIn_LocalSpace(_fvector vLocalPosition, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isIn_LocalSpace(vLocalPosition, fRange);
}

#if defined(_USE_IMGUI) || defined(_DEBUG)
_vector CGameInstance::Picking_On_Spheres(const POINT& ptMouse, CNavigation* pNavigation, CTransform* pTransform)
{
	if (nullptr == m_pCalculator)
		return _vector();

	return m_pCalculator->Picking_On_Spheres(ptMouse, pNavigation, pTransform);
}

vector<pair<_uint, _int>> CGameInstance::Pick_Spheres(const POINT& ptMouse, CNavigation* pNavigation, CTransform* pTransform)
{
	if (nullptr == m_pCalculator)
		return vector<pair<_uint, _int>>();

	return m_pCalculator->Pick_Spheres(ptMouse, pNavigation, pTransform);
}

HRESULT CGameInstance::Play_Sound(const _tchar* pSoundTag, CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume, _bool bForcePlay)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Play_Sound(pSoundTag, eChannel, fVolume, bForcePlay);
}

HRESULT CGameInstance::Play_BGM(const _tchar* pSoundTag, _float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Play_BGM(pSoundTag, fVolume);
}

HRESULT CGameInstance::Stop_Sound(CSound_Manager::SOUNDCHANNEL eChannel)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Stop_Sound(eChannel);
}

HRESULT CGameInstance::Pause_Sound(CSound_Manager::SOUNDCHANNEL eChannel)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Pause_Sound(eChannel);
}

HRESULT CGameInstance::Restart_Sound(CSound_Manager::SOUNDCHANNEL eChannel)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Restart_Sound(eChannel);
}

HRESULT CGameInstance::Stop_AllSound()
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Stop_AllSound();
}

HRESULT CGameInstance::Set_ChannelVolume(CSound_Manager::SOUNDCHANNEL eChannel, _float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->Set_ChannelVolume(eChannel, fVolume);
}

#endif

void CGameInstance::ResizeBuffers(_uint& g_ResizeWidth, _uint& g_ResizeHeight)
{
	if (nullptr == m_pGraphic_Device)
		return;

	m_pGraphic_Device->ResizeBuffers(g_ResizeWidth, g_ResizeHeight);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();

	CFont_Manager::GetInstance()->DestroyInstance();

	CLight_Manager::GetInstance()->DestroyInstance();

	CCalculator::GetInstance()->DestroyInstance();

	CFileInfo::GetInstance()->DestroyInstance();

	CPipeLine::GetInstance()->DestroyInstance();

	CCollider_Manager::GetInstance()->DestroyInstance();

	CObject_Manager::GetInstance()->DestroyInstance();

	CComponent_Manager::GetInstance()->DestroyInstance();

	CLevel_Manager::GetInstance()->DestroyInstance();

	CTimer_Manager::GetInstance()->DestroyInstance();

	CFrustum::GetInstance()->DestroyInstance();

	CTarget_Manager::GetInstance()->DestroyInstance();

	CInput_Device::GetInstance()->DestroyInstance();

	CSound_Manager::GetInstance()->DestroyInstance();

	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pPlayer);

	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pCalculator);
	Safe_Release(m_pFileInfo);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pInput_Manager);
	Safe_Release(m_pSound_Manager);

	Safe_Release(m_pGraphic_Device);
}
