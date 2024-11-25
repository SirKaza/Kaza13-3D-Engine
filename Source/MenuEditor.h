#pragma once
#include <imgui.h>
#include "ConsoleEditor.h"
#include "LogEditor.h"
#include "Globals.h"
#include "FrameData.h"

// Forward Declaration
void ShowAboutWindow(bool* p_open);
void ShowConfigurationWindow(bool* p_open);

ConsoleEditor myConsole;
LogEditor myLog;

// Data to be shared accross different functions of the demo.
struct ImGuiWindowData
{
    bool showLog = false;
    bool ShowConsole = false;
    bool ShowConfiguration = false;
    bool ShowAbout = false;
};

void ShowMenuEditor()
{
    static ImGuiWindowData editor_data;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Log", "", editor_data.showLog)) { editor_data.showLog = true; }
            if (ImGui::MenuItem("Console", "", editor_data.ShowConsole)) { editor_data.ShowConsole = true; }
            if (ImGui::MenuItem("Configuration", "", editor_data.ShowConfiguration)) { editor_data.ShowConfiguration = true; }
            ImGui::Separator();
            char menuText[64];
            std::snprintf(menuText, sizeof(menuText), "About %s", ENGINE_NAME);
            if (ImGui::MenuItem(menuText, "", editor_data.ShowAbout)) { editor_data.ShowAbout = true; }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (editor_data.showLog) myLog.Draw("Log", &editor_data.showLog);
    if (editor_data.ShowConsole) myConsole.Draw("Console", &editor_data.ShowConsole);
    if (editor_data.ShowConfiguration) ShowConfigurationWindow(&editor_data.ShowConfiguration);
    if (editor_data.ShowAbout) ShowAboutWindow(&editor_data.ShowAbout);
}

void ShowConfigurationWindow(bool* p_open)
{
    if (!ImGui::Begin("Configuration", p_open))
    {
        ImGui::End();
        return;
    }    

    ImGui::Text("Options");
    if (ImGui::CollapsingHeader("Application"))
    {
        // Some info
        ImGui::InputText("App Name", (char*)ENGINE_NAME, IM_ARRAYSIZE(ENGINE_NAME), ImGuiInputTextFlags_ReadOnly);
        ImGui::InputText("Organization", (char*)ORGANIZATION_NAME, IM_ARRAYSIZE(ORGANIZATION_NAME), ImGuiInputTextFlags_ReadOnly);

        // FPS Slider
        static int slider_FPS = 60;
        if (slider_FPS == 0) maxFPS = 60;
        else maxFPS = slider_FPS;
        static ImGuiSliderFlags flags = ImGuiSliderFlags_None;
        const ImGuiSliderFlags flags_for_sliders = flags & ~ImGuiSliderFlags_WrapAround;
        ImGui::SliderInt("Max FPS", &slider_FPS, 0, 60, "%d", flags_for_sliders);

        ImGui::Text("Limit Framerate: ");
        ImGui::SameLine();
        char buffer[10];
        sprintf(buffer, "%d", slider_FPS);
        const char* c_str = buffer;
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), c_str);

        // FPS & MS graph
        char title[25]; // Title text
        sprintf_s(title, sizeof(title), "Framerate %.1f", fps_log[fps_log.size()-1]);
        ImGui::PlotHistogram("##framerate", &fps_log[0], (int)fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
        sprintf_s(title, sizeof(title), "Milliseconds %0.1f", ms_log[ms_log.size()-1]);
        ImGui::PlotHistogram("##milliseconds", &ms_log[0], (int)ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
    }

    if (ImGui::CollapsingHeader("Window"))
    {

    }

    if (ImGui::CollapsingHeader("Hardware"))
    {

    }
    ImGui::End();
}


