#pragma once

#include "CBase.h"

BEGIN(Engine)

class CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)

private:
	explicit CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT Reserve_Containers(_uint iNumLevels);
	HRESULT Add_Camera(_uint iLevelIndex, const _tchar* pCameraTag, class CCamera* pCamera);
	HRESULT Remove_Camera(_uint iLevelIndex, const _tchar* pCameraTag);
	void Clear_LevelResources(_uint iLevelIndex);

public:
	HRESULT On_Camera(_uint iLevelIndex, const _tchar* pCameraTag);
	
private:
	typedef unordered_map<const _tchar*, class CCamera*> CAMERAS;
	class CCamera* m_pCurCamera = { nullptr };
	CAMERAS* m_pCameras = { nullptr };
	_uint m_iNumLevels = { 0 };
	
private:
	class CCamera* Find_Camera(_uint iLevelIndex, const _tchar* pCameraTag);

public:
	virtual void Free() override;
};

END