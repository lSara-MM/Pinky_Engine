#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "ImGuiWindows.h"

#include "ModuleResource.h"

#include "External Libraries\SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Source/External Libraries/Glew/libx86/glew32.lib")

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"

#include "GameObject.h"
#include "G_UI.h"
#include "UI_Canvas.h"
#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_CheckBox.h"


ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	hierarchy = nullptr;
	inspector = nullptr;
	console = nullptr;
	project = nullptr;
	rm = nullptr;

	canvas = nullptr;
}

// Destructor
ModuleScene::~ModuleScene()
{}

// Called before render is available
bool ModuleScene::Init()
{
	LOG("Creating scene");
	bool ret = true;

	// Windows
	hierarchy = new Hierarchy(0);
	App->editor->AddWindow(hierarchy);

	inspector = new Inspector(1);
	App->editor->AddWindow(inspector);

	rm = new ResourcesManager(2);
	App->editor->AddWindow(rm);

	console = new Console(3);
	App->editor->AddWindow(console);

	project = new ProjectFiles(4);
	App->editor->AddWindow(project);

	return ret;
}

bool ModuleScene::Start()
{
	LOG("Starting scene");
	rootNode = new GameObject("Scene", nullptr);

	std::array<std::string, 4> components = { "Cube", "Sphere", "Cylinder", "Plane" };
	for (int i = 0; i < components.size(); i++)
	{
		ai::CreatePolyPrimitive(ai::POLY_PRIMITIVE_TYPE(i));
	}

	RELEASE(rootNode);
	rootNode = new GameObject("Scene", nullptr);

	//Main Camera  
	mainCamera = new GameObject("Main Camera", rootNode);
	float3 posIni = { 47, 25, -55 };
	float3 rotIni = { 28, -38, 21 };
	mainCamera->transform->SetPosition(posIni);
	mainCamera->transform->SetRotation(rotIni);
	mainCamera->AddComponent(C_TYPE::CAM);

	App->resource->ImportToSceneV("street2.fbx", PINKY_ASSETS_AUX "3dObject\\street\\");
	//App->resource->ImportToScene("Baker_house.dds", PINKY_ASSETS_AUX "3dObject\\baker_house\\");

	G_UI* UI_Element = new G_UI(UI_TYPE::CANVAS);
	G_UI* UI_Element2 = new G_UI(UI_TYPE::IMAGE, App->scene->rootNode, 784, 660);

	App->resource->ImportTextureToModel("intro_bg.png", PINKY_ASSETS_AUX "UI\\", *UI_Element2);

	// VSync
	G_UI* UI_Element3 = new G_UI(UI_TYPE::CHECKBOX, App->scene->rootNode, 100, 100, 300, 300);
	App->resource->ImportTextureToModel("checkboxBG.png", PINKY_ASSETS_AUX "UI\\", *(static_cast<UI_Checkbox*>
		(UI_Element3->GetComponentUI(UI_TYPE::CHECKBOX))->bgImg->gameObject));
	App->resource->ImportTextureToModel("checkmark.png", PINKY_ASSETS_AUX "UI\\", *(static_cast<UI_Checkbox*>
		(UI_Element3->GetComponentUI(UI_TYPE::CHECKBOX))->cmImg->gameObject));
	static_cast<UI_Checkbox*>(UI_Element3->GetComponentUI(UI_TYPE::CHECKBOX))->defaultFunction1 = true;


	G_UI* UI_Element4 = new G_UI(UI_TYPE::BUTTON, App->scene->rootNode, 100, 100, 300, 300);
	App->resource->ImportTextureToModel("checkboxBG.png", PINKY_ASSETS_AUX "UI\\", *UI_Element4);

	static_cast<UI_Button*> (UI_Element4->GetComponentUI(UI_TYPE::BUTTON))->defaultFunction = true;

	UI_Element = nullptr;
	UI_Element2 = nullptr;
	UI_Element3 = nullptr;
	UI_Element4 = nullptr;

	return true;
}

// PreUpdate: clear buffer
update_status ModuleScene::PreUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	rootNode->Update(dt);

	if (!hierarchy->GetSelectedGOs().empty())
	{
		if (hierarchy->GetSelectedGOs() != inspector->GetSelectedGOs())
		{
			inspector->SetSelected(hierarchy->GetSelectedGOs());
		}
	}


	if ((App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) && !ImGui::GetIO().WantTextInput && !App->input->GetInputActive())
	{
		// Delete all selected go
		if (!hierarchy->GetSelectedGOs().empty())
		{
			for (auto j = 0; j < hierarchy->GetSelectedGOs().size(); j++)
			{
				hierarchy->GetSelectedGOs()[j]->pParent->DeleteChild(hierarchy->GetSelectedGOs()[j]);
			}

			hierarchy->SetSelected(nullptr);
			inspector->SetSelected(nullptr);
		}
	}

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleScene::PostUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	return ret;
}

// Called before quitting
bool ModuleScene::CleanUp()
{
	LOG("Destroying Scene");

	hierarchy = nullptr;
	inspector = nullptr;
	console = nullptr;
	project = nullptr;
	rm = nullptr;

	RELEASE(rootNode);
	return true;
}

G_UI* ModuleScene::GetCanvas()
{
	return canvas;
}

void ModuleScene::SetCanvas(G_UI* newCanvas)
{
	canvas = newCanvas;
}
