#pragma once

namespace Engine
{
	
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WM_FULL, WM_WIN, WM_END };
		HWND			hWnd;
		unsigned int	iViewportSizeX, iViewportSizeY;
		WINMODE			eWinMode;
	}GRAPHICDESC;

	typedef struct tagVertex_Position_TexCoord
	{
		XMFLOAT3 vPosition;
		XMFLOAT2 vTexCoord;
	}VTXPOSTEX;

	typedef struct tag_View_Elements
	{
		XMMATRIX ViewMatrix;

		XMVECTOR vEye;
		XMVECTOR vAt;
		XMVECTOR vUp;

		tag_View_Elements()
		{
			ZeroMemory(this, sizeof(tag_View_Elements));
			ViewMatrix = XMMatrixIdentity();
		}
		tag_View_Elements(const XMVECTOR& vEye, const XMVECTOR& vAt, const XMVECTOR& vUp = XMVECTOR{ 0.f, 1.f, 0.f, 0.f })
		{
			this->vEye = vEye;
			this->vAt = vAt;
			this->vUp = vUp;

			LookAtLH();
		}
		tag_View_Elements(const tag_View_Elements& rhs)
		{
			this->vEye = rhs.vEye;
			this->vAt = rhs.vAt;
			this->vUp = rhs.vUp;

			LookAtLH();
		}
		void LookAtLH()
		{
			ViewMatrix = XMMatrixLookAtLH(vEye, vAt, vUp);
		}
	}VIEWELEM;

	typedef struct tag_Projection_Elements
	{
		XMMATRIX ProjectionMatrix;

		_float fFov;
		_float fAspect;
		_float fNear;
		_float fFar;

		tag_Projection_Elements()
		{
			ZeroMemory(this, sizeof(tag_Projection_Elements));
			ProjectionMatrix = XMMatrixIdentity();
		}
		tag_Projection_Elements(const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
		{
			this->fFov = fFov;
			this->fAspect = fAspect;
			this->fNear = fNear;
			this->fFar = fFar;

			PerspectiveLH();
		}
		tag_Projection_Elements(const tag_Projection_Elements& rhs)
		{
			this->fFov = rhs.fFov;
			this->fAspect = rhs.fAspect;
			this->fNear = rhs.fNear;
			this->fFar = rhs.fFar;

			PerspectiveLH();
		}
		void PerspectiveLH(void)
		{
			ProjectionMatrix = XMMatrixPerspectiveFovLH(fFov, fAspect, fNear, fFar);
		}
	}PROJELEM;

	typedef struct ENGINE_DLL tagVertex_Position_TexCoord_Declaration
	{
		static const _uint iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOSTEX_DECL;
}
