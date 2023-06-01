#pragma once

#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)

public:
	enum TRANSFORMSTATE { STATE_VIEW, STATE_PROJ, STATE_END };

private:
	explicit CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_matrix Get_Transform_Matrix(TRANSFORMSTATE eState);
	_matrix Get_Transform_Inverse_Matrix(TRANSFORMSTATE eState);
	_float4x4 Get_Transform_Float4x4(TRANSFORMSTATE eState);
	_float4x4 Get_Trasnform_Inverse_Float4x4(TRANSFORMSTATE eState);

	_matrix Get_UI_View_Matrix();
	_float4x4 Get_UI_View_Float4x4();
	_matrix Get_UI_Proj_Matrix(const _uint& iWinSizeX, const _uint& iWinSizeY);
	_float4x4 Get_UI_Proj_Float4x4(const _uint& iWinSizeX, const _uint& iWinSizeY);

	HRESULT Set_Transform(TRANSFORMSTATE eState, _fmatrix _Matrix);

public:
	void Tick();

private:
	_float4x4 m_Transform[STATE_END];
	_float4x4 m_Transform_Inverse[STATE_END];

public:
	virtual void Free() override;
};

END