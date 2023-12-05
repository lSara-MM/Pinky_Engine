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
	mResources = {};
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

GameObject* ModuleResource::ImportFileToEngine(const char* fileDir, GameObject* goToLink)
{
	std::string dir = fileDir;
	GameObject* go = nullptr;

	std::string normFileName = App->fs->NormalizePath((metaPath + ".meta.json").c_str());

	if (App->fs->Exists(normFileName.c_str()))
	{
		switch (CheckExtensionType(fileDir))
		{
		case R_TYPE::MESH:

			// get mesh uid and add to counter 



			//go = App->parson->CreateGOfromMeta(normFileName);
			//LoadChildrenMeshes(go, go->vChildren.size());
			break;
		case R_TYPE::TEXTURE:
			LoadFromLibrary(normFileName, R_TYPE::TEXTURE);
			break;
		case R_TYPE::SCENE:
			break;
		case R_TYPE::NONE:
			break;
		default:
			break;
		}

		//LoadFromLibrary(R_TYPE::MESH);
	}
	else
	{
		std::vector<Resource*>* vResources = &std::vector<Resource*>();
		switch (CheckExtensionType(fileDir))
		{
		case R_TYPE::MESH:
			vResources = ai::ImportVMesh(fileDir);

			// TODO: Delete when we dupe
			App->parson->CreateResourceMetaFile(*vResources, fileDir, ".fbx");
			break;
		case R_TYPE::TEXTURE:
			if (true)
			{
				// TODO: no funciona jaja
				R_Texture* t = new R_Texture();
				t->ImportTexture(fileDir);
				SaveToLibrary(t);
				LoadFromLibrary(fileDir, R_TYPE::TEXTURE);

				vResources->push_back(t);

				// TODO: Delete when we dupe
				App->parson->CreateResourceMetaFile(*vResources, fileDir, ".dds");
			}
			break;
		case R_TYPE::SCENE:
			break;
		case R_TYPE::NONE:
			break;
		default:
			break;
		}

		App->parson->CreateResourceMetaFile(*vResources, fileDir);

		vResources = nullptr;
	}

	return go;
}

void ModuleResource::ImportVModel(const char* meshPath, std::vector<const char*> texPaths)
{
	Get_Set_FilePath(meshPath);

	ImportFileToEngine(meshPath);
	ImportFileToEngine(texPaths[0]);
}

int ModuleResource::ImportToScene(const char* path)
{
	LOG("path %s", path);
	path;
	std::string normFileName = App->fs->NormalizePath((metaPath + ".meta.json").c_str());
	std::string dir = "a";
	GameObject* go = nullptr;
	go = App->parson->CreateGOfromMeta(normFileName);

	LOG("normFileName %s", normFileName);
	return 0;
}


GameObject* ModuleResource::ImportFile(const char* fileDir, GameObject* goToLink)
{
	std::string dir = fileDir;
	GameObject* go = nullptr;

	std::string normFileName = App->fs->NormalizePath((metaPath + ".meta.json").c_str());

	if (App->fs->Exists(normFileName.c_str()))
	{
		switch (CheckExtensionType(fileDir))
		{
		case R_TYPE::MESH:

			// get mesh uid and add to counter 



			//go = App->parson->CreateGOfromMeta(normFileName);
			//LoadChildrenMeshes(go, go->vChildren.size());
			break;
		case R_TYPE::TEXTURE:
			//LoadFromLibrary(normFileName, R_TYPE::TEXTURE);
			break;
		case R_TYPE::SCENE:
			break;
		case R_TYPE::NONE:
			break;
		default:
			break;
		}

		//LoadFromLibrary(R_TYPE::MESH);
	}
	else
	{
		switch (CheckExtensionType(fileDir))
		{
		case R_TYPE::MESH:
			go = ai::ImportMesh(fileDir);
			break;
		case R_TYPE::TEXTURE:
			if (true)
			{
				C_Material* mat = static_cast<C_Material*>(goToLink->GetComponentByType(C_TYPE::MATERIAL));
				mat->tex->ImportTexture(fileDir);
				std::string path = App->resource->SaveToLibrary(mat->tex);
				mat->tex = static_cast<R_Texture*>(App->resource->LoadFromLibrary(path, R_TYPE::TEXTURE));
			}			
			break;
		case R_TYPE::SCENE:
			break;
		case R_TYPE::NONE:
			break;
		default:
			break;
		}
		//App->parson->CreateMeshMetaFile(go, fileDir);
		//App->parson->CreateJSON(go, "Assets\\");
		//App->parson->CreateJSON(go, fileDir);
	}

	return go;
}

