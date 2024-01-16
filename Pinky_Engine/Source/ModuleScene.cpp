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
	f1 = nullptr;

	//
	crossHair = false;
	street = true;
	mainScreen = 0;
	loaded = false;
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

	LoadFirstScene();

	//
	App->resource->ImportToSceneV("street2.fbx", PINKY_ASSETS_AUX "3dObject\\street\\");
	App->parson->SaveScene("Street");

	G_UI* UI_Element = new G_UI(UI_TYPE::CANVAS);

	G_UI* UI_Element2 = new G_UI(UI_TYPE::IMAGE, rootNode, 0, 0, 784, 660);
	App->resource->ImportTextureToModel("intro_bg.png", PINKY_ASSETS_AUX "UI\\", *UI_Element2);

	G_UI* UI_Element4 = new G_UI(UI_TYPE::BUTTON, rootNode, 300, 300, 100, 100);
	App->resource->ImportTextureToModel("button.png", PINKY_ASSETS_AUX "UI\\", *UI_Element4);
	static_cast<UI_Button*> (UI_Element4->GetComponentUI(UI_TYPE::BUTTON))->defaultFunction = true;

	UI_Element = nullptr;
	UI_Element2 = nullptr;
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

	// Delete GameObject
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

		if (canvas == nullptr)
		{
			mainScreen = 0;
		}
	}

	// Open menu
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN && !ImGui::GetIO().WantTextInput && !App->input->GetInputActive())
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

	// TODO: 
	ImportDefaultScene();

	if (street)
	{
		rootNode->DeleteChild(rootNode->vChildren[1]);
		street = false;
	}
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

void ModuleScene::LoadFirstScene()
{
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

	C_Camera* cam = (C_Camera*)App->scene->mainCamera->GetComponentByType(C_TYPE::CAM);
	cam->isGameCam = true;
	cam->SetAsMain(cam->isGameCam);
	cam = nullptr;

	App->parson->SaveScene("MainScreen");

	//
	App->resource->ImportToSceneV("street2.fbx", PINKY_ASSETS_AUX "3dObject\\street\\");
	App->parson->SaveScene("Street");

	G_UI* UI_Element = new G_UI(UI_TYPE::CANVAS);

	G_UI* UI_Element2 = new G_UI(UI_TYPE::IMAGE, rootNode, 0, 0, 784, 660);
	App->resource->ImportTextureToModel("intro_bg.png", PINKY_ASSETS_AUX "UI\\", *UI_Element2);

	G_UI* UI_Element4 = new G_UI(UI_TYPE::BUTTON, rootNode, 300, 300, 100, 100);
	App->resource->ImportTextureToModel("button.png", PINKY_ASSETS_AUX "UI\\", *UI_Element4);
	static_cast<UI_Button*> (UI_Element4->GetComponentUI(UI_TYPE::BUTTON))->defaultFunction = true;

	UI_Element = nullptr;
	UI_Element2 = nullptr;
	UI_Element4 = nullptr;
	mainScreen = 1;
}

void ModuleScene::ImportDefaultMainScreen()
{
	App->parson->LoadScene(ASSETS_AUX "MainScreen.pnk");

	G_UI* UI_Element = new G_UI(UI_TYPE::CANVAS);

	G_UI* UI_Element2 = new G_UI(UI_TYPE::IMAGE, rootNode, 0, 0, 784, 660);
	App->resource->ImportTextureToModel("intro_bg.png", PINKY_ASSETS_AUX "UI\\", *UI_Element2);

	G_UI* UI_Element4 = new G_UI(UI_TYPE::BUTTON, rootNode, 300, 300, 100, 100);
	App->resource->ImportTextureToModel("button.png", PINKY_ASSETS_AUX "UI\\", *UI_Element4);
	static_cast<UI_Button*> (UI_Element4->GetComponentUI(UI_TYPE::BUTTON))->defaultFunction = true;

	UI_Element = nullptr;
	UI_Element2 = nullptr;
	UI_Element4 = nullptr;
	mainScreen = 1;
}

