#pragma once
extern cl_enginefunc_t g_Engine, * g_pEngine;
extern cl_clientfunc_t g_Client, * g_pClient;
enum ModulesList
{
	HW,					 // "hw.dll"
	CLIENT,				 // "client.dll"
	GAMEUI,				 // "gameui.dll"
	VGUI2,				 // "vgui2.dll"
	STEAMCLIENT,		 // "steamclient.dll"
	SDL2,				 // "sdl32.dll"
	OPENGL32,			 // "opengl32.dll"
	MODULES_MAX_COUNT
};

struct ModuleInfo
{
	HMODULE handle;
	uintptr_t base;
	uintptr_t end;
	size_t size;
};

inline ModuleInfo Modules[MODULES_MAX_COUNT];

bool FindModules();
bool FindGameOffsets();
bool FindModuleByName(const char* name, ModuleInfo* info);
uintptr_t FindPattern(const char* signature, uintptr_t start, uintptr_t end, int offset = 0);
int HUD_Key_Event(int down, int keynum, const char* pszCurrentBinding);