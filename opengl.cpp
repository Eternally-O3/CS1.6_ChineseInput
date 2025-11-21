#include"pch.h"

typedef BOOL(APIENTRY* wglSwapBuffers_t)(HDC  hdc);
wglSwapBuffers_t pwglSwapBuffers = NULL;

BOOL APIENTRY Hooked_wglSwapBuffers(HDC hdc)
{
	HookImGui(hdc);
	return(*pwglSwapBuffers)(hdc);
}

void HookOpenGL()
{
	HMODULE hmOpenGL = GetModuleHandle(L"opengl32.dll");
	if (hmOpenGL)
	{
		
		pwglSwapBuffers = (wglSwapBuffers_t)DetourFunction((LPBYTE)GetProcAddress(hmOpenGL, "wglSwapBuffers"), (LPBYTE)&Hooked_wglSwapBuffers);
		
	}
}