void ShowAboutWindow(bool* p_open)
{
    char menuText[64];
    std::snprintf(menuText, sizeof(menuText), "About %s", ENGINE_NAME);
    if (!ImGui::Begin(menuText, p_open))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("%s (%s)", ENGINE_NAME, ENGINE_VERSION);

    ImGui::TextLinkOpenURL("Homepage", "https://github.com/ocornut/imgui");
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("FAQ", "https://github.com/ocornut/imgui/blob/master/docs/FAQ.md");
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("Wiki", "https://github.com/ocornut/imgui/wiki");
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("Releases", "https://github.com/ocornut/imgui/releases");
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("Funding", "https://github.com/ocornut/imgui/wiki/Funding");

    ImGui::Separator();
    ImGui::Text("Game engine developed with OpenGL3 as part of a Video Game Master's Project at UPC.");
    ImGui::Text("By Marc Casanova Torrequebrada.");
    ImGui::Text("Libraries:");
    ImGui::Text(" - Window: SDL, v2");
    ImGui::Text(" - OpenGL extensions: Glew, v2.1.0");
    ImGui::Text(" - Editor: Dear ImGui, v1.91.5");
    ImGui::Text(" - More to add...");
    ImGui::Text("%s is licensed under the MIT License, see LICENSE for more information.", ENGINE_NAME);

    static bool show_config_info = false;
    ImGui::Checkbox("Config/Build Information", &show_config_info);
    if (show_config_info)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();

        bool copy_to_clipboard = ImGui::Button("Copy to clipboard");
        ImVec2 child_size = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 18);
        ImGui::BeginChild(ImGui::GetID("cfg_infos"), child_size, ImGuiChildFlags_FrameStyle);
        if (copy_to_clipboard)
        {
            ImGui::LogToClipboard();
            ImGui::LogText("```\n"); // Back quotes will make text appears without formatting when pasting on GitHub
        }

        ImGui::Text("%s (%s)", ENGINE_NAME, ENGINE_VERSION);
        ImGui::Separator();
        ImGui::Text("sizeof(size_t): %d, sizeof(ImDrawIdx): %d, sizeof(ImDrawVert): %d", (int)sizeof(size_t), (int)sizeof(ImDrawIdx), (int)sizeof(ImDrawVert));
        ImGui::Text("define: __cplusplus=%d", (int)__cplusplus);
