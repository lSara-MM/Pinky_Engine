#include "parsingJSON.h"

#include "Globals.h"
#include "GameObject.h"

#include "External Libraries/Assimp/Assimp.h"
#include "ModuleResource.h"
#include "ModuleScene.h"

ParsingJSON::ParsingJSON()
{
}

ParsingJSON::~ParsingJSON()
{
}

void ParsingJSON::CreateFile(std::string fileName, std::string fileExt, const char* realDir)
{
	std::string file_name = "Assets//" + fileName + "." + fileExt;

	root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);
	char* serialized_string = NULL;

	std::string node_name = "GameObject.Info";
	json_object_dotset_string(root_object, (node_name + ".Name").c_str(), fileName.c_str());
	json_object_dotset_string(root_object, (node_name + ".Real Directory").c_str(), realDir);
	json_object_dotset_string(root_object, (node_name + ".Local Directory").c_str(), file_name.c_str());

	serialized_string = json_serialize_to_string_pretty(root_value);
	puts(serialized_string);

	json_serialize_to_file(root_value, file_name.c_str());
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);
}

const char* ParsingJSON::GetRealDirFF(const char* dir)
{
	root_value = json_parse_file(dir);
	root_object = json_value_get_object(root_value);

	return json_object_dotget_string(root_object, "GameObject.Info.Real Directory");
}

//---Create and write in .meta---
void ParsingJSON::CreateResourceMetaFile(std::vector<Resource*> resources, const char* path)
{
	std::string filePath, fileName, fileExt;
	App->fs->SplitFilePath(path, &filePath, &fileName, &fileExt);

	std::string file_name = filePath + fileName + "." + fileExt + ".meta";

	root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);

	if (root_value != nullptr)
	{
		char* serialized_string = NULL;

		std::string node_name = "Resource";
		std::string node_path;
		json_object_dotset_string(root_object, (node_name + ".Info.Local Directory").c_str(), file_name.c_str());
		json_object_dotset_string(root_object, (node_name + ".Info.Origin").c_str(), path);

		json_object_dotset_number(root_object, (node_name + ".Info.Children number").c_str(), resources.size());

		switch (App->resource->CheckExtensionType(path))
		{
		case R_TYPE::MESH:
			//node_name += ".Mesh ";
			node_path = MESHES_PATH;
			break;
		case R_TYPE::TEXTURE:
			//node_name += ".Texture ";
			node_path = TEXTURES_PATH;
			break;
		case R_TYPE::SCENE:
			break;
		case R_TYPE::NONE:
			break;
		default:
			break;
		}

		for (int i = 0; i < resources.size(); i++)
		{
			json_object_dotset_number(root_object, (node_name + " " + std::to_string(i) + ".UID").c_str(), resources.at(i)->GetUID());
			json_object_dotset_string(root_object, (node_name + " " + std::to_string(i) + ".Library Path").c_str(), (node_path +
				std::to_string(resources.at(i)->GetUID())).c_str());
		}

		serialized_string = json_serialize_to_string_pretty(root_value);
		puts(serialized_string);

		json_serialize_to_file(root_value, file_name.c_str());
		json_free_serialized_string(serialized_string);

		LOG("Successfully created [%s]", file_name.c_str());
	}
	else
	{
		LOG("Could not create meta file of [%s]", file_name.c_str());
	}
	json_value_free(root_value);
}

std::string ParsingJSON::GetResourceMetaFile(const char* path, int i)
{
	root_value = json_parse_file(path);
	root_object = json_value_get_object(root_value);
	std::string libPath = json_object_dotget_string(root_object, ("Resource " + std::to_string(i) + ".Library Path").c_str());

	return libPath;
}

//---Create and write GameObject structure information---
void ParsingJSON::CreatePrefabFromGO(GameObject* go)
{
	int resource_count = 0;

	std::string file_name = PREFABS_PATH + go->name + ".pgo";	// Pinky Game Object
	root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);
	if (root_value != nullptr)
	{
		char* serialized_string = NULL;

		std::string node_name = "GameObject.Info";
		json_object_dotset_number(root_object, (node_name + ".Children number").c_str(), resource_count);
		json_object_dotset_string(root_object, (node_name + ".Local Directory").c_str(), file_name.c_str());

		resource_count = GameObjectInfo(go, node_name + ".Resources") - 1;

		json_object_dotset_number(root_object, (node_name + ".Children number").c_str(), resource_count);

		GameObjectJSON(go, "GameObject.Parent");

		serialized_string = json_serialize_to_string_pretty(root_value);
		puts(serialized_string);

		json_serialize_to_file(root_value, file_name.c_str());
		json_free_serialized_string(serialized_string);

		LOG("Successfully created [%s]", file_name);
	}
	else
	{
		LOG("Could not create meta file of [%s]", file_name);
	}
	json_value_free(root_value);
}

