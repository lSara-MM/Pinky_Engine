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

	std::string ImportFileToEngine(const char* path);
	//int ImportToScene(std::string path, std::string dir, GameObject* goParent = nullptr, bool component = false);
	int ImportToSceneV(std::string path, std::string dir, GameObject* goParent = nullptr, bool component = false);

	void ImportModel(const char* meshPath, std::vector<const char*> texPaths);

	std::string SaveToLibrary(Resource* r);
	Resource* LoadFromLibrary(std::string path, R_TYPE type = R_TYPE::NONE);

	R_TYPE CheckExtensionType(const char* fileDir);

	void LoadMesh(GameObject& go);
	void LoadChildrenTextures(std::string path, std::string libPath = "");

	// Return false if pending to delete
	bool AddResource(Resource* r, bool i = true);

public:
	std::map <u32, Resource*> mResources;
	std::vector<Resource*> vPendingToDelete;

	std::vector<Resource*> vTempM;	// Temp vector
	std::vector<Resource*> vTempT;	// Temp vector


	std::vector<Resource*> vMeshesResources;
	std::vector<Resource*> vTexturesResources;

	// Scene management
	std::string assetsPathAux;
	std::string sceneFileName;
	bool pendingToLoadScene;
};