#include "Globals.h"
#include "Application.h"
#include "ModuleFS.h"

#include "../SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "../PhysFS/include/physfs.h"
#include <fstream>
#include <filesystem>
#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

ModuleFS::ModuleFS(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	// needs to be created before Init so other modules can use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(nullptr);
	SDL_free(base_path);

	//Setting the working directory as the writing directory
	if (PHYSFS_setWriteDir(".") == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	AddPath("."); //Adding ProjectFolder (working directory)
	AddPath("Assets");
	CreateLibraryDirectories();
}

// Destructor
ModuleFS::~ModuleFS()
{}

// Called before render is available
bool ModuleFS::Init()
{
	LOG("Creating File System");
	bool ret = true;

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleFS::PreUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

update_status ModuleFS::Update(float dt)
{
	

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleFS::PostUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	

	return ret;
}

// Called before quitting
bool ModuleFS::CleanUp()
{
	LOG("Destroying 3D Renderer");
	
	RELEASE(rootNode);
	return true;
}