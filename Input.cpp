#include "pch.h"
static RECT CwindowRect;
static int CwindowWidth, CwindowHeight;
char input_buffer[128] = "";
bool set_focus = true;

void Getwindow()
{
  
    if (hGameWnd) {
        GetWindowRect(hGameWnd, &CwindowRect);
        CwindowWidth = CwindowRect.right - CwindowRect.left;
        CwindowHeight = CwindowRect.bottom - CwindowRect.top;
    }
}

static float g_windowHeight = 50.0f; 
static float g_targetHeight = 50.0f; 
static float g_animationSpeed = 0.3f; 
static bool g_hasCandidates = false; // 当前是否有候选词

void Input_box()
{
    if (!bShowMenu)
    {
        set_focus = true;
        return;
    }
    ShowCursor(TRUE);
    // 更新动画状态
    UpdateWindowAnimation();

    ImGui::SetNextWindowBgAlpha(0.5f);
    ImGui::SetNextWindowSize(ImVec2(400, g_windowHeight), ImGuiCond_Always); 

    
    ImVec2 screen_size = ImGui::GetIO().DisplaySize;
    ImVec2 window_size = ImVec2(400, g_windowHeight);

   
    ImVec2 window_pos = ImVec2(
        (screen_size.x - window_size.x) * 0.5f,  
        screen_size.y * 0.67f - window_size.y * 0.5f  
    );
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 10.0f));

    ImGui::Begin("Chinese Input", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);


    ImGui::SetKeyboardFocusHere();

    if (set_focus) {        
        TempEnableIME(hGameWnd);
        set_focus = false;
    }

    
    ImGui::PushItemWidth(-1);
    ImGui::InputText("##ChineseInput", input_buffer, IM_ARRAYSIZE(input_buffer), ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CtrlEnterForNewLine);
    ImGui::PopItemWidth();

    // 显示候选框
    DisplayCandidates();

    ImGui::PopStyleVar(4);
    ImGui::End();
}



void DisplayCandidates()
{
    if (g_imeState.candidates.empty()) {
        return;
    }

    ImGui::Spacing();

    
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));

    if (ImGui::BeginChild("##Candidates", ImVec2(-1, 0), true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysUseWindowPadding)) {

        
        for (size_t i = 0; i < g_imeState.candidates.size() && i < 9; i++) {
            char cand_utf8[256];
            WideCharToMultiByte(CP_UTF8, 0, g_imeState.candidates[i].c_str(), -1, cand_utf8, 256, NULL, NULL);

            if (i > 0) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 0.6f), "|");
                ImGui::SameLine();
            }

           
              ImGui::Text("%d.%s", (int)i + 1, cand_utf8);
            
        }

        ImGui::EndChild();
    }

    ImGui::PopStyleVar(4);
    ImGui::PopStyleColor(2);
}


void UpdateWindowAnimation()
{
    
    bool nowHasCandidates = !g_imeState.candidates.empty();

    
    if (nowHasCandidates != g_hasCandidates) {
        g_hasCandidates = nowHasCandidates;

        if (g_hasCandidates) {
            
            int candidateCount = min((int)g_imeState.candidates.size(), 9);
            float candidateHeight = 40.0f; 
            g_targetHeight = 50.f + candidateHeight;
        }
        else {
            g_targetHeight = 50.f;
        }
    }
  
    float delta = g_targetHeight - g_windowHeight;
    if (fabs(delta) > 0.5f) {
        
        g_windowHeight += delta * g_animationSpeed;
    }
    else {
        g_windowHeight = g_targetHeight;
    }
}