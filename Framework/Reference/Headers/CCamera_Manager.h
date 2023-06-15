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
	HRESULT Reserve_Containers(const _uint& iNumLevels);
	HRESULT Add_Camera(const _uint& iLevelIndex, const wstring& CameraTag, class CCamera* pCamera);
	HRESULT Remove_Camera(const _uint& iLevelIndex, const wstring& CameraTag);
	void Clear_LevelResources(const _uint& iLevelIndex);

public:
	HRESULT On_Camera(const _uint& iLevelIndex, const wstring& CameraTag);
	
private:
	typedef unordered_map<wstring, class CCamera*> CAMERAS;
	class CCamera* m_pCurCamera = { nullptr };
	CAMERAS* m_pCameras = { nullptr };
	_uint m_iNumLevels = { 0 };
	
private:
	class CCamera* Find_Camera(const _uint& iLevelIndex, const wstring& CameraTag);

public:
	virtual void Free() override;
};

END