#include "CModel.h"
#include "CMesh.h"
#include "CBone.h"
#include "CAnimation.h"
#include "CChannel.h"

#include "CGameInstance.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
	ZeroMemory(&m_vCurPosition, sizeof _float4);
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
	, m_iRootBoneIndex(rhs.m_iRootBoneIndex)
{
	ZeroMemory(&m_vCurPosition, sizeof _float4);

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
	return m_vecAnimations[m_iCurrentAnimIndex]->isLoop();
}

_bool CModel::isFinishedAnimation() const
{
	return m_vecAnimations[m_iCurrentAnimIndex]->m_isFinished;
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

void CModel::Set_AnimIndex(const _uint& iAnimIndex)
{
	if (iAnimIndex >= m_iNumAnimations ||
		iAnimIndex == m_iCurrentAnimIndex)
		return;

	if (false == m_vecAnimations[m_iCurrentAnimIndex]->isLerped())
		m_vecAnimations[m_iCurrentAnimIndex]->Reset_Animation();

	m_vecAnimations[iAnimIndex]->Bind_LerpAnimation(m_vecAnimations[m_iCurrentAnimIndex]);

	m_iCurrentAnimIndex = iAnimIndex;

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

	return S_OK;
}

HRESULT CModel::Render(const _uint& iMeshIndex)
{
	if (iMeshIndex < 0 || iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_vecMeshes[iMeshIndex]->Render();
}

_float4 CModel::ComputeAnimMovement()
{
	CTransform* pTransform = dynamic_cast<CGameObject3D*>(m_pOwner)->Get_Transform();

	if (true == m_vecAnimations[m_iCurrentAnimIndex]->isLoop())
	{
		_float4 vPosition;
		XMStoreFloat4(&vPosition, pTransform->Get_State(CTransform::STATE_POSITION));
		return vPosition;
	}

	if (true == m_isFirst)
	{
		XMStoreFloat4x4(&m_WorldMatrix, pTransform->Get_WorldMatrix());
		m_isFirst = false;
		return _float4(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, m_WorldMatrix._44);
	}

	/* 오프셋 * 컴바인 * 피벗으로 로컬스페이스 상의 뼈의 상태행렬을 가져온다. */
	_matrix BoneMatrix = m_vecBones[m_iRootBoneIndex]->Get_OffsetMatrix() * m_vecBones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_PivotMatrix);

	_matrix WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix); /* 객체의 상태(월드)행렬 */

	for (_uint i = 0; i < 3; ++i)
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

	BoneMatrix *= WorldMatrix;

	XMStoreFloat4(&m_vCurPosition, BoneMatrix.r[3]);

	//cout << "X: " << m_vCurPosition.x << " Y: " << m_vCurPosition.y << " Z: " << m_vCurPosition.z << endl;
	return m_vCurPosition;
}

void CModel::Play_Animation(const _double& TimeDelta)
{
	/* 어떤 애니메이션을 재생하려고하는지?! */
	/* 이 애니메이션은 어떤 뼈를 사용하는지?! */
	/* 뼈들은 각각 어떤 상태(TransformationMatrix)를 취하고 있어야하는가?! */

	/* 현재 애니메이션에서 사용하는 뼈들을 찾아서 해당 뼈들의 TransformationMatrix를 갱신한다. */
		
	/* TransformationMatrix 를 선형보간한다. */
	/* 여기가 실질적으로 뼈가 이동하는 부분 */

	if (false == m_isBlock)
		m_vecAnimations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(m_vecBones, TimeDelta);
	else /* 벽에 부딪힌 경우 이동값을 죽인다. */
		m_vecAnimations[m_iCurrentAnimIndex]->Invalidate_NotMoveTransformationMatrix(m_vecBones, TimeDelta);

	/* 선형보간한 TransformationMatrix들을 CombinedTransformation 에 적용시킨다. */
	for (auto& pBone : m_vecBones)
		pBone->Invalidate_CombinedTransformationMatrix(m_vecBones);
}

void CModel::Pause_Animation()
{
	m_vecAnimations[m_iCurrentAnimIndex]->Pause_Animation();
}

void CModel::RePlay_Animation()
{
	m_vecAnimations[m_iCurrentAnimIndex]->Play_Animation();
}

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
		CAnimation* pAnimation = CAnimation::Create(ModelData.pAnimations[i], m_vecBones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_vecAnimations.push_back(pAnimation);
	}

	return S_OK;
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

