#pragma once
#include "Module.h"
#include "Globals.h"

#include "Resource.h"

class ModuleResource : public Module
{
public:
	ModuleResource(Application* app, bool start_enabled = true);
	~ModuleResource();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();


	void ImportFile(const char* path);
	bool SaveToLibrary(Resource* r);
	bool LoadFromLibrary(Resource* r);

private:
	
};