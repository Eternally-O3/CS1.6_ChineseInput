#include "pch.h"

HWND hGameWnd;
WNDPROC hGameWndProc;
bool bShowMenu = false;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);


std::string GetAppDataPath(int folderId) {
	char path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathA(NULL, folderId, NULL, 0, path))) {
		return std::string(path);
	}
	return "";
}

void SetIMEToChineseMode(HWND hWnd)
{
    HIMC himc = ImmGetContext(hWnd);
    if (!himc) return;

   
    DWORD conversion = IME_CMODE_NATIVE;    // 只保留本地字符集模式
    DWORD sentence = IME_SMODE_PHRASEPREDICT;

   
    conversion &= ~IME_CMODE_ALPHANUMERIC;  // 确保移除字母数字模式
    conversion &= ~IME_CMODE_KATAKANA;      // 移除片假名模式
    conversion &= ~IME_CMODE_LANGUAGE;      // 移除语言模式

    // 设置中文输入需要的模式
    conversion |= IME_CMODE_NATIVE;
    conversion |= IME_CMODE_FULLSHAPE;      // 全角模式

    ImmSetConversionStatus(himc, conversion, sentence);

    // 验证设置
    DWORD new_conversion, new_sentence;
    ImmGetConversionStatus(himc, &new_conversion, &new_sentence);
    

    ImmReleaseContext(hWnd, himc);
}


void GetCandidatesDirect(HIMC himc)
{
    g_imeState.candidates.clear();

    if (!himc) {
       
        return;
    }

    
    for (DWORD dwIndex = 0; dwIndex < 4; dwIndex++) {
        DWORD dwBufLen = ImmGetCandidateListW(himc, dwIndex, NULL, 0);
        

        if (dwBufLen > 0) {
            LPCANDIDATELIST pCandList = (LPCANDIDATELIST)new BYTE[dwBufLen];
            if (ImmGetCandidateListW(himc, dwIndex, pCandList, dwBufLen)) {
               

                for (DWORD i = 0; i < pCandList->dwCount; i++) {
                    if (pCandList->dwOffset[i] < dwBufLen) {
                        WCHAR* candidate = (WCHAR*)((BYTE*)pCandList + pCandList->dwOffset[i]);
                        if (candidate && wcslen(candidate) > 0) {
                            
                            g_imeState.candidates.push_back(candidate);
                        }
                    }
                }
            }
            delete[](BYTE*)pCandList;

            if (!g_imeState.candidates.empty()) {
                break; 
            }
        }
    }

    if (g_imeState.candidates.empty()) {
       
    }
}

void CheckCompositionStatus(HIMC himc)
{
    if (!himc) return;

    
    DWORD compStrLen = ImmGetCompositionStringW(himc, GCS_COMPSTR, NULL, 0);
    
    if (compStrLen > 0) {
        std::vector<WCHAR> buffer(compStrLen / sizeof(WCHAR) + 1);
        ImmGetCompositionStringW(himc, GCS_COMPSTR, buffer.data(), compStrLen);
        buffer[compStrLen / sizeof(WCHAR)] = L'\0';
        
    }
    else {
        
    }

   
    DWORD candInfoLen = ImmGetCandidateListW(himc, 0, NULL, 0);
    
}


LRESULT HandleIMENotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HIMC himc = ImmGetContext(hWnd);
    if (!himc) {
        
        return DefWindowProc(hWnd, WM_IME_NOTIFY, wParam, lParam);
    }

    

    switch (wParam) {
    case IMN_OPENCANDIDATE:
        
        CheckCompositionStatus(himc);
        
        SetIMEToChineseMode(hWnd);
        UpdateIMEPosition(hWnd);
        GetCandidatesDirect(himc);
        break;

    case IMN_CHANGECANDIDATE:
        
        UpdateIMEPosition(hWnd);
        GetCandidatesDirect(himc);
        break;

    case IMN_SETCONVERSIONMODE:
    {
        DWORD conversion, sentence;
        ImmGetConversionStatus(himc, &conversion, &sentence);
        
        SetIMEToChineseMode(hWnd);
    }
    break;
    }

    LRESULT result = DefWindowProc(hWnd, WM_IME_NOTIFY, wParam, lParam);
    ImmReleaseContext(hWnd, himc);
    return result;
}

void UpdateIMEPosition(HWND hWnd)
{
    if (!bShowMenu) return;

    HIMC himc = ImmGetContext(hWnd);
    if (!himc) return;

    
    ImVec2 input_pos = ImGui::GetItemRectMin();
    ImVec2 input_size = ImGui::GetItemRectSize();

    
    POINT screen_pos;
    screen_pos.x = (LONG)input_pos.x;
    screen_pos.y = (LONG)(input_pos.y + input_size.y);
    ClientToScreen(hWnd, &screen_pos);

    
    COMPOSITIONFORM cf;
    cf.dwStyle = CFS_DEFAULT;
    cf.ptCurrentPos.x = screen_pos.x;
    cf.ptCurrentPos.y = screen_pos.y;
    ImmSetCompositionWindow(himc, &cf);

    
    for (DWORD i = 0; i < 4; i++) {
        CANDIDATEFORM cand;
        cand.dwIndex = i;
        cand.dwStyle = CFS_CANDIDATEPOS;
        cand.ptCurrentPos.x = screen_pos.x;
        cand.ptCurrentPos.y = screen_pos.y;
        ImmSetCandidateWindow(himc, &cand);
    }

    ImmReleaseContext(hWnd, himc);
}



