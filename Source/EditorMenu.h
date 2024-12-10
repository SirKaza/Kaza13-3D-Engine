#pragma once
#include "Globals.h"
#include "ConsoleEditor.h"

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

    update_status Draw();
	void showAboutWindow(bool* p_open);
	void showConfigurationWindow(bool* p_open);
    void showCaps() const;

    void setWindowFlags(WindowFlags others) { windowFlags = others; }
    bool getVsync() const { return windowFlags.vsync; }

private:
    WindowFlags windowFlags;
};

// global var
extern ConsoleEditor myConsole;
