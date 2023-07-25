#include "CModel.h"
#include "CMesh.h"
#include "CBone.h"
#include "CAnimation.h"
#include "CChannel.h"

#include "CGameInstance.h"
#include "CFileInfo.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_vecMeshes(rhs.m_vecMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_vecMaterials(rhs.m_vecMaterials)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_eType(rhs.m_eType)
	, m_isCloned(true)
	, m_iRootBoneIndex(rhs.m_iRootBoneIndex)
{
	for (auto& pOriginBone : rhs.m_vecBones)
		m_vecBones.push_back(pOriginBone->Clone());

	for (auto& pMesh : m_vecMeshes)
		Safe_AddRef(pMesh);

	for (auto& Meterial : m_vecMaterials)
	{
		for (auto& Texture : Meterial.pMtrlTexture)
			Safe_AddRef(Texture);
	}

	for (auto& pOriginAnimation : rhs.m_vecAnimations)
		m_vecAnimations.push_back(pOriginAnimation->Clone());
}

_bool CModel::isLoopAnimation() const
{
	return m_pCurrentAnimation->isLoop();
}

_bool CModel::isFinishedAnimation() const
{
	return m_pCurrentAnimation->m_isFinished;
}

_bool CModel::isFollowAnimation() const
{
	return m_pCurrentAnimation->m_isFollowAnimation;
}

_bool CModel::isAbleChangeAnimation() const
{
	return m_pCurrentAnimation->m_isAbleChange;
}

const CBone* CModel::Get_Bone(const _char* pBoneName)
{
	auto iter = find_if(m_vecBones.begin(), m_vecBones.end(), [&](CBone* pBone)
		{
			if (pBone->Get_Name() == pBoneName)
				return true;
			else
				return false;
		});

	if (iter == m_vecBones.end())
		return nullptr;

	return (*iter);
}

