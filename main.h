//==============================================================================
//  File   : main.h
//  Brief  : 
// 
//  Author : Ryoto Kikuchi
//  Date   : 2026/4/21
//------------------------------------------------------------------------------
//
//==============================================================================
#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>
#include <assert.h>
#include <functional>

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")


#include <DirectXMath.h>
using namespace DirectX;

#include "DirectXTex.h"
#if _DEBUG
#pragma comment(lib, "DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex_Release.lib")
#endif


#pragma comment (lib, "winmm.lib")

#include "vector3.h"
#include "time.h"

#include "imgui\\imgui.h"
#include "imgui\\imgui_impl_win32.h"
#include "imgui\\imgui_impl_dx11.h"

#define SCREEN_WIDTH	(1280)
#define SCREEN_HEIGHT	(720)


HWND GetWindow();

void Invoke(std::function<void()> Function, int Time);

