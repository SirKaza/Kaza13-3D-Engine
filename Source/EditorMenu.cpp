#include "EditorMenu.h"
#include <imgui.h>
#include "Globals.h"
#include "FrameData.h"
#include "ConsoleEditor.h"
#include "LogEditor.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleOpenGL.h"
#include "ModuleTexture.h"
#include "SDL.h"
#include <GL/glew.h>
#include "DirectXTex.h"

// global variables
ConsoleEditor myConsole;

EditorMenu::EditorMenu()
{
}

EditorMenu::~EditorMenu()
{
}

update_status EditorMenu::Draw()
{
    static ImGuiWindowData editor_data;
    
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Console", "", editor_data.ShowConsole)) { editor_data.ShowConsole = !editor_data.ShowConsole; }
            if (ImGui::MenuItem("Configuration", "", editor_data.ShowConfiguration)) { editor_data.ShowConfiguration = !editor_data.ShowConfiguration; }
            ImGui::Separator();
            char menuText[64];
            std::snprintf(menuText, sizeof(menuText), "About %s", ENGINE_NAME);
            if (ImGui::MenuItem(menuText, "", editor_data.ShowAbout)) { editor_data.ShowAbout = !editor_data.ShowAbout; }
            if (ImGui::MenuItem("Quit")) { return UPDATE_STOP; }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (editor_data.ShowConsole) myConsole.Draw("Console", &editor_data.ShowConsole);
    if (editor_data.ShowConfiguration) showConfigurationWindow(&editor_data.ShowConfiguration);
    if (editor_data.ShowAbout) showAboutWindow(&editor_data.ShowAbout);
    return UPDATE_CONTINUE;
}

