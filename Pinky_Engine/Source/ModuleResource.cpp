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
	pendingToLoadScene = false;

	return true;
}

// -----------------------------------------------------------------
update_status ModuleResource::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleResource::FinishUpdate(float dt)
{
	if (pendingToLoadScene)
	{
		RELEASE(App->scene->rootNode);
		App->parson->LoadScene(sceneFileName);
		LoadChildrenMeshes(App->scene->rootNode, App->scene->rootNode->vChildren.size());

		pendingToLoadScene = false;
	}
	else
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
	}

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
std::string ModuleResource::ImportFileToEngine(const char* fileDir)
{
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

	return filePath;
}

/// <summary>
/// If first time import to engine --> Create a copy in local space
/// If it already exists the file --> Dupe it with another name (nameN.ext) { N == number}
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
int ModuleResource::ImportToScene(std::string path, std::string dir, GameObject* goParent, bool component)
{
	GameObject* go = nullptr;
	std::string::size_type i = 0;
	std::string normFileName = App->fs->NormalizePath((dir + path).c_str());
	std::string libPath;

	//const char* realPath = App->parson->GetRealDirFF((ASSETS_AUX + path).c_str());	
	if (App->fs->Exists((normFileName + ".meta").c_str()))
	{
		switch (CheckExtensionType(path.c_str()))
		{
		case R_TYPE::MESH:
			libPath = App->parson->HasToReImport((normFileName + ".meta").c_str(), R_TYPE::MESH);
			if (libPath == "")
			{
				go = ai::ImportMesh(normFileName.c_str(), goParent, component);

				// Creates "Assets/name.ext.meta"
				App->parson->CreateResourceMetaFile(vTempM, normFileName.c_str());
				go->ReSetUID();

				ClearVec(vTempM);
			}
			else
			{
				go = App->parson->CreateGOfromMeta(PREFABS_PATH + App->fs->GetFileName(path.c_str()) + PREFABS_EXT);
				LoadChildrenMeshes(go, go->vChildren.size());
			}
			break;
		case R_TYPE::TEXTURE:
			libPath = App->parson->HasToReImport((normFileName + ".meta").c_str(), R_TYPE::TEXTURE);
			if (libPath == "")
			{
				R_Texture* t = new R_Texture();
				I_Texture::Import(normFileName.c_str(), t);
				std::string texturePath = SaveToLibrary(t);
				vTempT.push_back(t);

				// Creates "Assets/name.ext.meta"
				App->parson->CreateResourceMetaFile(vTempT, normFileName.c_str());

				RELEASE(t);
				LoadChildrenTextures(texturePath);
				ClearVec(vTempT);
			}
			else
			{
				LoadChildrenTextures(libPath);
			}
			break;
		case R_TYPE::SCENE:
			// This will never happen as scenes don't create meta
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
			go = ai::ImportMesh(normFileName.c_str(), goParent, component);

			// Creates "Assets/name.ext.meta"
			App->parson->CreateResourceMetaFile(vTempM, normFileName.c_str());
			go->ReSetUID();

			ClearVec(vTempM);
			break;
		case R_TYPE::TEXTURE:
			if (true)
			{
				R_Texture* t = new R_Texture();
				I_Texture::Import(normFileName.c_str(), t);
				std::string texturePath = SaveToLibrary(t);
				vTempT.push_back(t);

				// Creates "Assets/name.ext.meta"
				App->parson->CreateResourceMetaFile(vTempT, normFileName.c_str());

				RELEASE(t);
				LoadChildrenTextures(texturePath);
				ClearVec(vTempT);
			}
			break;
		case R_TYPE::SCENE:
			pendingToLoadScene = true;
			sceneFileName = normFileName;
			break;
		case R_TYPE::NONE:
			break;
		default:
			break;
		}
	}

	LOG("%s imported", normFileName);

	go = nullptr;
	return 0;
}

//
void ModuleResource::ImportModel(const char* meshPath, std::vector<const char*> texPaths)
{
	GameObject* go = nullptr;

	//Get_Set_FilePath(meshPath);

	ImportFileToEngine(meshPath);
	//ImportToScene(meshPath);

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
//

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
		ext = TEXTURES_EXT;
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

			if (i != r->GetUID()) { r->SetUID(i); }

			I_Texture::Load(static_cast<R_Texture*>(r), path.c_str());
			break;
		case R_TYPE::SCENE:
			break;
		case R_TYPE::NONE:
			break;
		default:
			break;
		}

		RELEASE_ARRAY(buffer);
	}

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

	if (App->fs->HasExtension(fileDir, "pnk"))
	{
		return R_TYPE::SCENE;
	}
	return R_TYPE::NONE;
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

void ModuleResource::LoadChildrenTextures(std::string path)
{
	std::vector<GameObject*> it = App->scene->hierarchy->GetSelectedGOs();
	C_Material* mat;
	for (int i = 0; i < it.size(); i++)
	{
		mat = static_cast<C_Material*>(it[i]->GetComponentByType(C_TYPE::MATERIAL));
		if (mat != nullptr && mat->tex != nullptr)
		{
			if (path != "")
			{
				// Get correct path (Probably from assimp)
				std::string filePath1, fileName1;
				App->fs->SplitFilePath(path.c_str(), &filePath1, &fileName1);
				mat->tex->libraryFile = filePath1 + fileName1;
			}

			std::string filePath, fileName, fileExt;
			App->fs->SplitFilePath((mat->tex->libraryFile + TEXTURES_EXT).c_str(), &filePath, &fileName, &fileExt);
			u32 id = std::stoi(fileName.c_str());
			
			auto itr = mResources.find(id);
			
			if (itr != mResources.end())
			{
				//RELEASE(mat->tex);
				mat->tex = static_cast<R_Texture*>(itr->second);
				mat->tex->vComponents.push_back(mat);
				mat->tex->name = it[i]->name;
			}
			else
			{
				mat->tex = static_cast<R_Texture*>(LoadFromLibrary(mat->tex->libraryFile + TEXTURES_EXT, R_TYPE::TEXTURE));
				mat->tex->vComponents.push_back(mat);
				mat->tex->name = fileName;
			}
			AddResource(mat->tex);
		}
	}
	mat = nullptr;
	ClearVec(it);
}


bool ModuleResource::AddResource(Resource* r, bool i)
{
	// If i == true --> add, else substract resource
	if (i)
	{
		auto itr = mResources.find(r->GetUID());
		r->count++;

		// Only add it to the vector if it hasn't been added
		if (itr == mResources.end())
		{
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

		mResources.insert(std::pair<uint, Resource*>(r->GetUID(), r));
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
				ClearVec(vMeshesResources);
				break;
			case R_TYPE::TEXTURE:
				ClearVec(vTexturesResources);
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