#ifdef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_OBSOLETE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_WIN32_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_FILE_FUNCTIONS
        ImGui::Text("define: IMGUI_DISABLE_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_ALLOCATORS
        ImGui::Text("define: IMGUI_DISABLE_DEFAULT_ALLOCATORS");
#endif
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
        ImGui::Text("define: IMGUI_USE_BGRA_PACKED_COLOR");
#endif
#ifdef _WIN32
        ImGui::Text("define: _WIN32");
#endif
#ifdef _WIN64
        ImGui::Text("define: _WIN64");
#endif
#ifdef __linux__
        ImGui::Text("define: __linux__");
#endif
#ifdef __APPLE__
        ImGui::Text("define: __APPLE__");
#endif
#ifdef _MSC_VER
        ImGui::Text("define: _MSC_VER=%d", _MSC_VER);
#endif
#ifdef _MSVC_LANG
        ImGui::Text("define: _MSVC_LANG=%d", (int)_MSVC_LANG);
#endif
#ifdef __MINGW32__
        ImGui::Text("define: __MINGW32__");
#endif
#ifdef __MINGW64__
        ImGui::Text("define: __MINGW64__");
#endif
#ifdef __GNUC__
        ImGui::Text("define: __GNUC__=%d", (int)__GNUC__);
#endif
#ifdef __clang_version__
        ImGui::Text("define: __clang_version__=%s", __clang_version__);
#endif
#ifdef __EMSCRIPTEN__
        ImGui::Text("define: __EMSCRIPTEN__");
        ImGui::Text("Emscripten: %d.%d.%d", __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__);
#endif
#ifdef IMGUI_HAS_VIEWPORT
        ImGui::Text("define: IMGUI_HAS_VIEWPORT");
#endif
#ifdef IMGUI_HAS_DOCK
        ImGui::Text("define: IMGUI_HAS_DOCK");
#endif
        ImGui::Separator();
        ImGui::Text("io.BackendPlatformName: %s", io.BackendPlatformName ? io.BackendPlatformName : "NULL");
        ImGui::Text("io.BackendRendererName: %s", io.BackendRendererName ? io.BackendRendererName : "NULL");
        ImGui::Text("io.ConfigFlags: 0x%08X", io.ConfigFlags);
        if (io.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard)        ImGui::Text(" NavEnableKeyboard");
        if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad)         ImGui::Text(" NavEnableGamepad");
        if (io.ConfigFlags & ImGuiConfigFlags_NoMouse)                  ImGui::Text(" NoMouse");
        if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)      ImGui::Text(" NoMouseCursorChange");
        if (io.ConfigFlags & ImGuiConfigFlags_NoKeyboard)               ImGui::Text(" NoKeyboard");
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)            ImGui::Text(" DockingEnable");
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)          ImGui::Text(" ViewportsEnable");
        if (io.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)  ImGui::Text(" DpiEnableScaleViewports");
        if (io.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleFonts)      ImGui::Text(" DpiEnableScaleFonts");
        if (io.MouseDrawCursor)                                         ImGui::Text("io.MouseDrawCursor");
        if (io.ConfigViewportsNoAutoMerge)                              ImGui::Text("io.ConfigViewportsNoAutoMerge");
        if (io.ConfigViewportsNoTaskBarIcon)                            ImGui::Text("io.ConfigViewportsNoTaskBarIcon");
        if (io.ConfigViewportsNoDecoration)                             ImGui::Text("io.ConfigViewportsNoDecoration");
        if (io.ConfigViewportsNoDefaultParent)                          ImGui::Text("io.ConfigViewportsNoDefaultParent");
        if (io.ConfigDockingNoSplit)                                    ImGui::Text("io.ConfigDockingNoSplit");
        if (io.ConfigDockingWithShift)                                  ImGui::Text("io.ConfigDockingWithShift");
        if (io.ConfigDockingAlwaysTabBar)                               ImGui::Text("io.ConfigDockingAlwaysTabBar");
        if (io.ConfigDockingTransparentPayload)                         ImGui::Text("io.ConfigDockingTransparentPayload");
        if (io.ConfigMacOSXBehaviors)                                   ImGui::Text("io.ConfigMacOSXBehaviors");
        if (io.ConfigNavMoveSetMousePos)                                ImGui::Text("io.ConfigNavMoveSetMousePos");
        if (io.ConfigNavCaptureKeyboard)                                ImGui::Text("io.ConfigNavCaptureKeyboard");
        if (io.ConfigInputTextCursorBlink)                              ImGui::Text("io.ConfigInputTextCursorBlink");
        if (io.ConfigWindowsResizeFromEdges)                            ImGui::Text("io.ConfigWindowsResizeFromEdges");
        if (io.ConfigWindowsMoveFromTitleBarOnly)                       ImGui::Text("io.ConfigWindowsMoveFromTitleBarOnly");
        if (io.ConfigMemoryCompactTimer >= 0.0f)                        ImGui::Text("io.ConfigMemoryCompactTimer = %.1f", io.ConfigMemoryCompactTimer);
        ImGui::Text("io.BackendFlags: 0x%08X", io.BackendFlags);
        if (io.BackendFlags & ImGuiBackendFlags_HasGamepad)             ImGui::Text(" HasGamepad");
        if (io.BackendFlags & ImGuiBackendFlags_HasMouseCursors)        ImGui::Text(" HasMouseCursors");
        if (io.BackendFlags & ImGuiBackendFlags_HasSetMousePos)         ImGui::Text(" HasSetMousePos");
        if (io.BackendFlags & ImGuiBackendFlags_PlatformHasViewports)   ImGui::Text(" PlatformHasViewports");
        if (io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport)ImGui::Text(" HasMouseHoveredViewport");
        if (io.BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset)   ImGui::Text(" RendererHasVtxOffset");
        if (io.BackendFlags & ImGuiBackendFlags_RendererHasViewports)   ImGui::Text(" RendererHasViewports");
        ImGui::Separator();
        ImGui::Text("io.Fonts: %d fonts, Flags: 0x%08X, TexSize: %d,%d", io.Fonts->Fonts.Size, io.Fonts->Flags, io.Fonts->TexWidth, io.Fonts->TexHeight);
        ImGui::Text("io.DisplaySize: %.2f,%.2f", io.DisplaySize.x, io.DisplaySize.y);
        ImGui::Text("io.DisplayFramebufferScale: %.2f,%.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        ImGui::Separator();
        ImGui::Text("style.WindowPadding: %.2f,%.2f", style.WindowPadding.x, style.WindowPadding.y);
        ImGui::Text("style.WindowBorderSize: %.2f", style.WindowBorderSize);
        ImGui::Text("style.FramePadding: %.2f,%.2f", style.FramePadding.x, style.FramePadding.y);
        ImGui::Text("style.FrameRounding: %.2f", style.FrameRounding);
        ImGui::Text("style.FrameBorderSize: %.2f", style.FrameBorderSize);
        ImGui::Text("style.ItemSpacing: %.2f,%.2f", style.ItemSpacing.x, style.ItemSpacing.y);
        ImGui::Text("style.ItemInnerSpacing: %.2f,%.2f", style.ItemInnerSpacing.x, style.ItemInnerSpacing.y);

        if (copy_to_clipboard)
        {
            ImGui::LogText("\n```\n");
            ImGui::LogFinish();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}