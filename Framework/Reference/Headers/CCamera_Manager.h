#pragma once

#include "CBase.h"

BEGIN(Engine)

class CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	_matrix Get_Current_CameraViewMatrix();
	_matrix Get_Current_CameraProjMatrix();

public:
	HRESULT Reserve_Containers(_uint iNumLevels);
	HRESULT Add_Camera(_uint iLevelIndex, const _tchar* pCameraTag, class CCamera* pCamera);
	HRESULT Remove_Camera(_uint iLevelIndex, const _tchar* pCameraTag);
	void Clear_LevelResources(_uint iLevelIndex);

public:
	HRESULT On_Camera(_uint iLevelIndex, const _tchar* pCameraTag);
	
private:
	_uint m_iNumLevels = { 0 };
	class CCamera* m_pCurCamera = { nullptr };
	unordered_map<const _tchar*, class CCamera*>* m_pCameras = { nullptr };
	typedef unordered_map<const _tchar*, class CCamera*> CAMERAS;

private:
	class CCamera* Find_Camera(_uint iLevelIndex, const _tchar* pCameraTag);

public:
	virtual void Free() override;
};

END