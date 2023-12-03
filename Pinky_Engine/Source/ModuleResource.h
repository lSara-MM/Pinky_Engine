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


	GameObject* ImportFile(const char* path, GameObject* goToLink = nullptr);
	void ImportModel(const char* meshPath, std::vector<const char*> texPaths);

	std::string SaveToLibrary(Resource* r);
	Resource* LoadFromLibrary(std::string path, R_TYPE type = R_TYPE::NONE);

public:
	std::map <uint, Resource> mResources;
private:
	
};