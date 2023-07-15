#pragma once

namespace Engine
{
	typedef struct ENGINE_DLL tagGraphicDesc
	{
		enum WINMODE { WM_FULL, WM_WIN, WM_END };
		HWND			hWnd;
		HINSTANCE		hInst;
		unsigned int	iViewportSizeX;
		unsigned int	iViewportSizeY;
		WINMODE			eWinMode;
	}GRAPHICDESC;

	typedef struct tagMeshMaterial
	{
		class CTexture* pMtrlTexture[TEXTURE_TYPE_MAX];
	}MESHMATERIAL;

	typedef struct tagTriangleList
	{
		XMFLOAT3 vDot[3];
	}TRIANGLE;

	/* ========= OBSERVER DESC ========= */

	/* Notify Datas */
	typedef struct tagNotifyDesc
	{
		_float fPoint = { 0.f };
		vector<wstring> vecNotifyTags;
	}NOTIFYDESC;

	/* ========= VERTEX DESC ========= */

	typedef struct tagVertex_Position
	{
		XMFLOAT3 vPosition;
	}VTXPOS;

	typedef struct ENGINE_DLL tagVertex_Position_Declaration
	{
		static const _uint iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOS_DECL;

	typedef struct tagVertex_Cube
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vTexCoord;
	}VTXCUBE;

	typedef struct ENGINE_DLL tagVertex_Cube_Declaration
	{
		static const _uint iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXCUBE_DECL;

	typedef struct tagVertex_Position_Color
	{
		XMFLOAT3 vPosition;
		XMFLOAT4 vColor;
	}VTXPOSCOL;

	typedef struct ENGINE_DLL tagVertex_Color_Declaration
	{
		static const _uint iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOSCOL_DECL;

	typedef struct tagVertex_Position_TexCoord
	{
		XMFLOAT3 vPosition;
		XMFLOAT2 vTexCoord;
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertex_Position_TexCoord_Declaration
	{
		static const _uint iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOSTEX_DECL;

	typedef struct tagVertex_Position_Normal_TexCoord
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexCoord;
	}VTXPOSNORTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_TexCoord_Declaration
	{
		static const _uint iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOSNORTEX_DECL;

	/* Mesh Desc */

	typedef struct tagVertex_Mesh
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexCoord;
		XMFLOAT3 vTangent;
	}VTXMESH;

	typedef struct ENGINE_DLL tagVertex_Mesh_Declaration
	{
		static const _uint iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXMESH_DECL;

	typedef struct tagVertex_AnimMesh
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexCoord;
		XMFLOAT3 vTangent;
		XMUINT4	vBlendIndices; /* 이 정점은 어떤 뼈들(최대4개)의 상태행렬 받아서 처리되야하는가?! : 뼈(모델X, 메시O)의 인덱스 */
		XMFLOAT4 vBlendWeights; /* 네개뼈를 이용하되 각 뼈의 상태행렬을 몇퍼센트(1을 기준으로 표현한 비율)나 이용하는가?! */
	}VTXANIMMESH;

	typedef struct ENGINE_DLL tagVertex_AnimMesh_Declaration
	{
		static const _uint iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXANIMMESH_DECL;

	/* Instacne Desc */

	typedef struct tagVertex_Point
	{
		XMFLOAT3 vPosition;
		XMFLOAT2 vPSize;
	}VTXPOINT;

	typedef struct tagVertex_Instance
	{
		XMFLOAT4 vRight;
		XMFLOAT4 vUp;
		XMFLOAT4 vLook;
		XMFLOAT4 vTranslation;
	}VTXINSTANCE;

	typedef struct ENGINE_DLL tagVertex_Point_Instance_Declaration
	{
		static const _uint iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOINTINSTANCE_DECL;

	typedef struct ENGINE_DLL tagVertex_Rect_Instance_Declaration
	{
		static const _uint iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXRECTINSTANCE_DECL;

	/* ========= Save Data Model ========= */

	typedef struct tagBoneData
	{
		_char szName[MAX_PATH] = { "" };
		_int iParentIdx = { 0 };
		_uint iIndex = { 0 };
		_float4x4 TransformationMatrix;
		_float4x4 OffsetMatrix;
	}BONEDATA;

	typedef struct tagMaterialPath
	{
		_tchar szMaterialTexturePath[21][MAX_PATH] = { L"" };
	}MATERIALPATH;

	typedef struct tagMeshData
	{
		_char szName[MAX_PATH] = { "" };
		_uint iMaterialIndex = { 0 };
		_uint iNumNonAnimVertices = { 0 };
		_uint iNumAnimVertices = { 0 };
		_uint iNumIndices = { 0 };
		_uint iNumMeshBones = { 0 };	// 메쉬가 가지고 있는 뼈의 개수
		_uint* pBoneIndices = { nullptr };
		VTXMESH* pNonAnimVertices = { nullptr };
		VTXANIMMESH* pAnimVertices = { nullptr };
		_ulong* pIndices = { nullptr };
	}MESHDATA;

	typedef struct tagKeyFrame
	{
		_float3 vScale;
		_float4 vRotation;
		_float3 vTranslation;
		_double Time;
	}KEYFRAME;

	typedef struct tagChannelData
	{
		_char szName[MAX_PATH] = { "" };
		_uint iNumKeyFrames = { 0 };
		vector<KEYFRAME> vecKeyFrames;
	}CHANNELDATA;

	typedef struct tagAnimationData
	{
		_char szName[MAX_PATH] = { "" };
		_double Duration = { 0.0 };
		_double TickPerSec = { 0.0 };
		_uint iNumChannels = { 0 };
		_bool bIsLoop = { true };
		_bool bIsFollowAnimation = { true };
		_uint iNumPoints = { 0 };
		vector<NOTIFYDESC> vecNotifyDesc;
		vector<CHANNELDATA> vecChannels;
	}ANIMATIONDATA;

	typedef struct tagModelBinaryData
	{
		_tchar szTag[MAX_PATH] = { L"" };
		_uint iNumMeshes = { 0 };
		_uint iNumMaterials = { 0 };
		MATERIALPATH* pMaterialPaths = { nullptr };
		_uint iNumBones = { 0 }; // 모델이 가지고 있는 전체 뼈의 개수
		BONEDATA* pBoneDatas = { nullptr }; // 전체 모델의 뼈 정보
		_uint iNumAnimations = { 0 };
		vector<ANIMATIONDATA> vecAnimations;
		MESHDATA* pMeshDatas = { nullptr };
	}MODEL_BINARYDATA;

	/* ========= Save Data Object ========= */
	typedef struct tagModelData
	{
		_tchar szObjectTag[MAX_PATH] = { L"" };
		_float4x4 TransformMatrix;
		_float3 vAngle;
		MODEL_BINARYDATA BinaryData;
	}MODELDATA;

	typedef struct tagFileData
	{
		/* Player */
		_float4x4 WorldMatrix;
		_float3 vAngle;

		/* Terrain */
		_uint iXCount = { 0 };
		_uint iZCount = { 0 };
		_float3* pPositions = { nullptr };

		/* Models */
		vector<MODELDATA> vecModelData;
		vector<MODELDATA> vecMonsterData;
	}FILEDATA;
}
