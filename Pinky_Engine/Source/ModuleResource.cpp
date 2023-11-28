#include "Globals.h"
#include "Application.h"
#include "ModuleResource.h"

#include "FileSystemManager.h"

#include "Resource.h"
#include "I_Mesh.h"
#include "I_Texture.h"

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

	/*switch ()
	{
	case R_TYPE::MESH:
		path = MESHES_PATH;
		break;
	case R_TYPE::TEXTURE:
		path = TEXTURES_PATH;
		break;
	case R_TYPE::SCENE:
		break;
	case R_TYPE::NONE:
		break;
	default:
		break;
	}*/

	return ret;
}

bool ModuleResource::SaveToLibrary(Resource* r)
{
	std::string path;

	char* buffer;
	uint size = 0;

	switch (r->GetType())
	{
	case R_TYPE::MESH:
		path = MESHES_PATH;
		size = I_Mesh::Save(static_cast<R_Mesh*>(r), &buffer);
		break;
	case R_TYPE::TEXTURE:
		path = TEXTURES_PATH;
		size = I_Texture::Save(static_cast<R_Texture*>(r), &buffer);
		break;
	case R_TYPE::SCENE:
		break;
	case R_TYPE::NONE:
		break;
	default:
		break;
	}

	App->fs->Save(path.c_str(), buffer, size);
	return false;
}
