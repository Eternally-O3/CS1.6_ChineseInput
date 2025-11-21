#include "pch.h"
cl_clientfunc_t g_Client, * g_pClient;
cl_enginefunc_t g_Engine, * g_pEngine;
std::unordered_map<std::string, xcommand_t> g_ClientCommands;



bool Initialize()
{
	for (auto it = g_pEngine->pfnGetCmdList(); it; it = it->next)
		g_ClientCommands[it->name] = it->function;

	return !g_ClientCommands.empty();
}

bool FindModuleByName(const char* name, ModuleInfo* info)
{
	assert(info);

	if (!name || !strlen(name))
		return false;

	HMODULE handle = GetModuleHandleA(name);

	if (!handle || handle == INVALID_HANDLE_VALUE)
		return false;

	IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)handle;
	IMAGE_NT_HEADERS* pe = (IMAGE_NT_HEADERS*)((uintptr_t)dos + (uintptr_t)dos->e_lfanew);

	if (pe->Signature != IMAGE_NT_SIGNATURE)
		return false;

	info->handle = handle;
	info->base = (uintptr_t)handle;
	info->size = (uintptr_t)pe->OptionalHeader.SizeOfImage;
	info->end = info->base + info->size - 1;

	return true;
}


bool FindModules()
{

	if (!FindModuleByName("hw.dll", &Modules[HW]))
		return false;

	if (!FindModuleByName("client.dll", &Modules[CLIENT]))
		return false;

	return true;
}


bool FindGameOffsets()
{
#define CHECK_OFFSET(ptr) assert(ptr); \
		if (!(ptr)) \
			return false;
	
	if (!FindModules())
		return false;
	
	static auto read = [](uintptr_t ptr, int offset = 0)
		{
			if (ptr)
				return (uintptr_t)(*(uintptr_t*)ptr) + offset;
	
			return uintptr_t{};
		};
	CHECK_OFFSET(g_pClient = (cl_clientfunc_t*)read(FindPattern("FF 15 ?? ?? ?? ?? 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 1C", Modules[HW].base, Modules[HW].end, 2)));
	CHECK_OFFSET(g_pEngine = (cl_enginefunc_t*)read(FindPattern("FF 15 ?? ?? ?? ?? 83 C4 04 89 87 18 77 01 00", Modules[CLIENT].base, Modules[CLIENT].end, 2)));

	memcpy(&g_Client, g_pClient, sizeof(g_Client));
	memcpy(&g_Engine, g_pEngine, sizeof(g_Engine));



	g_pClient->HUD_Key_Event = HUD_Key_Event;

	if (!Initialize())return false;
	

	return true;
}



uintptr_t FindPattern(const char* signature, uintptr_t start, uintptr_t end, int offset)
{
	assert(start && end && start != end && signature && strlen(signature));

	static auto pattern_to_byte = [](const char* pattern)
		{
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;
					if (*current == '?')
						++current;

					bytes.push_back(-1);
				}
				else
					bytes.push_back(strtoul(current, &current, 16));
			}

			return bytes;
		};

	const auto pattern = pattern_to_byte(signature);
	const auto patternBytes = pattern.data();
	const auto patternLength = pattern.size();

	bool up = false;

	if (end < start)
		up = true;

	if (!up)
	{
		for (auto i = start; i < end - patternLength; ++i)
		{
			bool found = true;

			for (size_t j = 0; j < patternLength; ++j)
			{
				if (patternBytes[j] != -1 && (CHAR)patternBytes[j] != *(PCHAR)(i + j))
				{
					found = false;
					break;
				}
			}

			if (found)
				return i + offset;
		}
	}
	else
	{
		for (auto i = start; i > end - patternLength; --i)
		{
			bool found = true;

			for (size_t j = 0; j < patternLength; ++j)
			{
				if (patternBytes[j] != -1 && (CHAR)patternBytes[j] != *(PCHAR)(i + j))
				{
					found = false;
					break;
				}
			}

			if (found)
				return i + offset;
		}
	}

	return 0;
}

int HUD_Key_Event(int down, int keynum, const char* pszCurrentBinding)
{
	if (bShowMenu)
	{
		TempEnableIME(hGameWnd);
		
		return false;
	}
	return g_Client.HUD_Key_Event(down, keynum, pszCurrentBinding);
}