void CModel::Change_Animation(const string& strTag)
{
	if (string::npos == m_pCurrentAnimation->m_strName.find("Impact") &&
		string::npos != m_pCurrentAnimation->m_strName.find(strTag))
		return;

	CAnimation* pAnimation = Find_Animation(strTag);

	if (nullptr == pAnimation)
		return;

	if (false == m_pCurrentAnimation->isLerped())
		m_pCurrentAnimation->Reset_Animation();

	pAnimation->Bind_LerpAnimation(m_pCurrentAnimation);

	m_pCurrentAnimation = pAnimation;

	m_pCurrentAnimation->m_isAbleChange = false;

	m_isFirst = true;
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const MODEL_BINARYDATA& ModelData, _fmatrix PivotMatrix)
{
	m_eType = eModelType;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Ready_Bones(ModelData, nullptr)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(ModelData, eModelType)))
		return E_FAIL;

	if (FAILED(Ready_Materials(ModelData)))
		return E_FAIL;

	if (FAILED(Ready_Animations(ModelData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	if (FAILED(CComponent::Initialize(iLevelIndex, pOwner, pArg)))
		return E_FAIL;

	for (auto& iter = m_vecAnimations.begin(); iter != m_vecAnimations.end(); ++iter)
	{
		if (string::npos != (*iter)->m_strName.find("Idle"))
		{
			m_pCurrentAnimation = *iter;
			return S_OK;
		}
	}

	return S_OK;
}

HRESULT CModel::Render(const _uint& iMeshIndex)
{
	if (iMeshIndex < 0 || iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_vecMeshes[iMeshIndex]->Render();
}

_vector CModel::ComputeAnimMovement(OUT _float3* pDirection)
{
	CTransform* pTransform = dynamic_cast<CGameObject3D*>(m_pOwner)->Get_Transform();

	_vector vPosition = pTransform->Get_State(CTransform::STATE_POSITION);
	_matrix WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	if (false == m_pCurrentAnimation->isFollowAnimation())
		return vPosition;

	if (true == m_isFirst)
	{
		m_isFirst = false;
		m_WorldMatrix = pTransform->Get_WorldFloat4x4();
		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
		return WorldMatrix.r[3];
	}

	/* 오프셋 * 컴바인 * 피벗으로 로컬스페이스 상의 뼈의 상태행렬을 가져온다. */
	_matrix BoneMatrix = m_vecBones[m_iRootBoneIndex]->Get_OffsetMatrix() * m_vecBones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_PivotMatrix);

	for (_uint i = 0; i < 3; ++i)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	BoneMatrix *= WorldMatrix; /* 로컬상의 뼈행렬을 월드로 변환 */

	if (nullptr != pDirection)
		XMStoreFloat3(pDirection, XMVector3Normalize(BoneMatrix.r[3] - vPosition));

	vPosition = BoneMatrix.r[3];

	//cout << "X: " << vPosition.m128_f32[0] << " Y: " << vPosition.m128_f32[1] << " Z: " << vPosition.m128_f32[2] << endl;
	return vPosition;
}

void CModel::Play_Animation(const _double& TimeDelta, class CNavigation* pNavigation)
{
	/* 어떤 애니메이션을 재생하려고하는지?! */
	/* 이 애니메이션은 어떤 뼈를 사용하는지?! */
	/* 뼈들은 각각 어떤 상태(TransformationMatrix)를 취하고 있어야하는가?! */

	/* 현재 애니메이션에서 사용하는 뼈들을 찾아서 해당 뼈들의 TransformationMatrix를 갱신한다. */

	/* TransformationMatrix 를 선형보간한다. */

	/* 여기가 실질적으로 뼈가 이동하는 부분 */
	m_pCurrentAnimation->Invalidate_TransformationMatrix(m_vecBones, TimeDelta);


	/* 선형보간한 TransformationMatrix들을 CombinedTransformation 에 적용시킨다. */
	for (auto& pBone : m_vecBones)
		pBone->Invalidate_CombinedTransformationMatrix(m_vecBones);
}

void CModel::Pause_Animation()
{
	m_pCurrentAnimation->Pause_Animation();
}

void CModel::RePlay_Animation()
{
	m_pCurrentAnimation->Play_Animation();
}

//HRESULT CModel::Setup_Notifys(const wstring& wstrFilePath)
//{
//	vector<ANIMATIONDATA> vecAnimation;
//
//	CFileInfo::GetInstance()->Read_Notify_Data(wstrFilePath, vecAnimation);
//
//	CGameObject3D* pOwner = dynamic_cast<CGameObject3D*>(m_pOwner);
//	if (nullptr == m_pOwner)
//		return E_FAIL;
//
//	_uint iAnimIndex = { 0 };
//	for (auto& pAnimation : m_vecAnimations)
//	{
//		if (FAILED(pAnimation->Bind_Notifys(pOwner, vecAnimation[iAnimIndex++].vecNotifyDesc)))
//			return E_FAIL;
//	}
//
//	return S_OK;
//}

HRESULT CModel::Bind_Material(CShader* pShader, const string& strTypename, const _uint& iMeshIndex, TEXTURETYPE eTextureType)
{
	if (nullptr == pShader ||
		iMeshIndex < 0 || iMeshIndex >= m_iNumMeshes ||
		eTextureType < 0 || eTextureType >= TEXTURE_TYPE_MAX)
		return E_FAIL;

	_uint iMaterialIndex = m_vecMeshes[iMeshIndex]->Get_MaterialIndex();

	if (nullptr == m_vecMaterials[iMaterialIndex].pMtrlTexture[eTextureType])
		return S_OK;

	return m_vecMaterials[iMaterialIndex].pMtrlTexture[eTextureType]->Bind_ShaderResource(pShader, strTypename);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const string& strTypename, const _uint& iMeshIndex)
{
	if (nullptr == pShader ||
		iMeshIndex < 0 || iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_float4x4 BoneMatrices[256];
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * 256);

	m_vecMeshes[iMeshIndex]->Get_Matrices(m_vecBones, BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	return pShader->Bind_Float4x4_Array(strTypename, BoneMatrices, 256);
}

HRESULT CModel::Ready_Meshes(const MODEL_BINARYDATA& ModelData, TYPE eModelType)
{
	m_iNumMeshes = ModelData.iNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eModelType, ModelData.pMeshDatas[i]);
		if (nullptr == pMesh)
			return E_FAIL;

		m_vecMeshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const MODEL_BINARYDATA& ModelData)
{
	m_iNumMaterials = ModelData.iNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MESHMATERIAL MeshMaterialDesc;
		ZeroMemory(&MeshMaterialDesc, sizeof MeshMaterialDesc);

		for (_uint j = 0; j < TEXTURE_TYPE_MAX; ++j)
		{
			if (0 == lstrlen(ModelData.pMaterialPaths[i].szMaterialTexturePath[j]))
				continue;

			MeshMaterialDesc.pMtrlTexture[j] = CTexture::Create(m_pDevice, m_pContext, ModelData.pMaterialPaths[i].szMaterialTexturePath[j]);
			if (nullptr == MeshMaterialDesc.pMtrlTexture[j])
				return E_FAIL;
		}

		m_vecMaterials.push_back(MeshMaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const MODEL_BINARYDATA& ModelData, class CBone* pParent)
{
	if (nullptr == ModelData.pBoneDatas)
		return S_OK;

	CBone* pBone = CBone::Create(ModelData.pBoneDatas[0], nullptr);
	if (nullptr == pBone)
		return E_FAIL;

	m_vecBones.push_back(pBone);

	string strRootTag = "";

	for (_uint i = 1; i < ModelData.iNumBones; ++i)
	{
		CBone* pBone = CBone::Create(ModelData.pBoneDatas[i], m_vecBones[ModelData.pBoneDatas[i].iParentIdx]);
		if (nullptr == pBone)
			return E_FAIL;

		strRootTag = ModelData.pBoneDatas[i].szName;
		if (string::npos != strRootTag.find("Bone") &&
			string::npos != strRootTag.find("Root"))
			m_iRootBoneIndex = i;

		m_vecBones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(const MODEL_BINARYDATA& ModelData)
{
	m_iNumAnimations = ModelData.iNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(ModelData.vecAnimations[i], m_vecBones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_vecAnimations.push_back(pAnimation);
	}

	return S_OK;
}

CAnimation* CModel::Find_Animation(const string& strTag)
{
	for (auto iter = m_vecAnimations.begin(); iter != m_vecAnimations.end(); ++iter)
	{
		if (string::npos != (*iter)->m_strName.find(strTag))
			return *iter;
	}

	return nullptr;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	TYPE eModelType, const MODEL_BINARYDATA& ModelData, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, ModelData, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CModel* CModel::Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(iLevelIndex, pOwner, pArg)))
	{
		MSG_BOX("Failed to Cloned CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	for (auto& Animation : m_vecAnimations)
		Safe_Release(Animation);
	m_vecAnimations.clear();

	for (auto& pMesh : m_vecMeshes)
		Safe_Release(pMesh);
	m_vecMeshes.clear();

	for (auto& Meterial : m_vecMaterials)
	{
		for (auto& Texture : Meterial.pMtrlTexture)
			Safe_Release(Texture);
	}
	m_vecMaterials.clear();

	for (auto& pBone : m_vecBones)
		Safe_Release(pBone);
	m_vecBones.clear();

	CComponent::Free();
}

#if defined(_USE_IMGUI) || defined(_DEBUG)

vector<KEYFRAME>& CModel::Get_MaxKeyFrames(const _uint iAnimIndex)
{
	return m_vecAnimations[iAnimIndex]->Get_MaxKeyFrames();
}

void CModel::Set_MaxKeyFrames(const vector<KEYFRAME>& vecKeyFrame)
{
	m_pCurrentAnimation->Set_MaxKeyFrames(vecKeyFrame);
}

vector<ANIMATIONDATA> CModel::Get_AnimationDatas()
{
	vector<ANIMATIONDATA> vecRet;

	for (auto& Animation : m_vecAnimations)
	{
		ANIMATIONDATA Data;
		strcpy_s(Data.szName, Animation->m_strName.c_str());
		Data.Duration = Animation->m_Duration;
		Data.TickPerSec = Animation->m_TickPerSec;
		Data.iNumChannels = Animation->m_iNumChannels;
		Data.bIsLoop = Animation->m_isLoop;
		Data.bIsFollowAnimation = Animation->m_isFollowAnimation;

		for (_uint i = 0; i < Data.iNumChannels; ++i)
		{
			CHANNELDATA ChannelData;
			strcpy_s(ChannelData.szName, Animation->m_vecChannels[i]->m_szName);
			ChannelData.iNumKeyFrames = Animation->m_vecChannels[i]->m_iNumKeyFrames;

			for (_uint j = 0; j < ChannelData.iNumKeyFrames; ++j)
			{
				KEYFRAME KeyFrame;
				KeyFrame.vScale = Animation->m_vecChannels[i]->m_vecKeyFrames[j].vScale;
				KeyFrame.vRotation = Animation->m_vecChannels[i]->m_vecKeyFrames[j].vRotation;
				KeyFrame.vTranslation = Animation->m_vecChannels[i]->m_vecKeyFrames[j].vTranslation;
				KeyFrame.Time = Animation->m_vecChannels[i]->m_vecKeyFrames[j].Time;
				KeyFrame.isChangeAnim = Animation->m_vecChannels[i]->m_vecKeyFrames[j].isChangeAnim;
				KeyFrame.isEnable = Animation->m_vecChannels[i]->m_vecKeyFrames[j].isEnable;

				ChannelData.vecKeyFrames.push_back(KeyFrame);
			}
			Data.vecChannels.push_back(ChannelData);
		}
		vecRet.push_back(Data);
	}

	return vecRet;
}

const _uint& CModel::Get_MaxNumKeyFrame() const
{
	return m_pCurrentAnimation->Get_MaxNumKeyFrame();
}

const _uint& CModel::Get_MaxRootKeyFrame() const
{
	return m_pCurrentAnimation->Get_MaxRootKeyFrames();
}

const _uint& CModel::Get_CurrentKeyFrameIndex() const
{
	return m_pCurrentAnimation->Get_CurrentKeyFrameIndex();
}

const _uint& CModel::Get_CurrentRootKeyFrameIndex() const
{
	return m_pCurrentAnimation->Get_CurrentRootKeyFrameIndex();
}

void CModel::Set_KeyFrame(const _uint& iIndex)
{
	m_pCurrentAnimation->Set_KeyFrame(iIndex);
}

void CModel::Set_RootKeyFrame(const _uint& iIndex)
{
	m_pCurrentAnimation->Set_RootKeyFrame(iIndex);
}

void CModel::Set_Translation(const _uint& iIndex, const _float3& vTranslation)
{
	vector<KEYFRAME>& KeyFrames = m_pCurrentAnimation->Get_RootKeyFrames();

	KeyFrames[iIndex].vTranslation = vTranslation;
}

void CModel::Set_KeyFrameTime(const _uint& iIndex, const _float& fTime)
{
	vector<KEYFRAME>& KeyFrames = m_pCurrentAnimation->Get_MaxKeyFrames();

	KeyFrames[iIndex].Time = fTime;
}

void CModel::Set_Duration(const _float& fDuration)
{
	m_pCurrentAnimation->m_Duration = (_double)fDuration;
}

void CModel::Set_TickPerSec(const _float& fTickPerSec)
{
	m_pCurrentAnimation->m_TickPerSec = (_double)fTickPerSec;
}

_float3 CModel::Get_Translation(const _uint& iIndex)
{
	vector<KEYFRAME> Data = m_pCurrentAnimation->Get_RootKeyFrames();
	return Data[iIndex].vTranslation;
}

_float CModel::Get_KeyFrameTime(const _uint& iIndex)
{
	vector<KEYFRAME> Data = m_pCurrentAnimation->Get_MaxKeyFrames();
	return Data[iIndex].Time;
}

_float CModel::Get_Duration()
{
	return _float(m_pCurrentAnimation->m_Duration);
}

_float CModel::Get_TickPerSec()
{
	return _float(m_pCurrentAnimation->m_TickPerSec);
}

_bool CModel::isPause() const
{
	return m_pCurrentAnimation->m_isPause;
}

HRESULT CModel::Set_Animation(const string& strTag, const ANIMATIONDATA& AnimData)
{
	CAnimation* pAnimation = Find_Animation(strTag);
	if (nullptr == pAnimation)
		return E_FAIL;

	pAnimation->m_strName = AnimData.szName;
	pAnimation->m_Duration = AnimData.Duration;
	pAnimation->m_TickPerSec = AnimData.TickPerSec;
	pAnimation->m_isLoop = AnimData.bIsLoop;
	pAnimation->m_isFollowAnimation = AnimData.bIsFollowAnimation;

	for (_uint i = 0; i < pAnimation->m_iNumChannels; ++i)
	{
		for (_uint j = 0; j < pAnimation->m_vecChannels[i]->m_iNumKeyFrames; ++j)
		{
			pAnimation->m_vecChannels[i]->m_vecKeyFrames[j].Time = AnimData.vecChannels[i].vecKeyFrames[j].Time;
		}
	}

	return S_OK;
}

HRESULT CModel::Add_Animation(const ANIMATIONDATA& AnimData)
{
	CAnimation* pAnimation = CAnimation::Create(AnimData, m_vecBones);
	if (nullptr == pAnimation)
		return E_FAIL;

	m_vecAnimations.push_back(pAnimation);
	++m_iNumAnimations;

	return S_OK;
}

HRESULT CModel::Delete_Animation(const string& strTag)
{
	for (auto iter = m_vecAnimations.begin(); iter != m_vecAnimations.end(); ++iter)
	{
		if (string::npos != (*iter)->m_strName.find(strTag))
		{
			Safe_Release(*iter);
			m_pCurrentAnimation = *(iter - 1);
			m_vecAnimations.erase(iter);
			return S_OK;
		}
	}

	return E_FAIL;
}

#endif