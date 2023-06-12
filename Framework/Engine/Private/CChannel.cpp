#include "CChannel.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const CHANNELDATA& ChannelData)
{
	strcpy_s(m_szName, ChannelData.szName);

	if (FAILED(Ready_KeyFrames(ChannelData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChannel::Ready_KeyFrames(const CHANNELDATA& ChannelData)
{
	m_iNumKeyFrames = ChannelData.iNumKeyFrames;

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
		m_vecKeyFrames.push_back(ChannelData.pKeyFrames[i]);

	return S_OK;
}

CChannel* CChannel::Create(const CHANNELDATA& ChannelData)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(ChannelData)))
	{
		MSG_BOX("Failed to Create CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
}
