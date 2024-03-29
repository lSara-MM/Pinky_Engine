#include "Application.h"
#include "TimeManager.h"

extern Application* App = nullptr;

Application::Application()
{
	App = this;

	//------
	randomLCG = new LCG();
	fs = new FileSystemManager();
	parson = new ParsingJSON();
	//------

	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);

	resource = new ModuleResource(this);

	scene = new ModuleScene(this);
	editor = new ModuleEditor(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);

	AddModule(resource);

	AddModule(camera);
	AddModule(input);

	// Renderer last!
	AddModule(scene);
	AddModule(renderer3D);
	AddModule(editor);
}

Application::~Application()
{
	for (std::vector<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); ++it)
	{
		delete (*it);
		(*it) = nullptr;
	}
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret; ++it)
	{
		(*it)->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret; ++it)
	{
		(*it)->Start();
	}
	
	//fps control
	ms_timer.Start();
	fps = 60;
	maxFrameDuration = 1000/fps;
	frcap = true;

	TimeManager::StartEngineTime();

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	maxFrameDuration = 1000 / fps;

	if (maxFrameDuration > (float)ms_timer.Read() && frcap)
	{
		SDL_Delay(maxFrameDuration - (float)ms_timer.Read());
	}	

	dt = (float)ms_timer.Read() / 1000.0f;

	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	TimeManager::Update();
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->PreUpdate(dt);
	}

	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->Update(dt);
	}

	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->PostUpdate(dt);
	}

	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->FinishUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	for (std::vector<Module*>::reverse_iterator it = list_modules.rbegin(); it != list_modules.rend() && ret; ++it)
	{
		ret = (*it)->CleanUp();
	}
	TimeManager::CleanUp();
	RELEASE(randomLCG);
	RELEASE(fs);
	// TODO: wtf peta?
	//RELEASE(parson);
	return ret;
}


int Application::ListModulesSize()
{
	return list_modules.size();
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}