int ParsingJSON::GameObjectJSON(GameObject* go, std::string node_name, int counter, std::string subInfo)
{
	counter++;
	json_object_dotset_string(root_object, (node_name + ".Name").c_str(), go->name.c_str());
	json_object_dotset_number(root_object, (node_name + ".UID").c_str(), go->GetUID());

	if (go->pParent != nullptr)
	{
		json_object_dotset_number(root_object, (node_name + ".Parent UID").c_str(), go->pParent->GetUID());
	}
	else
	{
		json_object_dotset_number(root_object, (node_name + ".Parent UID").c_str(), 0);
	}

	json_object_dotset_boolean(root_object, (node_name + ".Active").c_str(), go->isActive);
	json_object_dotset_boolean(root_object, (node_name + ".Static").c_str(), go->isStatic);
	json_object_dotset_number(root_object, (node_name + ".Components num").c_str(), go->vComponents.size());

	for (int i = 0; i < go->vComponents.size(); i++)
	{
		ComponentsJSON(go->vComponents[i], node_name, i);
	}

	for (int i = 0; i < go->vChildren.size(); i++)
	{
		counter = GameObjectJSON(go->vChildren[i], subInfo + ".Child " + std::to_string(counter), counter, subInfo);
	}

	return counter;
}

int ParsingJSON::GameObjectInfo(GameObject* go, std::string node_name, int counter)
{
	counter++;

	json_object_dotset_string(root_object, (node_name + ".Name").c_str(), go->name.c_str());
	json_object_dotset_number(root_object, (node_name + ".UID").c_str(), go->GetUID());

	for (int i = 0; i < go->vChildren.size(); i++)
	{
		counter = GameObjectInfo(go->vChildren[i], node_name, counter);
	}

	return counter;
}

void ParsingJSON::ComponentsJSON(Component* comp, std::string node_name, int i)
{
	std::string comp_name = node_name + ".Components.Component " + std::to_string(i);
	json_object_dotset_string(root_object, (comp_name + ".Name").c_str(), comp->name.c_str());
	json_object_dotset_number(root_object, (comp_name + ".Type").c_str(), (double)comp->type);
	json_object_dotset_number(root_object, (comp_name + ".Component UID").c_str(), comp->GetUID());

	switch (comp->type)
	{
	case C_TYPE::TRANSFORM:
		json_object_dotset_array(&static_cast<C_Transform*> (comp)->position[0], 3, comp_name + ".Position");
		json_object_dotset_array(&static_cast<C_Transform*> (comp)->rotation.x, 4, comp_name + ".Rotation");
		json_object_dotset_array(&static_cast<C_Transform*> (comp)->scale[0], 3, comp_name + ".Scale");
		break;
	case C_TYPE::MESH:
		json_object_dotset_number(root_object, (comp_name + ".Resource UID").c_str(), static_cast<C_Mesh*>(comp)->mesh->GetUID());
		json_object_dotset_string(root_object, (comp_name + ".Assets dir").c_str(),
			(static_cast<C_Mesh*>(comp)->mesh->assetsFile).c_str());
		json_object_dotset_string(root_object, (comp_name + ".Library dir").c_str(), (MESHES_PATH +
			std::to_string(static_cast<C_Mesh*>(comp)->mesh->GetUID()) + MESHES_EXT).c_str());
		break;
	case C_TYPE::MATERIAL:
		if (static_cast<C_Material*>(comp)->tex != nullptr)
		{
			json_object_dotset_number(root_object, (comp_name + ".Resource UID").c_str(), static_cast<C_Material*>(comp)->tex->GetUID());

			if ((static_cast<C_Material*>(comp)->tex->assetsFile) != "")
			{
				json_object_dotset_boolean(root_object, (comp_name + ".Has Assets").c_str(), true);
				json_object_dotset_string(root_object, (comp_name + ".Assets dir").c_str(),
					(static_cast<C_Material*>(comp)->tex->assetsFile).c_str());


				json_object_dotset_string(root_object, (comp_name + ".Library dir").c_str(), (TEXTURES_PATH +
					std::to_string(static_cast<C_Material*>(comp)->tex->GetUID()) + TEXTURES_EXT).c_str());
			}
			else
			{
				json_object_dotset_boolean(root_object, (comp_name + ".Has Assets").c_str(), false);
			}

			json_object_dotset_number(root_object, (comp_name + ".Color.R").c_str(), static_cast<C_Material*>(comp)->color.r);
			json_object_dotset_number(root_object, (comp_name + ".Color.G").c_str(), static_cast<C_Material*>(comp)->color.g);
			json_object_dotset_number(root_object, (comp_name + ".Color.B").c_str(), static_cast<C_Material*>(comp)->color.b);
			json_object_dotset_number(root_object, (comp_name + ".Color.A").c_str(), static_cast<C_Material*>(comp)->color.a);
		}
		else
		{
			LOG("[ERROR] Could not save material");
		}

		break;
	case C_TYPE::CAM:
		json_object_dotset_number(root_object, (comp_name + ".Near Plane").c_str(), static_cast<C_Camera*>(comp)->frustum.nearPlaneDistance);
		json_object_dotset_number(root_object, (comp_name + ".Far Plane").c_str(), static_cast<C_Camera*>(comp)->frustum.farPlaneDistance);
		json_object_dotset_number(root_object, (comp_name + ".FOV").c_str(), static_cast<C_Camera*>(comp)->fov);
		json_object_dotset_boolean(root_object, (comp_name + ".Game Camera").c_str(), static_cast<C_Camera*>(comp)->isGameCam);
		json_object_dotset_boolean(root_object, (comp_name + ".Culling").c_str(), static_cast<C_Camera*>(comp)->isCullEnabled);
		json_object_dotset_string(root_object, (comp_name + ".Library dir").c_str(), (TEXTURES_PATH +
			std::to_string(static_cast<C_Camera*>(comp)->GetUID())).c_str());
		break;
	case C_TYPE::NONE:
		break;
	default:
		break;
	}
}

