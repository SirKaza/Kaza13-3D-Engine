#pragma once
#include "Globals.h"
#include "ConsoleEditor.h"

struct ImGuiWindowData
{
    bool showLog = false;
    bool showConsole = false;
    bool showConfiguration = false;
    bool showProperties = false;
    bool showAbout = false;
};

class EditorMenu
{
public:
    EditorMenu();
    ~EditorMenu();

    update_status Draw();
	void showAboutWindow(bool* p_open) const;
	void showConfigurationWindow(bool* p_open);
    void showPropertiesWindow(bool* p_open) const;
    void showCaps() const;

    void setWindowFlags(WindowFlags others) { windowFlags = others; }
    void setDisplayWH(int w, int h) { width = w; height = h; }

private:
    WindowFlags windowFlags;
    int width, height;
};

// global var
extern ConsoleEditor myConsole;
