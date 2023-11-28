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

void ModuleResource::ImportFile(const char* fileDir)
{
	std::string dir = fileDir;
	std::array<std::string, 3> obj_ext = { ".fbx", ".FBX", ".obj", };
	std::array<std::string, 6> tex_ext = { ".png", ".PNG", ".jpg", ".JPG", ".dds", ".DDS" };

	for (auto i = 0; i < obj_ext.size(); i++)
	{
		if (dir.find(obj_ext.at(i)) != std::string::npos)
		{
			ai::ImportMesh(fileDir);
			break;
		}
	}

	for (auto i = 0; i < tex_ext.size(); i++)
	{
		if (dir.find(tex_ext.at(i)) != std::string::npos)
		{
			for each (ai::mesh * i in App->renderer3D->meshes)
			{
				ai::ImportTexture(i, fileDir);
			}
		}
	}
}

bool ModuleResource::SaveToLibrary(Resource* r)
{
	std::string path;

	char* buffer = nullptr;
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

	path += std::to_string(r->GetUID());
	path += ".pnk";

	App->fs->Save(path.c_str(), buffer, size);
	
	//RELEASE_ARRAY(buffer);
	return false;
}
