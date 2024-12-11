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
	~ModuleEditor() override;

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	bool CleanUp() override;

private:
	ImGuiContext* context;
	EditorMenu* menu;
};

