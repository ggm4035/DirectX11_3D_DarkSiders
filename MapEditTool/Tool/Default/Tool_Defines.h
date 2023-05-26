#pragma once

namespace Tool
{
	enum LEVELID { LEVEL_TOOLMAP, LEVEL_END };
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;
}
extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Tool;