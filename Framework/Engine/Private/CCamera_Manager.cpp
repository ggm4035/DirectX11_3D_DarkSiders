#include "CCamera_Manager.h"

#include "CCamera.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
{
}

const _matrix* CCamera_Manager::Get_Current_CameraViewMatrix() const
{
	if (nullptr == m_pCurCamera)
		return nullptr;

	return m_pCurCamera->Get_CameraViewMatrix();
}

const _matrix* CCamera_Manager::Get_Current_CameraProjMatrix() const
{
	if (nullptr == m_pCurCamera)
		return nullptr;

	return m_pCurCamera->Get_CameraProjMatrix();
}

HRESULT CCamera_Manager::Reserve_Containers(_uint iNumLevels)
{
	if (0 == iNumLevels)
		return E_FAIL;

	if (nullptr != m_pCameras)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pCameras = new CAMERAS[iNumLevels];

	return S_OK;
}

HRESULT CCamera_Manager::Add_Camera(_uint iLevelIndex, const _tchar* pCameraTag, CCamera* pCamera)
{
	if (nullptr == pCamera || iLevelIndex >= m_iNumLevels || iLevelIndex < 0)
		return E_FAIL;

	if (nullptr != Find_Camera(iLevelIndex, pCameraTag))
		return E_FAIL;

	m_pCameras[iLevelIndex].emplace(pCameraTag, pCamera);
	Safe_AddRef(pCamera);

	return S_OK;
}

void CCamera_Manager::Tick(_double TimeDelta)
{
	if (m_pCurCamera)
		m_pCurCamera->Tick(TimeDelta);
}

HRESULT CCamera_Manager::Remove_Camera(_uint iLevelIndex, const _tchar* pCameraTag)
{
	if (nullptr == pCameraTag || iLevelIndex >= m_iNumLevels || iLevelIndex < 0)
		return E_FAIL;

	for (auto& iter = m_pCameras[iLevelIndex].begin(); iter != m_pCameras[iLevelIndex].end(); ++iter)
	{
		if (0 == lstrcmp(pCameraTag, iter->first))
		{
			Safe_Release(iter->second);
			m_pCameras[iLevelIndex].erase(iter);
			return S_OK;
		}
	}

	return E_FAIL;
}

void CCamera_Manager::Clear_LevelResources(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels || iLevelIndex < 0)
		return;

	for (auto& Camera : m_pCameras[iLevelIndex])
		Safe_Release(Camera.second);
	m_pCameras[iLevelIndex].clear();
}

HRESULT CCamera_Manager::On_Camera(_uint iLevelIndex, const _tchar* pCameraTag)
{
	if (nullptr == pCameraTag || iLevelIndex >= m_iNumLevels || iLevelIndex < 0)
		return E_FAIL;

	CCamera* pCamera = Find_Camera(iLevelIndex, pCameraTag);

	if (nullptr == pCamera)
		return E_FAIL;

	if (m_pCurCamera)
	{
		m_pCurCamera->OffCamera();
		Safe_Release(m_pCurCamera);
	}

	m_pCurCamera = pCamera;
	Safe_AddRef(m_pCurCamera);

	m_pCurCamera->OnCamera();

	return S_OK;
}

CCamera* CCamera_Manager::Find_Camera(_uint iLevelIndex, const _tchar* pCameraTag)
{
	auto& iter = find_if(m_pCameras[iLevelIndex].begin(), m_pCameras[iLevelIndex].end(), CTag_Finder(pCameraTag));

	if (iter == m_pCameras[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CCamera_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Camera : m_pCameras[i])
			Safe_Release(Camera.second);
		m_pCameras[i].clear();
	}
	Safe_Delete_Array(m_pCameras);

	Safe_Release(m_pCurCamera);
}
