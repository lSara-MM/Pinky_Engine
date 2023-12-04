#pragma once
#include "Module.h"
#include "Globals.h"

#include "Resource.h"
#include "R_Mesh.h"

class GameObject;

class ModuleResource : public Module
{
public:
	ModuleResource(Application* app, bool start_enabled = true);
	~ModuleResource();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	GameObject* ImportVFile(const char* path, GameObject* goToLink = nullptr);

	GameObject* ImportFile(const char* path, GameObject* goToLink = nullptr);
	void ImportModel(const char* meshPath, std::vector<const char*> texPaths);

	std::string SaveToLibrary(Resource* r);
	Resource* LoadFromLibrary(std::string path, R_TYPE type = R_TYPE::NONE);

	R_TYPE CheckExtensionType(const char* fileDir);
	std::string Get_Set_FilePath(const char* fileDir);

	void LoadChildrenMeshes(GameObject* go, uint size);

public:
	std::map <u32, Resource*>* mResources;

private:
	std::string metaPath; 
	std::string normMetaPath;
};