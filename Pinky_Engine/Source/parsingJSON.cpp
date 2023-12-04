#include "parsingJSON.h"

#include "Globals.h"
#include "GameObject.h"

ParsingJSON::ParsingJSON()
{
}

ParsingJSON::~ParsingJSON()
{
}

//---Create and write in .meta---
void ParsingJSON::CreateResourceMetaFile(std::vector<Resource*> resources, const char* path, std::string ext)
{
	std::string filePath, fileName, fileExt;
	App->fs->SplitFilePath(path, &filePath, &fileName, &fileExt);

	std::string file_name = filePath + fileName + "." + fileExt + ext;

	// TODO: Remove when we have dupe
	if (ext != ".meta")
	{
		file_name = "Assets//" + fileName + "." + ext;
	}

	root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);
	char* serialized_string = NULL;

	int resource_count = 0;

	std::string node_name = "GameObject.Info";
	json_object_dotset_string(root_object, (node_name + ".Local Directory").c_str(), file_name.c_str());

	json_object_dotset_number(root_object, (node_name + ".Children number").c_str(), resource_count);

	switch (App->resource->CheckExtensionType(path))
	{
	case R_TYPE::MESH:
		node_name += ".Mesh ";
		break;
	case R_TYPE::TEXTURE:
		node_name += ".Texture ";
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
		json_object_dotset_number(root_object, (node_name + std::to_string(i) + ".UID").c_str(), resources.at(i)->GetUID());
		json_object_dotset_string(root_object, (node_name + std::to_string(i) + ".Library Path").c_str(), (MESHES_PATH +
			std::to_string(resources.at(i)->GetUID())).c_str());
	}

	serialized_string = json_serialize_to_string_pretty(root_value);
	puts(serialized_string);

	json_serialize_to_file(root_value, file_name.c_str());
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);
}

std::string ParsingJSON::GetResourceMetaFile(const char* path)
{
	root_value = json_parse_file(path);
	root_object = json_value_get_object(root_value);

	//go->isActive = json_object_dotget_boolean(root_object, (node_name + ".Active").c_str());

	return std::string();
}

//---Create and write GameObject information---
void ParsingJSON::CreatePrefabFromGO(GameObject* go, const char* path)
{
	std::string filePath, fileName, fileExt;
	App->fs->SplitFilePath(path, &filePath, &fileName, &fileExt);

	int resource_count = 0;

	std::string file_name = filePath + go->name + ".pgo";	// Pinky Game Object
	root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);
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
	json_value_free(root_value);
}

int ParsingJSON::GameObjectJSON(GameObject* go, std::string node_name, int counter)
{
	counter++;
	json_object_dotset_string(root_object, (node_name + ".Name").c_str(), go->name.c_str());
	json_object_dotset_number(root_object, (node_name + ".UID").c_str(), go->GetUID());
	json_object_dotset_number(root_object, (node_name + ".Parent UID").c_str(), go->pParent->GetUID());
	json_object_dotset_boolean(root_object, (node_name + ".Active").c_str(), go->isActive);
	json_object_dotset_boolean(root_object, (node_name + ".Static").c_str(), go->isStatic);
	json_object_dotset_number(root_object, (node_name + ".Parent UID").c_str(), go->pParent->GetUID());
	json_object_dotset_number(root_object, (node_name + ".Components num").c_str(), go->vComponents.size());

	for (int i = 0; i < go->vComponents.size(); i++)
	{
		ComponentsJSON(go->vComponents[i], node_name, i);
	}

	for (int i = 0; i < go->vChildren.size(); i++)
	{
		counter = GameObjectJSON(go->vChildren[i], "GameObject.Child " + std::to_string(counter), counter);
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
		json_object_dotset_string(root_object, (comp_name + ".Library dir").c_str(), (MESHES_PATH +
			std::to_string(static_cast<C_Mesh*>(comp)->mesh->GetUID())).c_str());
		break;
	case C_TYPE::MATERIAL:
		json_object_dotset_number(root_object, (comp_name + ".Resource UID").c_str(), static_cast<C_Material*>(comp)->tex->GetUID());
		json_object_dotset_string(root_object, (comp_name + ".Library dir").c_str(), (TEXTURES_PATH +
			std::to_string(static_cast<C_Material*>(comp)->tex->GetUID())).c_str());
		break;
	case C_TYPE::CAM:
		break;
	case C_TYPE::NONE:
		break;
	default:
		break;
	}
}