//---Load from .meta---
GameObject* ParsingJSON::CreateGOfromMeta(std::string path, std::string subInfo)
{
	root_value = json_parse_file(path.c_str());
	root_object = json_value_get_object(root_value);

	GameObject* go = nullptr, * temp = nullptr;

	if (root_value != nullptr)
	{
		std::vector<GameObject*> vTemp;

		go = GOfromMeta(subInfo + ".Parent");
		vTemp.push_back(go);

		int vChildrensize = json_object_dotget_number(root_object, (subInfo + ".Info.Children number").c_str());

		for (int i = 0; i < vChildrensize; i++)
		{
			temp = GOfromMeta(subInfo + ".Child " + std::to_string(i + 1));
			vTemp.push_back(temp);
		}

		for (auto it = vTemp.begin(); it != vTemp.end(); it++)
		{
			(*it)->SetUID(App->randomLCG->Int());
			(*it) = nullptr;
		}

		ClearVec(vTemp);
		temp = nullptr;

		LOG("Successfully created [%s]", go->name.c_str());
	}
	else
	{
		LOG("Could not create game object of [%s]", path);
	}

	return go;
}

GameObject* ParsingJSON::GOfromMeta(std::string node_name, bool scene)
{
	GameObject* go = new GameObject();
	go->name = json_object_dotget_string(root_object, (node_name + ".Name").c_str());
	u32 id = json_object_dotget_number(root_object, (node_name + ".UID").c_str());
	go->SetUID(id);
	id = json_object_dotget_number(root_object, (node_name + ".Parent UID").c_str());

	go->isActive = json_object_dotget_boolean(root_object, (node_name + ".Active").c_str());

	if (id > 0)	// Root node has no parent nor components
	{
		if (id > 1)	// If parent == root node --> don't reparent
		{
			go->ReParent(App->scene->rootNode->FindChild(id));
		}

		go->isStatic = json_object_dotget_boolean(root_object, (node_name + ".Static").c_str());

		int comp_num = json_object_dotget_number(root_object, (node_name + ".Components num").c_str());

		for (int i = 0; i < comp_num; i++)
		{
			ComponentsFromMeta(node_name, *go, i, scene);
		}
	}

	return go;
}