void EditorMenu::showConfigurationWindow(bool* p_open)
{
    if (!ImGui::Begin("Configuration", p_open))
    {
        ImGui::End();
        return;
    }

    static ImGuiSliderFlags flags = ImGuiSliderFlags_None;
    const ImGuiSliderFlags flags_for_sliders = flags & ~ImGuiSliderFlags_WrapAround;

    int refreshRate = App->GetWindow()->getRefreshRate();

    ImGui::Text("Options");
    if (ImGui::CollapsingHeader("Application"))
    {
        // Some info
        ImGui::InputText("App Name", (char*)ENGINE_NAME, IM_ARRAYSIZE(ENGINE_NAME), ImGuiInputTextFlags_ReadOnly);
        ImGui::InputText("Organization", (char*)ORGANIZATION_NAME, IM_ARRAYSIZE(ORGANIZATION_NAME), ImGuiInputTextFlags_ReadOnly);

        // FPS Slider
        static int slider_FPS = 0;
        ImGui::SliderInt("Max FPS", &slider_FPS, 0, refreshRate, "%d", flags_for_sliders);
        if (slider_FPS == 0) {
            if (windowFlags.vsync) maxFPS = refreshRate;
            else maxFPS = 0;
        }
        else maxFPS = slider_FPS;

        ImGui::Text("Limit Framerate: ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", maxFPS);

        // FPS & MS graph
        char title[25]; // Title text
        float maxYAxis;
        if (!windowFlags.vsync && slider_FPS == 0) maxYAxis = 500 * 1.66f;
        else maxYAxis = maxFPS * 1.66f;
        sprintf_s(title, sizeof(title), "Framerate %.1f", fps_log[fps_log.size() - 1]);
        ImGui::PlotHistogram("##framerate", &fps_log[0], (int)fps_log.size(), 0, title, 0.0f, maxYAxis, ImVec2(310, 100));
        sprintf_s(title, sizeof(title), "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
        ImGui::PlotHistogram("##milliseconds", &ms_log[0], (int)ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
    }

    if (ImGui::CollapsingHeader("Window"))
    {
        // Bright, FOV, width and height
        ModuleWindow* windowModule = App->GetWindow();
        static float slider_brightness = windowModule->getBrightness();
        static int slider_fov = App->GetCamera()->getFOV();
        int slider_width = windowModule->getScreenSurface()->w;
        int slider_height = windowModule->getScreenSurface()->h;

        bool brightnessChanged = ImGui::SliderFloat("Brightness", &slider_brightness, 0.0f, 1.0f, "%.3f", flags_for_sliders);
        bool fovChanged = ImGui::SliderInt("FOV", &slider_fov, 0, 120, "%d", flags_for_sliders);
        bool widthChanged = ImGui::SliderInt("Width", &slider_width, 0, windowModule->getDisplayW(), "%d", flags_for_sliders);
        bool heightChanged = ImGui::SliderInt("Height", &slider_height, 0, windowModule->getDisplayH(), "%d", flags_for_sliders);

        if (brightnessChanged) windowModule->setBrightness(slider_brightness);
        if (fovChanged) App->GetCamera()->setFOV(slider_fov);
        if (widthChanged || heightChanged) {
            windowModule->setWindowSize(slider_width, slider_height);
            App->GetOpenGL()->WindowResized(slider_width, slider_height, SDL_GetWindowID(windowModule->window));
        }

        // Refresh rate
        ImGui::Text("Refresh Rate: ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", refreshRate);

        // Window checkboxs
        if (ImGui::BeginTable("split", 2))
        {
            ImGui::TableNextColumn(); 
            if(ImGui::Checkbox("Fullscreen", &windowFlags.fullscreen))
                App->GetWindow()->setFullscreen(windowFlags.fullscreen);
            ImGui::TableNextColumn(); 
            if (ImGui::Checkbox("Borderless", &windowFlags.borderless))
                App->GetWindow()->setBorderless(windowFlags.borderless);
            ImGui::TableNextColumn(); 
            if (ImGui::Checkbox("Resizable", &windowFlags.resizable))
                App->GetWindow()->setResizable(windowFlags.resizable);
            ImGui::TableNextColumn(); 
            if (ImGui::Checkbox("Full Desktop", &windowFlags.fullDesktop))
                App->GetWindow()->setFullDesktop(windowFlags.fullDesktop);
            ImGui::TableNextColumn();
            if (ImGui::Checkbox("Vsync", &windowFlags.vsync))
                App->GetWindow()->setVsync(windowFlags.vsync);
            ImGui::EndTable();
        }
    }

    if (ImGui::CollapsingHeader("Texture"))
    {
        ModuleTexture* texture = App->GetTexture();
        const DirectX::ScratchImage& image = texture->getScratchImage();
        const DirectX::TexMetadata& metadata = image.GetMetadata();

        // Display texture info
        ImGui::Text("Width:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", metadata.width);

        ImGui::Text("Height:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", metadata.height);

        ImGui::Text("Format:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", texture->DXGIFormatToString(metadata.format));

        // WRAP modes
        static int wrapMode = texture->getWrapMode();
        const char* wrapModes[] = { "CLAMP_TO_BORDER", "CLAMP", "REPEAT", "MIRRORED_REPEAT" };
        if (ImGui::Combo("WRAP Mode", &wrapMode, wrapModes, IM_ARRAYSIZE(wrapModes)))
        {
            switch (wrapMode)
            {
            case WrapMode::CLAMP_TO_BORDER:
                texture->setWrapMode(GL_CLAMP_TO_BORDER);
                break;
            case WrapMode::CLAMP:
                texture->setWrapMode(GL_CLAMP);
                break;
            case WrapMode::REPEAT:
                texture->setWrapMode(GL_REPEAT);
                break;
            case WrapMode::MIRRORED_REPEAT:
                texture->setWrapMode(GL_MIRRORED_REPEAT);
                break;
            default:
                ENGINE_LOG("Error changing Wrap Mode %d", wrapMode);
                break;
            }
        }

        // MAG filters
        static int magFilter = texture->getMagFilter();
        const char* magFilterModes[] = { "NEAREST", "LINEAR" };
        if (ImGui::Combo("MAG Filter", &magFilter, magFilterModes, IM_ARRAYSIZE(magFilterModes)))
        {
            texture->setMagFilter(magFilter == 0 ? GL_NEAREST : GL_LINEAR);
        }

        // MIN filters
        static int minFilter = texture->getMinFilter();
        static bool useMipmaps = texture->getUseMipMaps();

        if (!useMipmaps)
        {
            const char* minFilterModes[] = { "NEAREST", "LINEAR" };
            if (ImGui::Combo("MIN Filter", &minFilter, minFilterModes, IM_ARRAYSIZE(minFilterModes)))
            {
                texture->setMinFilter(minFilter == 0 ? GL_NEAREST : GL_LINEAR);
            }
        }
        else
        {
            const char* minFilterModes[] = { "NEAREST_MIPMAP_NEAREST", "LINEAR_MIPMAP_NEAREST", "NEAREST_MIPMAP_LINEAR", "LINEAR_MIPMAP_LINEAR" };
            if (ImGui::Combo("MIN Filter", &minFilter, minFilterModes, IM_ARRAYSIZE(minFilterModes)))
            {
                switch (minFilter)
                {
                case MipMapMode::NEAREST_MIPMAP_NEAREST:
                    texture->setMinFilter(GL_NEAREST_MIPMAP_NEAREST);
                    break;
                case MipMapMode::LINEAR_MIPMAP_NEAREST:
                    texture->setMinFilter(GL_LINEAR_MIPMAP_NEAREST);
                    break;
                case MipMapMode::NEAREST_MIPMAP_LINEAR:
                    texture->setMinFilter(GL_NEAREST_MIPMAP_LINEAR);
                    break;
                case MipMapMode::LINEAR_MIPMAP_LINEAR:
                    texture->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
                    break;
                default:
                    ENGINE_LOG("Error changing MipMap Mode %d", minFilter);
                    break;
                }
            }
        }

        // Mipmap toggle
        if (ImGui::Checkbox("Enable Mipmaps", &useMipmaps))
        {
            if (useMipmaps)
            {
                texture->setUseMipMaps();
                minFilter = 0;
                texture->setMinFilter(GL_NEAREST_MIPMAP_NEAREST);
            }
            else
            {
                minFilter = 0; // GL_NEAREST
                texture->setMinFilter(GL_NEAREST); // Disable mipmaps
            }
        }
    }

    if (ImGui::CollapsingHeader("Hardware"))
    {
        // SDL version
        SDL_version sdlVersion;
        SDL_GetVersion(&sdlVersion);
        ImGui::Text("SDL Version:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d.%d.%d", sdlVersion.major, sdlVersion.minor, sdlVersion.patch);

        // OpenGL version
        const char* version = (const char*)glGetString(GL_VERSION);
        ImGui::Text("OpenGL Version:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", version);

        ImGui::Separator(); // ----

        // CPU
        ImGui::Text("CPUs:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d (Cache: %dkb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());

        // RAM
        ImGui::Text("System RAM:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.2f GB", SDL_GetSystemRAM() / 1024.0f);

        ImGui::Text("Caps:");
        showCaps();

        ImGui::Separator(); // ----

        // GPU
        const char* vendor = (const char*) glGetString(GL_VENDOR);
        ImGui::Text("GPU:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", vendor);
        
        // GPU Brand
        const char* renderer = (const char*)glGetString(GL_RENDERER);
        ImGui::Text("Brand:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", renderer);

        GLint vramBudget, vramUsage, vramAvailable, vramReserved;

        if (glewIsSupported("GL_NVX_gpu_memory_info")) {
            glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &vramBudget);  // Total memory
            glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &vramAvailable); // Actual memory available
            glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &vramReserved);      // Reserved memory for GPU

            vramUsage = vramBudget - vramAvailable; // memory usage

            // VRAM Metrics in MB
            ImGui::Text("VRAM Budget:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.2f MB", vramBudget / 1024.0f);

            ImGui::Text("VRAM Usage:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.2f MB", vramUsage / 1024.0f);

            ImGui::Text("VRAM Available:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.2f MB", vramAvailable / 1024.0f);

            ImGui::Text("VRAM Reserved:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.2f MB", vramReserved / 1024.0f);
        }
    }
    ImGui::End();
}

