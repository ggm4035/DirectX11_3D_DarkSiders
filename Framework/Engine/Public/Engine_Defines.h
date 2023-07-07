#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)
#pragma warning (disable : 26812)

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "DirectXCollision.h"

#define DIRECTINPUT_VERSION		0x0800
#include <dinput.h>

#include "fx11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectxTK/ScreenGrab.h"
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\Effects.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"
using namespace DirectX;

#include <vector>
#include <queue>
#include <list>
#include <map>
#include <any>
#include <unordered_map>
#include <functional>
#include <string>
#include <algorithm>
#include <atlimage.h>
#include <fstream>
using namespace std;

#include "Engine_Macro.h"
#include "Engine_Enums.h"
#include "Engine_TypeDef.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <iostream>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 

#ifndef _USE_IMGUI
#define new DBG_NEW 
#else
#define New DBG_NEW
#endif // _USE_IMGUI

#endif // _DEBUG
#else

#if defined(_USE_IMGUI) || defined(_DEBUG)
#define New new
#endif // _USE_IMGUI

#endif // _DEBUG


using namespace Engine;
