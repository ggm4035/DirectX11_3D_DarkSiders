#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#define DIRECTINPUT_VERSION		0x0800
#include <dinput.h>

#include "fx11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
using namespace DirectX;

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <atlimage.h>
#include <fstream>
using namespace std;

#include "Engine_Function.h"
#include "Engine_TypeDef.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

using namespace Engine;
