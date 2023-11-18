#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"

#include "ModuleFS.h"

#include "External Libraries/MathGeoLib/include/Algorithm/Random/LCG.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;

	ModuleEditor* editor;
	ModuleScene* scene;


	LCG* randomLCG;

private:

	Timer	ms_timer;
	float	dt;
	std::vector<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	
	int ListModulesSize();

	//frcap
	uint maxFrameDuration;
	int fps;
	bool frcap;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
}; extern Application* App;