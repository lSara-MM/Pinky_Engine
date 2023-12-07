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
	update_status FinishUpdate(float dt);
	bool CleanUp();

	GameObject* ImportFileToEngine(const char* path);
	void ImportVModel(const char* meshPath, std::vector<const char*> texPaths);
	int ImportToScene(std::string path, std::string dir);


	GameObject* ImportFile(const char* path, GameObject* goToLink = nullptr);
	void ImportModel(const char* meshPath, std::vector<const char*> texPaths);

	std::string SaveToLibrary(Resource* r);
	Resource* LoadFromLibrary(std::string path, R_TYPE type = R_TYPE::NONE);

	R_TYPE CheckExtensionType(const char* fileDir);
	std::string Get_Set_FilePath(const char* fileDir);

	void LoadChildrenMeshes(GameObject* go, uint size);

	// Return false if pending to delete
	bool AddResource(Resource* r, bool i = true);

public:
	std::map <u32, Resource*> mResources;
	std::vector<Resource*> vPendingToDelete;
	std::vector<Resource*> vResources;	// Temp vector


	std::vector<Resource*> vMeshesResources;
	std::vector<Resource*> vTexturesResources;
private:
	bool pendingToLoadScene;
	GameObject* temp;

	std::string sceneFileName;
	std::string normMetaPath;
};