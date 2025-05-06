#ifndef PCH_H
#define PCH_H

#include "framework.h"


#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <unordered_map>


//D12 header files
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

//General header
#include "DebugMacro.h"
#include "TColor.h"

#include "TDrawable.h"
#include "TTransform.h"

#endif //PCH_H
