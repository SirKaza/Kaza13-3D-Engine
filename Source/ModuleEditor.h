#pragma once
#include "Module.h"
#include "EditorWindow.h"

// Forward declaration
struct ImGuiContext;
class EditorMenu;

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	ImGuiContext* getContext() const { return context; }
	EditorMenu* getEditorMenu() const { return menu; }

private:
	ImGuiContext* context;
	EditorMenu* menu;
};

