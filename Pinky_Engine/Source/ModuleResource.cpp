#include "Globals.h"
#include "Application.h"
#include "ModuleResource.h"

#include "FileSystemManager.h"
#include "GameObject.h"

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

GameObject* ModuleResource::ImportFile(const char* fileDir, GameObject* goToLink)
{
	std::string dir = fileDir;
	std::array<std::string, 3> obj_ext = { ".fbx", ".FBX", ".obj", };
	std::array<std::string, 6> tex_ext = { ".png", ".PNG", ".jpg", ".JPG", ".dds", ".DDS" };

	GameObject* go = nullptr;
	bool imported = false;

	for (auto i = 0; i < obj_ext.size(); i++)
	{
		if (dir.find(obj_ext.at(i)) != std::string::npos)
		{
			imported = true;
			go = ai::ImportMesh(fileDir);
			break;
		}
	}

	if (!imported)
	{
		for (auto i = 0; i < tex_ext.size(); i++)
		{
			if (dir.find(tex_ext.at(i)) != std::string::npos)
			{
				goToLink->mesh->mesh->tex->ImportTexture(fileDir);

				std::string path = App->resource->SaveToLibrary(goToLink->mesh->mesh->tex);

				/*R_Texture* tex = new R_Texture();
				tex->ImportTexture(fileDir);

				std::string path = App->resource->SaveToLibrary(tex);
				RELEASE(tex);

				goToLink->mesh->mesh->tex = static_cast<R_Texture*>(App->resource->LoadFromLibrary(path, R_TYPE::TEXTURE));*/

				/*for each (R_Mesh* i in App->renderer3D->meshes)
				{
					R_Texture* tex = new R_Texture();
					tex->ImportTexture(fileDir);

					std::string path = App->resource->SaveToLibrary(tex);
					RELEASE(tex);

					tex = static_cast<R_Texture*>(App->resource->LoadFromLibrary(path, R_TYPE::TEXTURE));

					i->BindTexture(tex);
				}*/
			}
		}
	}

	return go;
}

void ModuleResource::ImportModel(const char* meshPath, std::vector<const char*> texPaths)
{
	GameObject* go = nullptr;
	go = ImportFile(meshPath);

	if (texPaths.size() == 1)
	{
		for (int i = 0; i < go->vChildren.size(); i++)
		{
			ImportFile(texPaths[0], go->vChildren[i]);
		}
	}
	else
	{
		for (int i = 0; i < go->vChildren.size(); i++)
		{
			if (i < texPaths.size())
			{
				ImportFile(texPaths[i], go->vChildren[i]);
			}
			else
			{
				ImportFile(texPaths[go->vChildren.size() - 1], go->vChildren[i]);
			}
		}
	}

	App->parson->CreateGOMetaFile(go);
	go = nullptr;
}

std::string ModuleResource::SaveToLibrary(Resource* r)
{
	std::string path, ext;

	char* buffer = nullptr;
	uint size = 0;

	switch (r->GetType())
	{
	case R_TYPE::MESH:
		path = MESHES_PATH;
		ext = ".pnk";
		size = I_Mesh::Save(static_cast<R_Mesh*>(r), &buffer);
		break;
	case R_TYPE::TEXTURE:
		path = TEXTURES_PATH;
		ext = ".sufrimientoinfernal"; // TODO: que nombre de extension tiene que tener? 
		size = I_Texture::Save(static_cast<R_Texture*>(r), &buffer);
		break;
	case R_TYPE::SCENE:
		break;
	case R_TYPE::NONE:
		break;
	default:
		break;
	}

	path = path + std::to_string(r->GetUID()) + ext;

	App->fs->Save(path.c_str(), buffer, size);

	//RELEASE_ARRAY(buffer);	// TODO: peta jaja
	buffer = nullptr;
	return path;
}

Resource* ModuleResource::LoadFromLibrary(std::string path, R_TYPE type)
{
	char* buffer = nullptr;

	Resource* r;
	uint size = App->fs->Load(path.c_str(), &buffer);

	switch (type)
	{
	case R_TYPE::MESH:
		r = new R_Mesh();
		I_Mesh::Load(static_cast<R_Mesh*>(r), buffer);
		break;
	case R_TYPE::TEXTURE:
		r = new R_Texture();
		I_Texture::Load(static_cast<R_Texture*>(r), buffer, size);
		break;
	case R_TYPE::SCENE:
		break;
	case R_TYPE::NONE:
		break;
	default:
		break;
	}

	//RELEASE_ARRAY(buffer);
	buffer = nullptr;
	return r;
}