void ParsingJSON::ComponentsFromMeta(std::string node_name, GameObject& go, int i, bool scene)
{
	std::string comp_name = node_name + ".Components.Component " + std::to_string(i);

	Component* comp;
	u32 id;
	float* f = nullptr;

	// TODO: Components uid should be unique right?
	C_TYPE comp_type = (C_TYPE)json_object_dotget_number(root_object, (comp_name + ".Type").c_str());
	id = json_object_dotget_number(root_object, (comp_name + ".Component UID").c_str());

	switch (comp_type)
	{
	case C_TYPE::TRANSFORM:
	{
		comp = new C_Transform();

		//---Position---
		f = C_json_object_dotget_array(static_cast<C_Transform*>(comp)->position.Size, comp_name + ".Position");

		static_cast<C_Transform*>(comp)->position.x = f[0];
		static_cast<C_Transform*>(comp)->position.y = f[1];
		static_cast<C_Transform*>(comp)->position.z = f[2];
		RELEASE_ARRAY(f);

		//---Rotation---
		f = C_json_object_dotget_array(4, comp_name + ".Rotation");

		static_cast<C_Transform*>(comp)->rotation.x = f[0];
		static_cast<C_Transform*>(comp)->rotation.y = f[1];
		static_cast<C_Transform*>(comp)->rotation.z = f[2];
		static_cast<C_Transform*>(comp)->rotation.w = f[3];
		RELEASE_ARRAY(f);

		//---Scale---
		f = C_json_object_dotget_array(static_cast<C_Transform*>(comp)->scale.Size, comp_name + ".Scale");

		static_cast<C_Transform*>(comp)->scale.x = f[0];
		static_cast<C_Transform*>(comp)->scale.y = f[1];
		static_cast<C_Transform*>(comp)->scale.z = f[2];
		RELEASE_ARRAY(f);

		go.AddComponent(comp);
	}
	break;
	case C_TYPE::MESH:
	{
		comp = new C_Mesh();
		id = json_object_dotget_number(root_object, (comp_name + ".Resource UID").c_str());

		// When create the component, add the resource already existing;
		static_cast<C_Mesh*>(comp)->mesh->SetUID(id);

		static_cast<C_Mesh*>(comp)->mesh->assetsFile = json_object_dotget_string(root_object, (comp_name + ".Assets dir").c_str());
		static_cast<C_Mesh*>(comp)->mesh->libraryFile = json_object_dotget_string(root_object, (comp_name + ".Library dir").c_str());

		std::string filePath, fileName;
		App->fs->SplitFilePath(static_cast<C_Mesh*>(comp)->mesh->assetsFile.c_str(), &filePath, &fileName);

		if (scene)
		{
			App->resource->ImportToSceneV(fileName, filePath, &go, true);
		}
		else
		{
			go.AddComponent(comp);
			App->resource->LoadMesh(go);
		}
	}
	break;
	case C_TYPE::MATERIAL:
	{
		comp = new C_Material();
		id = json_object_dotget_number(root_object, (comp_name + ".Resource UID").c_str());
		static_cast<C_Material*>(comp)->tex->SetUID(id);

		static_cast<C_Material*>(comp)->color.r = json_object_dotget_number(root_object, (comp_name + ".Color.R").c_str());
		static_cast<C_Material*>(comp)->color.g = json_object_dotget_number(root_object, (comp_name + ".Color.G").c_str());
		static_cast<C_Material*>(comp)->color.b = json_object_dotget_number(root_object, (comp_name + ".Color.B").c_str());
		static_cast<C_Material*>(comp)->color.a = json_object_dotget_number(root_object, (comp_name + ".Color.A").c_str());

		if (json_object_dotget_boolean(root_object, (comp_name + ".Has Assets").c_str()))
		{
			static_cast<C_Material*>(comp)->tex->assetsFile = json_object_dotget_string(root_object, (comp_name + ".Assets dir").c_str());
			static_cast<C_Material*>(comp)->tex->libraryFile = json_object_dotget_string(root_object, (comp_name + ".Library dir").c_str());

			std::string filePath, fileName;
			App->fs->SplitFilePath(static_cast<C_Material*>(comp)->tex->assetsFile.c_str(), &filePath, &fileName);

			if (scene)
			{
				App->resource->ImportToSceneV(fileName, filePath, &go, true);
				go.AddComponent(comp);
			}
			else
			{
				go.AddComponent(comp);
				App->scene->hierarchy->SetSelected(&go);
				App->resource->LoadChildrenTextures(static_cast<C_Material*>(comp)->tex->assetsFile, static_cast<C_Material*>(comp)->tex->libraryFile);
			}
		}
		else
		{
			go.AddComponent(comp);
		}
	}
	break;
	case C_TYPE::CAM:
	{
		comp = new C_Camera();
		static_cast<C_Camera*>(comp)->frustum.nearPlaneDistance = json_object_dotget_number(root_object, (comp_name + ".Near Plane").c_str());
		static_cast<C_Camera*>(comp)->frustum.farPlaneDistance = json_object_dotget_number(root_object, (comp_name + ".Far Plane").c_str());
		static_cast<C_Camera*>(comp)->fov = json_object_dotget_number(root_object, (comp_name + ".FOV").c_str());
		static_cast<C_Camera*>(comp)->isGameCam = json_object_dotget_boolean(root_object, (comp_name + ".Game Camera").c_str());
		static_cast<C_Camera*>(comp)->isCullEnabled = json_object_dotget_boolean(root_object, (comp_name + ".Culling").c_str());

		go.AddComponent(comp);
	}
	break;
	case C_TYPE::NONE:
		break;
	default:
		break;
	}

	comp->SetUID(id);
	comp->type = comp_type;
}

