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
		class CTexture* pMtrlTexture[AI_TEXTURE_TYPE_MAX];
	}MESHMATERIAL;

	typedef struct tagTriangleList
	{
		XMFLOAT3 vDot[3];
	}TRIANGLE;

	typedef struct tagVertex_Color
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
}
