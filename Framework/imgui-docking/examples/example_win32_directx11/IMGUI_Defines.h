#pragma once

#pragma warning (disable : 4828)

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <windows.h>
#include "CBase.h"
#include "ImGuiFileDialog.h"

#define IMFILE ImGuiFileDialog::Instance()

namespace Imgui
{
    const unsigned int g_iWinSizeX = 1280;
    const unsigned int g_iWinSizeY = 720;

    enum LEVELID { LEVEL_TOOL, LEVEL_END };
}

extern unsigned int g_ResizeWidth;
extern unsigned int g_ResizeHeight;
extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Imgui;
