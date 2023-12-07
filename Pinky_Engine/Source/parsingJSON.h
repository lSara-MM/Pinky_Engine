#pragma once
#include "Globals.h"
#include <string>

#include "External libraries/parson/parson.h"
#include "Resource.h"

using namespace std;

class GameObject;
class Component;

class ParsingJSON
{
public:
	ParsingJSON();
	~ParsingJSON();

	// NOT USED
	void CreateFile(std::string fileName, std::string fileExt, const char* realDir);
	const char* GetRealDirFF(const char* dir);
	//------

	void CreateResourceMetaFile(std::vector<Resource*> resources, const char* path);
	std::string GetResourceMetaFile(const char* path);

	//---Prefabs---
	void CreatePrefabFromGO(GameObject* go);

	int GameObjectJSON(GameObject* go, std::string node_name, int counter = 0);
	int GameObjectInfo(GameObject* go, std::string node_name, int counter = 0);
	void ComponentsJSON(Component* comp, std::string node_name, int i);

	GameObject* CreateGOfromMeta(std::string path);
	GameObject* GOfromMeta(std::string node_name);
	Component* ComponentsFromMeta(std::string node_name, int i);

	std::string GetLibraryPath(const char* path, R_TYPE type);

	//---Scene---
	void SaveScene(std::string name);

private:
	
	void json_object_dotset_array(float* num, int size, string name);
	float* C_json_object_dotget_array(int size, string name);

private:
	JSON_Value* root_value;
	JSON_Object* root_object;
};