vector<ANIMATIONDATA> CModel::Get_AnimationDatas()
{
	vector<ANIMATIONDATA> vecRet;

	for (auto& Animation : m_vecAnimations)
	{
		ANIMATIONDATA Data;
		strcpy_s(Data.szName, Animation->m_szName);
		Data.Duration = Animation->m_Duration;
		Data.TickPerSec = Animation->m_TickPerSec;
		Data.iNumChannels = Animation->m_iNumChannels;
		Data.bIsLoop = Animation->m_isLoop;

		CHANNELDATA* pChannels = new CHANNELDATA[Data.iNumChannels];
		for (_uint i = 0; i < Data.iNumChannels; ++i)
		{
			strcpy_s(pChannels[i].szName, Animation->m_vecChannels[i]->m_szName);
			pChannels[i].iNumKeyFrames = Animation->m_vecChannels[i]->m_iNumKeyFrames;

			KEYFRAME* pKeyFrame = new KEYFRAME[pChannels[i].iNumKeyFrames];
			for (_uint j = 0; j < pChannels[i].iNumKeyFrames; ++j)
			{
				pKeyFrame[j].vScale = Animation->m_vecChannels[i]->m_vecKeyFrames[j].vScale;
				pKeyFrame[j].vRotation = Animation->m_vecChannels[i]->m_vecKeyFrames[j].vRotation;
				pKeyFrame[j].vTranslation = Animation->m_vecChannels[i]->m_vecKeyFrames[j].vTranslation;
				pKeyFrame[j].Time = Animation->m_vecChannels[i]->m_vecKeyFrames[j].Time;
			}
			pChannels[i].pKeyFrames = pKeyFrame;
		}
		Data.pChannels = pChannels;
		vecRet.push_back(Data);
	}

	return vecRet;
}

const _uint& CModel::Get_MaxKeyFrame() const
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Get_MaxKeyFrames();
}

const _uint& CModel::Get_MaxRootKeyFrame() const
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Get_MaxRootKeyFrames();
}

const _uint& CModel::Get_CurrentKeyFrameIndex() const
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Get_CurrentKeyFrameIndex();
}

const _uint& CModel::Get_CurrentRootKeyFrameIndex() const
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Get_CurrentRootKeyFrameIndex();
}

void CModel::Set_KeyFrame(const _uint& iIndex)
{
	m_vecAnimations[m_iCurrentAnimIndex]->Set_KeyFrame(iIndex);
}

void CModel::Set_RootKeyFrame(const _uint& iIndex)
{
	m_vecAnimations[m_iCurrentAnimIndex]->Set_RootKeyFrame(iIndex);
}

void CModel::Set_Translation(const _uint& iIndex, const _float3& vTranslation)
{
	vector<KEYFRAME>& KeyFrames = m_vecAnimations[m_iCurrentAnimIndex]->Get_RootKeyFrames();

	KeyFrames[iIndex].vTranslation = vTranslation;
}

_float3 CModel::Get_Translation(const _uint& iIndex)
{
	vector<KEYFRAME> Data = m_vecAnimations[m_iCurrentAnimIndex]->Get_RootKeyFrames();
	return Data[iIndex].vTranslation;
}

HRESULT CModel::Set_Animation(_uint iAnimIndex, const ANIMATIONDATA& AnimData)
{
	m_vecAnimations[iAnimIndex]->m_Duration = AnimData.Duration;
	m_vecAnimations[iAnimIndex]->m_TickPerSec = AnimData.TickPerSec;
	m_vecAnimations[iAnimIndex]->m_isLoop = AnimData.bIsLoop;

	for (_uint i = 0; i < m_vecAnimations[iAnimIndex]->m_iNumChannels; ++i)
	{
		for (_uint j = 0; j < m_vecAnimations[iAnimIndex]->m_vecChannels[i]->m_iNumKeyFrames; ++j)
		{
			m_vecAnimations[iAnimIndex]->m_vecChannels[i]->m_vecKeyFrames[j].Time = AnimData.pChannels[i].pKeyFrames[j].Time;
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

HRESULT CModel::Delete_Animation(_uint iIndex)
{
	vector<CAnimation*>::iterator iter = m_vecAnimations.begin();

	Safe_Release(m_vecAnimations[iIndex]);
	m_vecAnimations.erase(iter + iIndex);

	return S_OK;
}

#endif