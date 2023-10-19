#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
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
	
	rootNode = new GameObject("Root node", true);		
	GO_num = 0;

	// TODO: borrar cuando ya no se use
	GameObject* go = new GameObject("cube", rootNode);
	GameObject* go1 = new GameObject("cube1", rootNode);
	GameObject* go2 = new GameObject("cube2", go);
	GameObject* go3 = new GameObject("cube3", go);
	GameObject* go4 = new GameObject("cube4", rootNode);
	GameObject* go5 = new GameObject("cube5", go3);
	GameObject* go6 = new GameObject("cube6", go3);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleScene::PreUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
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