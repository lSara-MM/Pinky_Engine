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

	void DeleteLibDirs(const char* dir, R_TYPE type);
	//------

	void CreateResourceMetaFile(std::vector<Resource*> resources, const char* path);
	std::string GetLibPath(const char* path, int i = 0);

	//---Prefabs---
	void CreatePrefabFromGO(GameObject* go);

	int GameObjectJSON(GameObject* go, std::string node_name, int counter = 0, std::string subInfo = "GameObject");
	int GameObjectInfo(GameObject* go, std::string node_name, int counter = 0);
	void ComponentsJSON(Component* comp, std::string node_name, int i);

	GameObject* CreateGOfromMeta(std::string path, std::string subInfo = "GameObject");
	GameObject* GOfromMeta(std::string node_name, bool scene = false);
	void ComponentsFromMeta(std::string node_name, GameObject &go, int i, bool scene);

	std::string HasToReImport(const char* path, R_TYPE type);

	//---Scene---
	void SaveScene(std::string name, std::string dir = ASSETS_AUX);
	void LoadScene(std::string path);

private:
	
	void json_object_dotset_array(float* num, int size, string name);
	float* C_json_object_dotget_array(int size, string name);

public:
	bool loadMeshes;
private:
	JSON_Value* root_value;
	JSON_Object* root_object;
};