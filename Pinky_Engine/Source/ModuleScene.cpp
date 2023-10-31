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

	// TODO: borrar cuando ya no se use
	GameObject* go = new GameObject("cube", rootNode);
		GameObject* go2 = new GameObject("cube2", go);
			GameObject* go7 = new GameObject("cube7", go2);
			GameObject* go9 = new GameObject("cube9", go2);
		GameObject* go3 = new GameObject("cube3", go);
			GameObject* go5 = new GameObject("cube5", go3);
			GameObject* go6 = new GameObject("cube6", go3);

	GameObject* go1 = new GameObject("cube1", rootNode);
		//GameObject* go10 = new GameObject("cube10", go1);

	GameObject* go4 = new GameObject("cube1", rootNode);
	
	//GameObject* go8 = new GameObject(go);
	//GameObject* go11 = new GameObject("cube11", rootNode);

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


	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN /* ||
		App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN*/)
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
	ai::ImportFile("..\\Game\\Assets\\3dObject\\baker_house\\BakerHouse.fbx");
	for each (ai::mesh * i in App->renderer3D->meshes)
	{
		i->tex.tex_id = ai::ImportTexture("..\\Game\\Assets\\3dObject\\baker_house\\Baker_house.dds");;
	}
}
