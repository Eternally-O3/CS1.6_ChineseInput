#pragma once
// IME状态结构
struct IMEState {
    bool isComposing;          // 是否正在组合
    std::wstring composition;  // 组合字符串
    std::vector<std::wstring> candidates; // 候选列表
    int candidatePage;         // 候选页
    int candidatePageSize;     // 每页候选数
};
extern HWND hGameWnd;
extern bool bShowMenu;
extern IMEState g_imeState;
void HookImGui(HDC hdc);
void UpdateIMEPosition(HWND hWnd);
void TempDisableIME(HWND hWnd);
void TempEnableIME(HWND hWnd);