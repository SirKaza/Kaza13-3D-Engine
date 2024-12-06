#pragma once
#include "Globals.h"

class LogEditor;
class ConsoleEditor;

struct ImGuiWindowData
{
    bool showLog = false;
    bool ShowConsole = false;
    bool ShowConfiguration = false;
    bool ShowAbout = false;
};

class EditorMenu
{
public:
    EditorMenu();
    ~EditorMenu();

    void Draw();
	void showAboutWindow(bool* p_open);
	void showConfigurationWindow(bool* p_open);
    void showCaps() const;

    void setWindowFlags(WindowFlags others) { windowFlags = others; }
    bool getVsync() const { return windowFlags.vsync; }

private:
    WindowFlags windowFlags;
};
extern LogEditor myLog;
extern ConsoleEditor myConsole;
