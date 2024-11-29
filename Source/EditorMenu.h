#pragma once

class LogEditor;
class ConsoleEditor;

struct ImGuiWindowData
{
    bool showLog = false;
    bool ShowConsole = false;
    bool ShowConfiguration = false;
    bool ShowAbout = false;
};

struct WindowFlags
{
    bool fullscreen = false;
    bool borderless = false;
    bool resizable = false;
    bool fullDesktop = false;
    bool vsync = false;

    bool operator!=(const WindowFlags& other) const {
        return fullscreen != other.fullscreen || borderless != other.borderless || resizable != other.resizable || fullDesktop != other.fullDesktop || vsync != other.vsync; 
    }
};

class EditorMenu
{
public:
    EditorMenu();
    ~EditorMenu();

    void Draw();
	void showAboutWindow(bool* p_open);
	void showConfigurationWindow(bool* p_open);
};
extern LogEditor myLog;
extern ConsoleEditor myConsole;
