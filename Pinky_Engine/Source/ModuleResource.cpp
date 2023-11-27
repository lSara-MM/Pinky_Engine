#include "Globals.h"
#include "Application.h"
#include "ModuleResource.h"
#include "External Libraries/MathGeoLib/include/Math/Quat.h"
#include "External Libraries/MathGeoLib/include/Math/float3.h"
#include "External Libraries/MathGeoLib/include/Math/float4x4.h"
#include "C_Camera.h"


ModuleResource::ModuleResource(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleResource::~ModuleResource()
{
	
}

// -----------------------------------------------------------------
bool ModuleResource::Start()
{
	return true;
}

// -----------------------------------------------------------------
update_status ModuleResource::Update(float dt)
{

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
bool ModuleResource::CleanUp()
{
	LOG("Cleaning resources");

	return true;
}

bool ModuleResource::ImportFile(const char* path)
{
	bool ret = false;
	//std::string normPath = App->fs->NormalizePath(path);
	std::string finalPath;

	if (App->fs->DuplicateFile(path, "Assets/", finalPath))
	{
		ret = true;	
	}

	return ret;
}
