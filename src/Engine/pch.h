#ifndef PCH_H
#define PCH_H

//DirectX :
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>

//PreCompiled lib files for d12:
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")

//Standard Library:
#include <vector>
#include "unordered_map"
#include <iostream>
#include <sstream>
#include <string>

// Engine Files:
#include "EngineMacro.h"
#include "StatsSystem.h"
#include "Component.h"
#include "ECSManager.h"
#include "Camera.h"

#include <Windows.h>

#endif