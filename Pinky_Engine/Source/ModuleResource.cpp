#include "Globals.h"
#include "Application.h"
#include "ModuleResource.h"
#include "ModuleScene.h"

#include "FileSystemManager.h"
#include "GameObject.h"

#include "Resource.h"
#include "I_Mesh.h"
#include "I_Texture.h"

#include "ImGuiWindows.h"

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

		for (int i = 0; i < App->scene->hierarchy->vSelectedGOs.size(); i++)
		{
			App->scene->hierarchy->vSelectedGOs[i] = nullptr;
		}

		ClearVec(App->scene->hierarchy->GetSelectedGOs());

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

			if ((*it)->vComponents.empty())
			{
				AddResource((*it), false);
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
/// <param name="path"> file name </param>
/// <param name="dir"> file path without name </param>
/// <returns></returns>
//int ModuleResource::ImportToScene(std::string path, std::string dir, GameObject* goParent, bool component)
//{
//	GameObject* go = nullptr;
//	std::string::size_type i = 0;
//	std::string normFileName = App->fs->NormalizePath((dir + path).c_str());
//	std::string libPath;
//
//	std::string filePath;
//	App->fs->SplitFilePath(normFileName.c_str(), &filePath);
//	assetsPathAux = normFileName;
//
//	//const char* realPath = App->parson->GetRealDirFF((ASSETS_AUX + path).c_str());	
//	if (App->fs->Exists((normFileName + ".meta").c_str()))
//	{
//		switch (CheckExtensionType(path.c_str()))
//		{
//		case R_TYPE::MESH:
//			libPath = App->parson->HasToReImport((normFileName + ".meta").c_str(), R_TYPE::MESH);
//			if (libPath == "")
//			{
//				go = ai::ImportMesh(normFileName.c_str(), goParent, component);
//				if (go != nullptr)
//				{
//					// Creates "Assets/name.ext.meta"
//					App->parson->CreateResourceMetaFile(vTempM, normFileName.c_str());
//					go->ReSetUID();
//
//					ClearVec(vTempM);
//				}
//			}
//			else
//			{
//				if (!component)
//				{
//					go = App->parson->CreateGOfromMeta(PREFABS_PATH + App->fs->GetFileName(path.c_str()) + PREFABS_EXT);
//					if (go != nullptr)
//					{
//						LoadChildrenMeshes(go, go->vChildren.size(), assetsPathAux);
//					}
//				}
//				else
//				{
//					auto itr = mResources.find(i);
//					if (itr != mResources.end())
//					{
//						goParent->mesh = new C_Mesh(goParent, true);
//						goParent->mesh->mesh = static_cast<R_Mesh*>(itr->second);
//						goParent->mesh->mesh->vComponents.push_back(goParent->mesh);
//						goParent->mesh->mesh->name = App->fs->GetFileName(normFileName.c_str());
//						goParent->mesh->mesh->name = assetsPathAux;
//					}
//					else
//					{
//						goParent->mesh = new C_Mesh(goParent, true);
//						std::string lib = App->parson->GetResourceMetaFile((normFileName + ".meta").c_str());
//						if (lib != "")
//						{
//							goParent->mesh->mesh = static_cast<R_Mesh*>(LoadFromLibrary(lib, R_TYPE::MESH));
//							goParent->mesh->mesh->vComponents.push_back(goParent->mesh);
//							goParent->mesh->mesh->name = App->fs->GetFileName(normFileName.c_str());
//							goParent->mesh->mesh->name = assetsPathAux;
//
//							vMeshesResources.push_back(goParent->mesh->mesh);
//						}
//					}
//				}
//			}
//			break;
//		case R_TYPE::TEXTURE:
//			libPath = App->parson->HasToReImport((normFileName + ".meta").c_str(), R_TYPE::TEXTURE);
//			if (libPath == "")
//			{
//				R_Texture* t = new R_Texture();
//				if (I_Texture::Import(normFileName.c_str(), t))
//				{
//					std::string texturePath = SaveToLibrary(t);
//					vTempT.push_back(t);
//
//					// Creates "Assets/name.ext.meta"
//					App->parson->CreateResourceMetaFile(vTempT, normFileName.c_str());
//
//					LoadChildrenTextures(texturePath);
//				}
//				RELEASE(t);
//				ClearVec(vTempT);
//			}
//			else
//			{
//				LoadChildrenTextures(libPath);
//			}
//			break;
//		case R_TYPE::SCENE:
//			// This will never happen as scenes don't create meta
//			break;
//		case R_TYPE::NONE:
//			break;
//		default:
//			break;
//		}
//	}
//	else
//	{
//		switch (CheckExtensionType(path.c_str()))
//		{
//		case R_TYPE::MESH:
//			go = ai::ImportMesh(normFileName.c_str(), goParent, component);
//			if (go != nullptr)
//			{
//				// Creates "Assets/name.ext.meta"
//				App->parson->CreateResourceMetaFile(vTempM, normFileName.c_str());
//				go->ReSetUID();
//
//				ClearVec(vTempM);
//			}
//			break;
//		case R_TYPE::TEXTURE:
//			if (true)
//			{
//				R_Texture* t = new R_Texture();
//				if (I_Texture::Import(normFileName.c_str(), t))
//				{
//					std::string texturePath = SaveToLibrary(t);
//					vTempT.push_back(t);
//
//					// Creates "Assets/name.ext.meta"
//					App->parson->CreateResourceMetaFile(vTempT, normFileName.c_str());
//
//					LoadChildrenTextures(texturePath);
//				}
//				RELEASE(t);
//				ClearVec(vTempT);
//			}
//			break;
//		case R_TYPE::SCENE:
//			pendingToLoadScene = true;
//			sceneFileName = normFileName;
//			break;
//		case R_TYPE::NONE:
//			break;
//		default:
//			break;
//		}
//	}
//
//	go = nullptr;
//	return 0;
//}


int ModuleResource::ImportToSceneV(std::string file, std::string dir, GameObject* goParent, bool component)
{
	std::string auxPath = App->fs->NormalizePath((dir + file).c_str());
	assetsPathAux = auxPath;

	switch (CheckExtensionType(file.c_str()))
	{
	case R_TYPE::MESH:
		if (!App->fs->Exists((auxPath + ".meta").c_str()) || App->parson->HasToReImport((auxPath + ".meta").c_str(), R_TYPE::MESH) == "")
		{
			GameObject* go = ai::ImportMesh(auxPath.c_str(), goParent, component);

			if (go != nullptr)
			{
				// Creates GameObject meta
				App->parson->CreatePrefabFromGO(go);

				// Creates "Assets/name.ext.meta"
				App->parson->CreateResourceMetaFile(vTempM, auxPath.c_str());
				go->ReSetUID();

				ClearVec(vTempM);
			}

			go = nullptr;
		}
		else
		{
			App->parson->CreateGOfromMeta(PREFABS_PATH + App->fs->GetFileName(file.c_str()) + PREFABS_EXT);
		}
		break;
	case R_TYPE::TEXTURE:
		if (!App->fs->Exists((auxPath + ".meta").c_str()) || App->parson->HasToReImport((auxPath + ".meta").c_str(), R_TYPE::TEXTURE) == "")
		{
			R_Texture* t = new R_Texture();
			if (I_Texture::Import(auxPath.c_str(), t))
			{
				std::string texturePath = SaveToLibrary(t);
				vTempT.push_back(t);

				LoadChildrenTextures(auxPath, texturePath);

				// Creates "Assets/name.ext.meta"
				App->parson->CreateResourceMetaFile(vTempT, auxPath.c_str());
			}
			RELEASE(t);
			ClearVec(vTempT);
		}
		else
		{
			std::string libPath = App->parson->HasToReImport((auxPath + ".meta").c_str(), R_TYPE::TEXTURE);
			if (libPath == "")
			{
				R_Texture* t = new R_Texture();
				if (I_Texture::Import(auxPath.c_str(), t))
				{
					std::string texturePath = SaveToLibrary(t);
					vTempT.push_back(t);

					LoadChildrenTextures(auxPath, texturePath);

					// Creates "Assets/name.ext.meta"
					App->parson->CreateResourceMetaFile(vTempT, auxPath.c_str());
				}
				RELEASE(t);
				ClearVec(vTempT);
			}
			else
			{
				LoadChildrenTextures(auxPath, libPath);
			}
		}
		break;
	case R_TYPE::PREFAB:
		break;
	case R_TYPE::SCENE:
		pendingToLoadScene = true;
		sceneFileName = assetsPathAux;
		break;
	case R_TYPE::NONE:
		break;
	default:
		break;
	}

	return 0;
}

//
void ModuleResource::ImportModel(const char* meshPath, std::vector<const char*> texPaths)
{
	GameObject* go = nullptr;

	//Get_Set_FilePath(meshPath);

	ImportFileToEngine(meshPath);
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
	std::vector<std::string> obj_ext = { "fbx", "FBX", "obj", "OBJ", "DAE", "dae" };
	std::vector<std::string> tex_ext = { "png", "PNG", "jpg", "JPG", "dds", "DDS", "tga", "TGA" };

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

void ModuleResource::LoadMesh(GameObject& go)
{
	if (go.mesh != nullptr)
	{
		std::string filePath;
		App->fs->SplitFilePath(go.mesh->mesh->libraryFile.c_str(), &filePath);

		auto itr = mResources.find(go.mesh->mesh->GetUID());
		if (itr != mResources.end())
		{
			RELEASE(go.mesh->mesh);
			go.mesh->mesh = static_cast<R_Mesh*>(itr->second);
			go.mesh->mesh->vComponents.push_back(go.mesh);
			go.mesh->mesh->name = go.name;
		}
		else
		{
			go.mesh->mesh = static_cast<R_Mesh*>(LoadFromLibrary(go.mesh->mesh->libraryFile, R_TYPE::MESH));
			go.mesh->mesh->vComponents.push_back(go.mesh);
			go.mesh->mesh->name = go.name;
		}

		if (go.mesh->mesh != nullptr)
		{
			AddResource(go.mesh->mesh);
		}
	}
}

void ModuleResource::LoadChildrenTextures(std::string path, std::string libPath)
{
	std::vector<GameObject*> it = App->scene->hierarchy->GetSelectedGOs();
	C_Material* mat;
	for (int i = 0; i < it.size(); i++)
	{
		mat = static_cast<C_Material*>(it[i]->GetComponentByType(C_TYPE::MATERIAL));
		if (mat != nullptr && mat->tex != nullptr)
		{
			std::string filePath, fileName;
			App->fs->SplitFilePath((libPath).c_str(), &filePath, &fileName);
			u32 id = std::stoi(fileName.c_str());

			auto itr = mResources.find(id);

			if (itr != mResources.end())
			{
				RELEASE(mat->tex);
				mat->tex = static_cast<R_Texture*>(itr->second);
				mat->tex->vComponents.push_back(mat);
				mat->tex->name = App->fs->GetFileName(path.c_str());
				//mat->tex->name = it[i]->name;
			}
			else
			{
				mat->tex = static_cast<R_Texture*>(LoadFromLibrary(libPath, R_TYPE::TEXTURE));

				if (mat->tex != nullptr)
				{
					mat->tex->vComponents.push_back(mat);
					mat->tex->name = App->fs->GetFileName(path.c_str());
					//mat->tex->name = it[i]->name;
				}
			}

			mat->tex->assetsFile = path;
			mat->tex->libraryFile = libPath;
			AddResource(mat->tex);
		}
	}
	mat = nullptr;
	ClearVec(it);
}


bool ModuleResource::AddResource(Resource* r, bool i)
{
	// If i == true --> add, else substract resource
	if (r != nullptr)
	{
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
				Resource* temp = r;
				/*for (int i = 0; i < r->vComponents.size(); i++)
				{
					r->vComponents[i]->gameObject->RemoveComponent(r->vComponents[i]);
				}*/
				mResources.erase(r->GetUID());
				RELEASE(r);

				switch (temp->GetType())
				{
				case R_TYPE::MESH:
					vMeshesResources.erase(std::remove(vMeshesResources.begin(), vMeshesResources.end(), temp), vMeshesResources.end());
					//ClearVec(vMeshesResources);
					break;
				case R_TYPE::TEXTURE:
					vTexturesResources.erase(std::remove(vTexturesResources.begin(), vTexturesResources.end(), temp), vTexturesResources.end());
					//ClearVec(vTexturesResources);
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

				temp = nullptr;
				return false;
			}
		}
	}

	return true;
}