void EditorMenu::showAboutWindow(bool* p_open)
{
    char menuText[64];
    std::snprintf(menuText, sizeof(menuText), "About %s", ENGINE_NAME);
    if (!ImGui::Begin(menuText, p_open))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("%s (%s)", ENGINE_NAME, ENGINE_VERSION);

    ImGui::TextLinkOpenURL("GitHub", "https://github.com/SirKaza/Kaza13-3D-Engine");
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("Readme", "https://github.com/SirKaza/Kaza13-3D-Engine/blob/master/README.md");
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("License", "https://github.com/SirKaza/Kaza13-3D-Engine/blob/master/LICENSE");
    ImGui::SameLine();
    ImGui::TextLinkOpenURL("Releases", "https://github.com/SirKaza/Kaza13-3D-Engine/releases");

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

void EditorMenu::showCaps() const
{
    if (SDL_HasRDTSC()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "RDTSC,");
    }
    if (SDL_HasAltiVec()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "AltiVec,");
    }
    if (SDL_HasMMX()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "MMX,");
    }
    if (SDL_Has3DNow()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "3DNow,");
    }
    if (SDL_HasSSE()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE,");
    }
    if (SDL_HasSSE2()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE2,");
    }
    if (SDL_HasSSE3()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE3,");
    }
    if (SDL_HasSSE41()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE41,");
    }
    if (SDL_HasSSE42()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE42,");
    }
    if (SDL_HasAVX()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "AVX,");
    }
    if (SDL_HasAVX2()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "AVX2,");
    }
    if (SDL_HasAVX512F()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "AVX512F,");
    }
    if (SDL_HasARMSIMD()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "ARMSIMD,");
    }
    if (SDL_HasNEON()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "NEON,");
    }
        
}