void ModuleScene::ImportDefaultScene()
{
	if (crossHair)
	{
		if (!loaded)
		{
			App->resource->ImportToSceneV("Street.pnk", ASSETS_AUX);
			loaded = true;
		}

		if (!App->resource->pendingToLoadScene)
		{
			crossHair = false;
			G_UI* UI_Element = new G_UI(UI_TYPE::IMAGE, rootNode, App->editor->gameViewSize.x / 2 - 150, App->editor->gameViewSize.y / 2 - 150, 300, 300);
			App->resource->ImportTextureToModel("crosshair.png", PINKY_ASSETS_AUX "UI\\", *UI_Element);
			
			G_UI* UI_Element0 = new G_UI(UI_TYPE::CANVAS, rootNode, App->editor->gameViewSize.x / 2 - 150, App->editor->gameViewSize.y / 2 - 150, 300, 300);

			G_UI* UI_Element2 = new G_UI(UI_TYPE::IMAGE, UI_Element0, App->editor->gameViewSize.x / 2 - 300, App->editor->gameViewSize.y / 2 - 300, 600, 600);
			App->resource->ImportTextureToModel("crosshair.png", PINKY_ASSETS_AUX "UI\\", *UI_Element);

			// VSync
			G_UI* UI_Element3 = new G_UI(UI_TYPE::CHECKBOX, UI_Element2, 200, 200);
			App->resource->ImportTextureToModel("checkboxBG.png", PINKY_ASSETS_AUX "UI\\", *(static_cast<UI_Checkbox*>
				(UI_Element3->GetComponentUI(UI_TYPE::CHECKBOX))->bgImg->gameObject));
			App->resource->ImportTextureToModel("checkmark.png", PINKY_ASSETS_AUX "UI\\", *(static_cast<UI_Checkbox*>
				(UI_Element3->GetComponentUI(UI_TYPE::CHECKBOX))->cmImg->gameObject));
			static_cast<UI_Checkbox*>(UI_Element3->GetComponentUI(UI_TYPE::CHECKBOX))->defaultFunction1 = true;
			static_cast<UI_Checkbox*>(UI_Element3->GetComponentUI(UI_TYPE::CHECKBOX))->displayText->text = "VSync";
			UI_Element3->isActive = false;

			// Draggeable
			G_UI* UI_Element4 = new G_UI(UI_TYPE::CHECKBOX, UI_Element2, 200, 500);
			App->resource->ImportTextureToModel("checkboxBG.png", PINKY_ASSETS_AUX "UI\\", *(static_cast<UI_Checkbox*>
				(UI_Element4->GetComponentUI(UI_TYPE::CHECKBOX))->bgImg->gameObject));
			App->resource->ImportTextureToModel("checkmark.png", PINKY_ASSETS_AUX "UI\\", *(static_cast<UI_Checkbox*>
				(UI_Element4->GetComponentUI(UI_TYPE::CHECKBOX))->cmImg->gameObject));
			static_cast<UI_Checkbox*>(UI_Element4->GetComponentUI(UI_TYPE::CHECKBOX))->defaultFunction2 = true;
			static_cast<UI_Checkbox*>(UI_Element4->GetComponentUI(UI_TYPE::CHECKBOX))->displayText->text = "Draggeable";

			G_UI* UI_Element5 = new G_UI(UI_TYPE::TEXT, UI_Element2, App->editor->gameViewSize.x / 2 - 300, App->editor->gameViewSize.y / 2 - 300);
			static_cast<UI_Text*>(UI_Element5->GetComponentUI(UI_TYPE::TEXT))->text = "MENU";

			//
			inspector->SetActiveState(UI_Element0, false);
			UI_Element0->isActive = false;

			//
			UI_Element = nullptr;
			UI_Element2 = nullptr;
			UI_Element3 = nullptr;
			UI_Element4 = nullptr;
			UI_Element5 = nullptr;

			mainScreen = 2;
		}
	}
}
