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

class EditorMenu
{
public:
    EditorMenu();
    ~EditorMenu();

    void Draw();
	void showAboutWindow(bool* p_open);
	void showConfigurationWindow(bool* p_open);
    void showCaps() const;
};
extern LogEditor myLog;
extern ConsoleEditor myConsole;
