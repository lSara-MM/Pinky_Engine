#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"

#include<vector>

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;

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

	//Frame cap 
	Timer timer;
	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;

	Uint64 frameCount = 0;
	Uint32 framesPerSecond = 0;
	Uint32 lastSecFrameCount = 0;

	float averageFps = 0.0f;
	float secondsSinceStartup = 0.0f;
	int  secondsSinceStartupTempo = 0;
	Uint32 maxFrameDuration = 0;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
}; extern Application* App;