IMEState g_imeState;

LRESULT CALLBACK HOOK_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MARGINS margins;

    
    if (bShowMenu && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    switch (uMsg)
    {
    case WM_CREATE:
        margins = { -1 };
        DwmExtendFrameIntoClientArea(hWnd, &margins);
        break;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)bShowMenu = false;
        if (wParam == VK_RETURN)
        {
            bShowMenu = !bShowMenu;
           
            if (!bShowMenu)
            {
                TempDisableIME(hGameWnd);
                if (strlen(input_buffer) != 0)
                {
                    std::string input(input_buffer);
                    std::string msginfo;
                    msginfo += "say ";
                    msginfo += input;
                    g_pEngine->pfnClientCmd(msginfo.c_str());
                }
                memset(input_buffer, 0, sizeof(input_buffer));
                
            }
            return true;
        }
        break;

        // IME消息处理
    case WM_IME_STARTCOMPOSITION:
        if (bShowMenu) {
            
            g_imeState.isComposing = true;
            g_imeState.composition.clear();
            g_imeState.candidates.clear();
            
            SetIMEToChineseMode(hWnd);
            UpdateIMEPosition(hWnd);
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
        break;

    case WM_IME_COMPOSITION:
        if (bShowMenu) {
            HIMC himc = ImmGetContext(hWnd);
            if (himc) {
                

                
                if (lParam & GCS_COMPSTR) {
                    CheckCompositionStatus(himc);
                    
                    SetIMEToChineseMode(hWnd);

                    
                    if (lParam & GCS_COMPSTR) {
                        GetCandidatesDirect(himc);
                    }
                }

                UpdateIMEPosition(hWnd);
                ImmReleaseContext(hWnd, himc);
            }
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
        break;

    case WM_IME_NOTIFY:
        if (bShowMenu) {
            return HandleIMENotify(hWnd, wParam, lParam);
        }
        break;

    case WM_IME_ENDCOMPOSITION:
        if (bShowMenu) {
           
            g_imeState.isComposing = false;
            g_imeState.composition.clear();
            g_imeState.candidates.clear();
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
        break;

    case WM_IME_SETCONTEXT:
        if (bShowMenu) {
            // 显示所有IME UI
            lParam |= ISC_SHOWUIALL;
            //return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
        break;

    case WM_INPUTLANGCHANGE:
        
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    
    return CallWindowProc(hGameWndProc, hWnd, uMsg, wParam, lParam);
}

void InistalizeImgui(HDC hdc)
{

	static bool bInitializeImGui = false;

	if (!bInitializeImGui)
	{
		hGameWnd = WindowFromDC(hdc);
		hGameWndProc = (WNDPROC)SetWindowLong(hGameWnd, GWL_WNDPROC, (LONG)HOOK_WndProc);
		
        
      
        HIMC himc = ImmGetContext(hGameWnd);
        if (!himc) {
           
            himc = ImmCreateContext();
            if (himc) {
                ImmAssociateContext(hGameWnd, himc);  // 关联上下文               
            }
            else {
                
                return;
            }
        }
        ImmSetOpenStatus(himc, FALSE);  
        ImmReleaseContext(hGameWnd, himc);
		ImGui::CreateContext();
		
		ImGui::StyleColorsLight();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;
		io.LogFilename = nullptr;
		
		char fontsPath[MAX_PATH];
		if (SHGetFolderPathA(NULL, CSIDL_FONTS, NULL, 0, fontsPath) == S_OK) {
			std::string fontPath = std::string(fontsPath) + "\\simhei.ttf";
			
			if (GetFileAttributesA(fontPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
				io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 14.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
			}
		}
		

		ImGui_ImplOpenGL2_Init();
		ImGui_ImplWin32_Init(hGameWnd);
		
		bInitializeImGui = true;
		ImGui_ImplOpenGL2_DestroyDeviceObjects();
		ImGui_ImplOpenGL2_CreateDeviceObjects();
	}
}


void HookImGui(HDC hdc)
{
	InistalizeImgui(hdc);
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Input_box();
	

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void TempDisableIME(HWND hWnd)
{
    HIMC himc = ImmGetContext(hWnd);
    if (himc) {
        ImmSetOpenStatus(himc, FALSE);  // 只是关闭IME，不移除上下文
        ImmReleaseContext(hWnd, himc);
        
    }
}


// 重新启用IME
void TempEnableIME(HWND hWnd)
{
    HIMC himc = ImmGetContext(hWnd);
    if (himc) {
        ImmSetOpenStatus(himc, TRUE);   // 重新打开IME
        ImmReleaseContext(hWnd, himc);
        
    }
}