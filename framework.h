#pragma once

//#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include <tchar.h>
#include <ntstatus.h>
#include <fstream>
#include <shlobj.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "detours.h"

#include "ImGui/imconfig.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_opengl2.h"
#include "ImGui/imgui_impl_win32.h"
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

#include"opengl.h"
#include "hook.h"
#include"Input.h"
#include"sdk.h"
#include "offset.h"
