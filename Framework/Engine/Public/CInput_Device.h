#pragma once

#include "CBase.h"

BEGIN(Engine)

class CInput_Device final : public CBase
{
	DECLARE_SINGLETON(CInput_Device)

public:
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

private:
	explicit CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_byte		Get_DIKeyState(const _ubyte& ubyKeyID)
	{
		return m_byKeyState[ubyKeyID];
	}
	_byte		Get_DIMouseState(MOUSEKEYSTATE eMouseID)
	{
		return m_tMouseState.rgbButtons[eMouseID];
	}

	_long		Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveID)
	{
		return *(((_long*)&m_tMouseState) + eMouseMoveID);
	}

public:
	HRESULT	Ready_DInput(HINSTANCE hInst, HWND hWnd);
	void	Update_DInput(void);

	_bool	Key_Pressing(const _ubyte& ubyKey);
	_bool	Key_Down(const _ubyte& ubyKey);
	_bool	Key_Up(const _ubyte& ubyKey);

	_bool	Mouse_Down(MOUSEKEYSTATE eMouseID);
	_bool	Mouse_Pressing(MOUSEKEYSTATE eMouseID);
	_bool	Mouse_Up(MOUSEKEYSTATE eMouseID);

private:
	LPDIRECTINPUT8			m_pInputSDK;

	LPDIRECTINPUTDEVICE8	m_pKeyBoard;
	LPDIRECTINPUTDEVICE8	m_pMouse;

private:
	_byte					m_byKeyState[MAX_DIK];
	_byte					m_byPreKeyState[MAX_DIK];
	DIMOUSESTATE			m_tMouseState;
	DIMOUSESTATE			m_tPreMouseState;

public:
	virtual void		Free(void) override;
};

END