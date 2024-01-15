#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"

#include "ImGuiWindows.h"
#include "ImGuiResourcesWindows.h"

class GameObject;
class G_UI;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	G_UI* GetCanvas();
	void SetCanvas(G_UI* newCanvas = nullptr);

private:
	void ImportDefaultScene();

public:
	GameObject* rootNode;

	Hierarchy* hierarchy;
	Inspector* inspector;
	Console* console;
	ProjectFiles* project;
	ResourcesManager* rm;

	GameObject* mainCamera;

	//
	bool crossHair;
	G_UI* f1;

private:
	G_UI* canvas;
};