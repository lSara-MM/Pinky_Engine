#include "Globals.h"
#include "Application.h"
#include "ModuleResource.h"
#include "ModuleScene.h"

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

update_status ModuleResource::PostUpdate(float dt)
{
	for (auto it = vPendingToDelete.begin(); it != vPendingToDelete.end(); it++)
	{
		for (int i = 0; i < (*it)->vComponents.size(); i++)
		{
			(*it)->vComponents[i]->gameObject->RemoveComponent((*it)->vComponents[i]);
		}
		//RELEASE((*it));
		/*for (int i = 0; i < (*it)->vComponents.size(); i++)
		{
			RELEASE((*it)->vComponents[i]);
		}*/

		/*for (auto comp = (*it)->vComponents.begin(); comp != (*it)->vComponents.end(); comp++)
		{
			RELEASE((*comp));
		}*/
	}

	ClearVec(vPendingToDelete);
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
bool ModuleResource::CleanUp()
{
	LOG("Cleaning resources");

	return true;
}

/// <summary>
/// If first time import to engine --> Create a copy in local space
/// If it already exists the file --> Dupe it with another name (nameN.ext) { N == number}
/// </summary>
/// <param name="fileDir"> Directory File to Import </param>
/// <returns></returns>
GameObject* ModuleResource::ImportFileToEngine(const char* fileDir)
{
	GameObject* go = nullptr;

	std::string filePath, fileName, fileExt, tempName, finalPath;
	App->fs->SplitFilePath(fileDir, &filePath, &fileName, &fileExt);

	//std::string dir = ASSETS_AUX + fileName + "." + fileExt;
	//tempName = fileName;
	//int i = 0;
	//while (App->fs->Exists(dir.c_str()))
	//{
	//	tempName = fileName;
	//	tempName += std::to_string(++i);
	//	dir = ASSETS_AUX + tempName + "." + fileExt;
	//}
	//App->parson->CreateFile(tempName, fileExt, fileDir);

	App->fs->DuplicateFile(fileDir, App->scene->project->selectedDir.c_str(), finalPath);

	return go;
}

void ModuleResource::ImportVModel(const char* meshPath, std::vector<const char*> texPaths)
{
	ImportFileToEngine(meshPath);
	ImportFileToEngine(texPaths[0]);
}

/// <summary>
/// If first time import to engine --> Create a copy in local space
/// If it already exists the file --> Dupe it with another name (nameN.ext) { N == number}
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
int ModuleResource::ImportToScene(std::string path, std::string dir)
{
	GameObject* go = nullptr;
	std::string::size_type i = 0;
	std::string normFileName = App->fs->NormalizePath((dir + path).c_str());

	//const char* realPath = App->parson->GetRealDirFF((ASSETS_AUX + path).c_str());
	if (App->fs->Exists((normFileName + ".meta").c_str()))
	{
		switch (CheckExtensionType(path.c_str()))
		{
		case R_TYPE::MESH:
			go = App->parson->CreateGOfromMeta(PREFABS_PATH + App->fs->GetFileName(path.c_str()) + PREFABS_EXT);
			LoadChildrenMeshes(go, go->vChildren.size());
			// get mesh uid and add to counter 
			//go = App->parson->CreateGOfromMeta(normFileName);
			break;
		case R_TYPE::TEXTURE:
			LoadFromLibrary((normFileName + ".meta"), R_TYPE::TEXTURE);
			break;
		case R_TYPE::SCENE:
			break;
		case R_TYPE::NONE:
			break;
		default:
			break;
		}
	}
	else
	{
		switch (CheckExtensionType(path.c_str()))
		{
		case R_TYPE::MESH:
			ai::ImportMesh(normFileName.c_str());
			break;
		case R_TYPE::TEXTURE:
			if (true)
			{
				// TODO: no funciona jaja
				R_Texture* t = new R_Texture();
				t->ImportTexture(path.c_str());
				SaveToLibrary(t);
				LoadFromLibrary(path.c_str(), R_TYPE::TEXTURE);

				vResources.push_back(t);
			}
			break;
		case R_TYPE::SCENE:
			break;
		case R_TYPE::NONE:
			break;
		default:
			break;
		}

		// Creates "Assets/name.ext.meta"
		App->parson->CreateResourceMetaFile(vResources, (normFileName + ".meta").c_str());
		ClearVec(vResources);
	}

	LOG("%s imported", normFileName);

	go = nullptr;
	return 0;
}


// TODO: DEPRECATED; NOT USED ANYMORE. KEEP ONLY FOR REFERENCE 
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
			//go = ai::ImportMesh(fileDir);
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

	metaPath = ASSETS_AUX + fileName;
	/*if (dir.find("C:\\") != std::string::npos)
	{
		metaPath = ASSETS_AUX + fileName;
	}
	else
	{
		metaPath = filePath + fileName;
	}
	metaPath = filePath + fileName;*/

	return metaPath;
}

void ModuleResource::LoadChildrenMeshes(GameObject* go, uint size)
{
	if (go->mesh != nullptr)
	{
		std::string filePath, fileName, fileExt;
		App->fs->SplitFilePath((go->mesh->mesh->libraryFile + MESHES_EXT).c_str(), &filePath, &fileName, &fileExt);

		u32 i = std::stoi(fileName.c_str());

		auto itr = mResources.find(i);
		if (itr != mResources.end())
		{
			RELEASE(go->mesh->mesh);
			go->mesh->mesh = static_cast<R_Mesh*>(itr->second);
			go->mesh->mesh->vComponents.push_back(go->mesh);
			go->mesh->mesh->name = go->name;
		}
		else
		{
			go->mesh->mesh = static_cast<R_Mesh*>(LoadFromLibrary(go->mesh->mesh->libraryFile + MESHES_EXT, R_TYPE::MESH));
			go->mesh->mesh->vComponents.push_back(go->mesh);
			go->mesh->mesh->name = go->name;
		}

		AddResource(go->mesh->mesh);
	}

	for (int i = 0; i < size; i++)
	{
		LoadChildrenMeshes(go->vChildren[i], go->vChildren[i]->vChildren.size());
	}
}

bool ModuleResource::AddResource(Resource* r, bool i)
{
	// If i == true --> add, else substract resource
	if (i)
	{
		r->count++;
		mResources.insert(std::pair<uint, Resource*>(r->GetUID(), r));

		switch (r->GetType())
		{
		case R_TYPE::MESH:
			vMeshesResources.push_back(r);
			break;
		case R_TYPE::TEXTURE:
			vTexturesResources.push_back(r);
			break;
		case R_TYPE::PREFAB:
			break;
		case R_TYPE::SCENE:
			break;
		case R_TYPE::NONE:
			break;
		default:
			break;
		}
	}
	else
	{
		r->count--;

		if (r->count == 0)
		{
			R_TYPE t = r->GetType();
			/*for (int i = 0; i < r->vComponents.size(); i++)
			{
				r->vComponents[i]->gameObject->RemoveComponent(r->vComponents[i]);
			}*/
			mResources.erase(r->GetUID());
			RELEASE(r);

			switch (t)
			{
			case R_TYPE::MESH:
				vMeshesResources.shrink_to_fit();
				break;
			case R_TYPE::TEXTURE:
				vTexturesResources.shrink_to_fit();
				break;
			case R_TYPE::PREFAB:
				break;
			case R_TYPE::SCENE:
				break;
			case R_TYPE::NONE:
				break;
			default:
				break;
			}
			return false;
		}
	}

	return true;
}
