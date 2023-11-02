#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "ImGuiWindows.h"
#include "../SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"

#ifdef _DEBUG
#pragma comment (lib, "MathGeoLib/libx86/libDebug/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#else
#pragma comment (lib, "MathGeoLib/libx86/libRelease/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#endif // _DEBUG

#include "GameObject.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleScene::~ModuleScene()
{}

// Called before render is available
bool ModuleScene::Init()
{
	LOG("Creating scene");
	bool ret = true;

	rootNode = new GameObject("Root node", nullptr);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleScene::PreUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	rootNode->Update(dt);

	if (!h->GetSelectedGOs().empty())
	{
		if (h->GetSelectedGOs() != i->GetSelectedGOs())
		{
			i->SetSelected(h->GetSelectedGOs());
		}
	}


	if ((App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN ||
		App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) && !ImGui::GetIO().WantTextInput)
	{
		// Delete all selected go
		if (!h->GetSelectedGOs().empty())
		{
			for (auto j = 0; j < h->GetSelectedGOs().size(); j++)
			{
				h->GetSelectedGOs()[j]->pParent->DeleteChild(h->GetSelectedGOs()[j]);
			}

			h->SetSelected(nullptr);
			i->SetSelected(nullptr);
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
	LOG("Destroying 3D Renderer");
	
	RELEASE(rootNode);
	return true;
}

void ModuleScene::BakerHouse()
{
	ai::ImportFile("Assets\\3dObject\\baker_house\\BakerHouse.fbx");
	ai::ImportFile("Assets\\3dObject\\baker_house\\Baker_house.dds");

	// TODO: remove when copy wip done
	//GameObject* go8 = new GameObject(rootNode->vChildren.back(), rootNode->vChildren.back()->vChildren.size(), rootNode->vChildren.back()->pParent);
	//go8 = nullptr;
}