// Check if it has to be reimported (file doesn't exist in library)
std::string ParsingJSON::HasToReImport(const char* path, R_TYPE type)
{
	root_value = json_parse_file(path);
	root_object = json_value_get_object(root_value);

	std::string node_name = "Resource";
	int size = json_object_dotget_number(root_object, (node_name + ".Info.Children number").c_str());

	std::string libPath;
	for (int i = 0; i < size; i++)
	{
		libPath = json_object_dotget_string(root_object, (node_name + " " + std::to_string(i) + ".Library Path").c_str());

		switch (type)
		{
		case R_TYPE::MESH:
			libPath += MESHES_EXT;
			break;
		case R_TYPE::TEXTURE:
			libPath += TEXTURES_EXT;
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

		if (!App->fs->Exists(libPath.c_str()))
		{
			return "";
		}
	}

	return libPath;
}

//---Scene---
void ParsingJSON::SaveScene(std::string name, std::string dir)	// Could be optimized in a single function with prefab creator
{
	std::string file_name = /*SCENES_AUX +*/ dir + name + SCENE_EXT;

	/*if (!App->fs->Exists(SCENES_AUX))
	{
		App->fs->CreateDir(SCENES_AUX);
	}*/

	root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);
	if (root_value != nullptr)
	{
		char* serialized_string = NULL;

		int resource_count = 0;

		std::string node_name = "Scene.Info";
		json_object_dotset_number(root_object, (node_name + ".Children number").c_str(), resource_count);
		json_object_dotset_string(root_object, (node_name + ".Local Directory").c_str(), file_name.c_str());

		resource_count = GameObjectInfo(App->scene->rootNode, node_name + ".Resources") - 1;

		json_object_dotset_number(root_object, (node_name + ".Children number").c_str(), resource_count);

		GameObjectJSON(App->scene->rootNode, "Scene.Parent", 0, "Scene");

		serialized_string = json_serialize_to_string_pretty(root_value);
		puts(serialized_string);

		json_serialize_to_file(root_value, file_name.c_str());
		json_free_serialized_string(serialized_string);

		LOG("Successfully created [%s]", file_name);
	}
	else
	{
		LOG("Could not create meta file of [%s]", file_name);
	}
	json_value_free(root_value);
}

void ParsingJSON::LoadScene(std::string path)
{
	root_value = json_parse_file(path.c_str());
	root_object = json_value_get_object(root_value);

	App->scene->rootNode = new GameObject("Scene", nullptr);

	GameObject* go, * temp;
	if (root_value != nullptr)
	{
		std::vector<GameObject*> vTemp;

		go = GOfromMeta("Scene.Parent");
		go->RemoveComponent(go->transform);
		ClearVec(App->scene->rootNode->vChildren);
		RELEASE(App->scene->rootNode);
		App->scene->rootNode = go;
		go->pParent = nullptr;

		int vChildrensize = json_object_dotget_number(root_object, "Scene.Info.Children number");

		//
		for (int i = 0; i < vChildrensize; i++)
		{
			temp = GOfromMeta("Scene.Child " + std::to_string(i + 1), true);
			vTemp.push_back(temp);
			temp = nullptr;
		}

		for (auto it = vTemp.begin(); it != vTemp.end(); it++)
		{
			(*it)->selected = false;
			(*it)->SetUID(App->randomLCG->Int());
			(*it) = nullptr;
		}

		ClearVec(vTemp);

		LOG("Successfully created [%s]", path);
	}
	else
	{
		LOG("Could not create meta file of [%s]", path);
	}
}

//---Custom functions---
void ParsingJSON::json_object_dotset_array(float* num, int size, std::string name)
{
	JSON_Value* value = json_value_init_array();
	JSON_Array* jArray = json_value_get_array(value);

	json_object_dotset_value(root_object, name.c_str(), value);

	for (int i = 0; i < size; i++)
	{
		json_array_append_number(jArray, num[i]);
	}
}

float* ParsingJSON::C_json_object_dotget_array(int size, string name)
{
	JSON_Array* jArray = json_object_dotget_array(root_object, name.c_str());

	float* ret = new float[size];
	for (int i = 0; i < size; i++)
	{
		ret[i] = (float)json_value_get_number(json_array_get_value(jArray, i));
	}

	return ret;
}
