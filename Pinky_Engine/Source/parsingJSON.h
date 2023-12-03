#pragma once
#include "Globals.h"
#include <string>

#include "External libraries/parson/parson.h"

using namespace std;

class GameObject;
class Component;

class ParsingJSON
{
public:
	ParsingJSON();
	~ParsingJSON();

	// From full path (with || w/o .ext) file, create the .meta file in the same dir
	void CreateJSON(GameObject* go, const char* path);

	// Use only if full path known
	void CreateMetaFromGO(GameObject* go, std::string path);

	int GameObjectJSON(GameObject* go, std::string node_name, int counter = 0);
	int GameObjectInfo(GameObject* go, std::string node_name, int counter = 0);
	void ComponentsJSON(Component* comp, std::string node_name, int i);

	GameObject* CreateGOfromMeta(std::string path);
	GameObject* GOfromMeta(std::string node_name);
	Component* ComponentsFromMeta(std::string node_name, int i);

private:
	
	void json_object_dotset_array(float* num, int size, string name);
	float* C_json_object_dotget_array(int size, string name);

private:
	JSON_Value* root_value;
	JSON_Object* root_object;
};