void ModuleResource::ImportModel(const char* meshPath, std::vector<const char*> texPaths)
{
	GameObject* go = nullptr;

	Get_Set_FilePath(meshPath);

	go = ImportFileToEngine(meshPath);

	//if (texPaths.empty())
	//{
	//}
	//else if (texPaths.size() == 1)
	//{
	//	for (int i = 0; i < go->vChildren.size(); i++)
	//	{
	//		ImportFile(texPaths[0], go->vChildren[i]);
	//	}
	//}
	//else
	//{
	//	for (int i = 0; i < go->vChildren.size(); i++)
	//	{
	//		if (i < texPaths.size())
	//		{
	//			ImportFile(texPaths[i], go->vChildren[i]);
	//		}
	//		else
	//		{
	//			ImportFile(texPaths[go->vChildren.size() - 1], go->vChildren[i]);
	//		}
	//	}
	//}

	//App->parson->CreateJSON(go, meshPath);
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
		ext = MESHES_EXT;	
		size = I_Mesh::Save(static_cast<R_Mesh*>(r), &buffer);
		break;
	case R_TYPE::TEXTURE:
		path = TEXTURES_PATH;
		ext = ".dds";
		size = I_Texture::Save(static_cast<R_Texture*>(r), &buffer);
		break;
	case R_TYPE::SCENE:
		break;
	case R_TYPE::NONE:
		break;
	default:
		break;
	}

	//mResources->insert(std::pair<uint, Resource*>(r->GetUID(), r));
	path = path + std::to_string(r->GetUID()) + ext;

	App->fs->Save(path.c_str(), buffer, size);

	//RELEASE_ARRAY(buffer);	// TODO: peta jaja
	buffer = nullptr;
	return path;
}

Resource* ModuleResource::LoadFromLibrary(std::string path, R_TYPE type)
{
	char* buffer = nullptr;

	Resource* r = nullptr;
	uint size = App->fs->Load(path.c_str(), &buffer);

	if (size != 0)
	{
		std::string filePath, fileName, fileExt;
		App->fs->SplitFilePath(path.c_str(), &filePath, &fileName, &fileExt);

		u32 i = std::stoi(fileName.c_str());
		switch (type)
		{
		case R_TYPE::MESH:
			r = new R_Mesh();

			if (i != r->GetUID()) { r->SetUID(i); }

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

		//mResources->insert(std::pair<uint, Resource*>(r->GetUID(), r));
		//RELEASE_ARRAY(buffer);
	}
	buffer = nullptr;
	
	return r;
}

R_TYPE ModuleResource::CheckExtensionType(const char* fileDir)
{
	std::vector<std::string> obj_ext = { "fbx", "FBX", "obj", };
	std::vector<std::string> tex_ext = { "png", "PNG", "jpg", "JPG", "dds", "DDS" };
	
	if (App->fs->HasExtension(fileDir, obj_ext))
	{
		return R_TYPE::MESH;
	}
	
	if (App->fs->HasExtension(fileDir, tex_ext))
	{
		return R_TYPE::TEXTURE;
	}

	return R_TYPE::NONE;
}

std::string ModuleResource::Get_Set_FilePath(const char* fileDir)
{
	string dir = fileDir;
	std::string filePath, fileName, fileExt;
	App->fs->SplitFilePath(fileDir, &filePath, &fileName, &fileExt);
	
	if (dir.find("C:\\") != std::string::npos)
	{
		metaPath = "Assets\\" + fileName;
	}
	else
	{
		metaPath = filePath + fileName;
	}
	metaPath = filePath + fileName;

	return metaPath;
}

void ModuleResource::LoadChildrenMeshes(GameObject* go, uint size)
{
	if (go->mesh != nullptr)
	{
		LoadFromLibrary(go->mesh->mesh->libraryFile + MESHES_EXT, R_TYPE::MESH);
	}

	for (int i = 0; i < size; i++)
	{
		LoadChildrenMeshes(go->vChildren[i], go->vChildren[i]->vChildren.size());
	}
}