//---Load from .meta---
GameObject* ParsingJSON::CreateGOfromMeta(std::string path)
{
	root_value = json_parse_file(path.c_str());
	root_object = json_value_get_object(root_value);

	GameObject* go, * temp;
	std::vector<GameObject*> vTemp;

	go = GOfromMeta("GameObject.Parent");
	vTemp.push_back(go);

	int vChildrensize = json_object_dotget_number(root_object, "GameObject.Info.Children number");

	// Count starts with 1
	for (int i = 0; i < vChildrensize; i++)
	{
		temp = GOfromMeta("GameObject.Child " + std::to_string(i + 1));
		vTemp.push_back(temp);
	}

	for (auto it = vTemp.begin(); it != vTemp.end(); it++)
	{
		(*it)->SetUID(App->randomLCG->Int());
		(*it) = nullptr;
	}

	ClearVec(vTemp);
	return go;
}

GameObject* ParsingJSON::GOfromMeta(std::string node_name)
{
	GameObject* go = new GameObject();
	go->name = json_object_dotget_string(root_object, (node_name + ".Name").c_str());
	u32 id = json_object_dotget_number(root_object, (node_name + ".UID").c_str());
	go->SetUID(id);
	id = json_object_dotget_number(root_object, (node_name + ".Parent UID").c_str());

	if (id == 0)
	{
		App->scene->rootNode->AddChild(go);
	}
	else
	{
		App->scene->rootNode->FindChild(id)->AddChild(go);
	}

	go->isActive = json_object_dotget_boolean(root_object, (node_name + ".Active").c_str());
	go->isStatic = json_object_dotget_boolean(root_object, (node_name + ".Static").c_str());

	int comp_num = json_object_dotget_number(root_object, (node_name + ".Components num").c_str());

	for (int i = 0; i < comp_num; i++)
	{
		Component* comp = ComponentsFromMeta(node_name, i);
		go->AddComponent(comp);
		comp = nullptr;
	}

	return go;
}

Component* ParsingJSON::ComponentsFromMeta(std::string node_name, int i)
{
	std::string comp_name = node_name + ".Components.Component " + std::to_string(i);

	Component* comp;
	u32 id;
	float* f = nullptr;

	C_TYPE comp_type = (C_TYPE)json_object_dotget_number(root_object, (comp_name + ".Type").c_str());
	id = json_object_dotget_number(root_object, (comp_name + ".Component UID").c_str());

	switch (comp_type)
	{
	case C_TYPE::TRANSFORM:
		comp = new C_Transform();

		//---Position---
		f = C_json_object_dotget_array(static_cast<C_Transform*>(comp)->position.Size, comp_name + ".Position");

		static_cast<C_Transform*>(comp)->position.x = f[0];
		static_cast<C_Transform*>(comp)->position.y = f[1];
		static_cast<C_Transform*>(comp)->position.z = f[2];

		//---Rotation---
		f = C_json_object_dotget_array(static_cast<C_Transform*>(comp)->position.Size, comp_name + ".Rotation");

		static_cast<C_Transform*>(comp)->rotation.x = f[0];
		static_cast<C_Transform*>(comp)->rotation.y = f[1];
		static_cast<C_Transform*>(comp)->rotation.z = f[2];
		static_cast<C_Transform*>(comp)->rotation.w = f[3];

		//---Scale---
		f = C_json_object_dotget_array(static_cast<C_Transform*>(comp)->position.Size, comp_name + ".Scale");

		static_cast<C_Transform*>(comp)->scale.x = f[0];
		static_cast<C_Transform*>(comp)->scale.y = f[1];
		static_cast<C_Transform*>(comp)->scale.z = f[2];
		break;
	case C_TYPE::MESH:
		comp = new C_Mesh();
		id = json_object_dotget_number(root_object, (comp_name + ".Resource UID").c_str());
		static_cast<C_Mesh*>(comp)->mesh->SetUID(id);

		static_cast<C_Mesh*>(comp)->mesh->libraryFile = json_object_dotget_string(root_object, (comp_name + ".Library dir").c_str());
		break;
	case C_TYPE::MATERIAL:
		comp = new C_Material();
		id = json_object_dotget_number(root_object, (comp_name + ".Resource UID").c_str());
		static_cast<C_Material*>(comp)->tex->SetUID(id);

		static_cast<C_Material*>(comp)->tex->libraryFile = json_object_dotget_string(root_object, (comp_name + ".Library dir").c_str());
		break;
	case C_TYPE::CAM:
		break;
	case C_TYPE::NONE:
		break;
	default:
		break;
	}

	comp->SetUID(id);
	comp->type = comp_type;

	//RELEASE(f);
	return comp;
}

std::string ParsingJSON::GetLibraryPath(const char* path, R_TYPE type)
{
	return std::string();
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
