#include "CChannel.h"

#include "CBone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const CHANNELDATA& ChannelData, const CModel::BONES& Bones)
{
	strcpy_s(m_szName, ChannelData.szName);

	auto& iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pValue)
		{
			if (0 != strcmp(m_szName, pValue->Get_Name().c_str()))
			{
				++m_iBoneIndex;
				return false;
			}
			else
				return true;
		});

	/* Setting KeyFrame */
	m_iNumKeyFrames = ChannelData.iNumKeyFrames;

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
		m_vecKeyFrames.push_back(ChannelData.pKeyFrames[i]);


	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(const CModel::BONES& Bones, const _double& TimeAcc, _uint* pCurrentKeyFrame)
{
	if (0.0 == TimeAcc)
		*pCurrentKeyFrame = 0;

	KEYFRAME LastKeyFrame = m_vecKeyFrames.back();

	_vector vScale;
	_vector vRotation;
	_vector vTranslation;

	if (TimeAcc >= LastKeyFrame.Time)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMLoadFloat3(&LastKeyFrame.vTranslation);
		vTranslation.m128_f32[3] = 1.f;
	}

	else /* 현재 존재하는 키프레임의 상태를 좌우 키프레임정보를 이용하여 선형보간한다. */
	{
		while ((((*pCurrentKeyFrame) + 1) < (m_vecKeyFrames.size() - 1)) && 
			(TimeAcc >= m_vecKeyFrames[(*pCurrentKeyFrame) + 1].Time))
			++(*pCurrentKeyFrame);

		_double Ratio = (TimeAcc - m_vecKeyFrames[(*pCurrentKeyFrame)].Time) /
			(m_vecKeyFrames[(*pCurrentKeyFrame) + 1].Time - m_vecKeyFrames[(*pCurrentKeyFrame)].Time);

		_vector vSourScale = XMLoadFloat3(&m_vecKeyFrames[(*pCurrentKeyFrame)].vScale);
		_vector vSourRotation = XMLoadFloat4(&m_vecKeyFrames[(*pCurrentKeyFrame)].vRotation);
		_vector vSourTranslation = XMLoadFloat3(&m_vecKeyFrames[(*pCurrentKeyFrame)].vTranslation);
		vSourTranslation.m128_f32[3] = 1.f;

		_vector vDestScale = XMLoadFloat3(&m_vecKeyFrames[(*pCurrentKeyFrame) + 1].vScale);
		_vector vDestRotation = XMLoadFloat4(&m_vecKeyFrames[(*pCurrentKeyFrame) + 1].vRotation);
		_vector vDestTranslation = XMLoadFloat3(&m_vecKeyFrames[(*pCurrentKeyFrame) + 1].vTranslation);
		vDestTranslation.m128_f32[3] = 1.f;

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)Ratio);
	}

	/* 진행된 시간에 맞는 뼈의 행렬을 만들어낸다. */
	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	/* 같은 이름을 가진 모델이 들고 있는 뼈에게 전달해준다. */
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

	/* 안움직이는 상태행렬을 하나 더 전달해준다. */
	vTranslation = XMLoadFloat3(&m_vecKeyFrames[0].vTranslation);
	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_NotMoveTransformationMatrix(TransformationMatrix);
}

void CChannel::Lerp_TransformationMatrix(const CModel::BONES& Bones, const _double& LerpDuration, const _double& TimeAcc, 
	const _uint& CurrentKeyFrame, const KEYFRAME& KeyFrame)
{
	_vector vScale;
	_vector vRotation;
	_vector vTranslation;

	_double Ratio = TimeAcc / LerpDuration;

	_vector vSourScale = XMLoadFloat3(&m_vecKeyFrames[CurrentKeyFrame].vScale);
	_vector vSourRotation = XMLoadFloat4(&m_vecKeyFrames[CurrentKeyFrame].vRotation);
	_vector vSourTranslation = XMLoadFloat3(&m_vecKeyFrames[0].vTranslation);

	_vector vDestScale = XMLoadFloat3(&KeyFrame.vScale);
	_vector vDestRotation = XMLoadFloat4(&KeyFrame.vRotation);
	_vector vDestTranslation = XMLoadFloat3(&KeyFrame.vTranslation);

	vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
	vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
	vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)Ratio);

	/* 진행된 시간에 맞는 뼈의 행렬을 만들어낸다. */
	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	/* 같은 이름을 가진 모델이 들고 있는 뼈에게 전달해준다. */
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

	Bones[m_iBoneIndex]->Set_NotMoveTransformationMatrix(TransformationMatrix);
}

/* 이동값을 없앤 transformationMatrix 를 반환 */
void CChannel::Invalidate_NotMoveTransformationMatrix(const CModel::BONES& Bones, const _double& TimeAcc, _uint* pCurrentKeyFrame, const _uint& iBlockIndex)
{
	if (0.0 == TimeAcc)
		*pCurrentKeyFrame = 0;

	KEYFRAME LastKeyFrame = m_vecKeyFrames.back();

	_vector vScale;
	_vector vRotation;
	_vector vTranslation;

	if (TimeAcc >= LastKeyFrame.Time)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMLoadFloat3(&m_vecKeyFrames[iBlockIndex].vTranslation);
	}

	else /* 현재 존재하는 키프레임의 상태를 좌우 키프레임정보를 이용하여 선형보간한다. 
			하지만 이동값은 없앰 */
	{
		while (TimeAcc >= m_vecKeyFrames[(*pCurrentKeyFrame) + 1].Time)
			++(*pCurrentKeyFrame);

		_double Ratio = (TimeAcc - m_vecKeyFrames[(*pCurrentKeyFrame)].Time) /
			(m_vecKeyFrames[(*pCurrentKeyFrame) + 1].Time - m_vecKeyFrames[(*pCurrentKeyFrame)].Time);

		_vector vSourScale = XMLoadFloat3(&m_vecKeyFrames[(*pCurrentKeyFrame)].vScale);
		_vector vSourRotation = XMLoadFloat4(&m_vecKeyFrames[(*pCurrentKeyFrame)].vRotation);
		_vector vSourTranslation = XMLoadFloat3(&m_vecKeyFrames[iBlockIndex].vTranslation);

		_vector vDestScale = XMLoadFloat3(&m_vecKeyFrames[(*pCurrentKeyFrame) + 1].vScale);
		_vector vDestRotation = XMLoadFloat4(&m_vecKeyFrames[(*pCurrentKeyFrame) + 1].vRotation);

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vTranslation = vSourTranslation;
	}

	/* 진행된 시간에 맞는 뼈의 행렬을 만들어낸다. */
	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	/* 같은 이름을 가진 모델이 들고 있는 뼈에게 전달해준다. */
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
	/* 안움직이는 상태행렬을 하나 더 전달해준다. */
	vTranslation = XMLoadFloat3(&m_vecKeyFrames[0].vTranslation);
	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_NotMoveTransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const CHANNELDATA& ChannelData, const CModel::BONES& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(ChannelData, Bones)))
	{
		MSG_BOX("Failed to Create CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	m_vecKeyFrames.clear();
}

#if defined(_USE_IMGUI) || defined(_DEBUG)

#endif