#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

/*for Fstream*/
#include <fstream>

namespace Engine
{
	enum MOUSEKEYSTATE { DIMKS_LBUTTON, DIMKS_RBUTTON, DIMKS_WHEEL, DIMKS_X, DIMKS_END };
	enum MOUSEMOVESTATE { DIMMS_X, DIMMS_Y, DIMMS_WHEEL, DIMMS_END };
}

#define EVENT_NONE 0
#define EVENT_DEAD 1

#include "Effects11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/ScreenGrab.h"
#include "DirectXTK/PrimitiveBatch.h"
#include "DirectXTK/VertexTypes.h"
#include "DirectXTK/Effects.h"

#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"

#ifdef _DEBUG
#define USEASSIMP 1
#endif // _DEBUG


#ifdef USEASSIMP

#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#endif // USEASSIMP

#include <random>

/* FMod*/
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h"

#include <tchar.h>
#include <io.h>

#pragma comment (lib, "fmod_vc.lib")


using namespace DirectX;

#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <map>

using namespace std;

namespace Engine
{
	const wstring g_strTransformTag = TEXT("Com_Transform");

	enum EKeyState
	{
		NONE, DOWN, PRESSING, UP, _END
	};
}

#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"

using namespace Engine;


#ifdef _DEBUG
/* ImGui */
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#endif

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

