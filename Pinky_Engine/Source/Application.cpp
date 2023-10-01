#include "Application.h"

extern Application* App = nullptr;

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);

	// Renderer last!
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
	App = this;

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
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
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

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	for (std::vector<Module*>::reverse_iterator it = list_modules.rbegin(); it != list_modules.rend() && ret; ++it)
	{
		ret = (*it)->CleanUp();
		*it = nullptr;
	}
	return ret;
}

void Application::HardwareDetection()
{
	{
		ImGui::Begin("Hardware");
		//Diapo sale cajita con active, no entiendo
		//ImGui::Text("SDL Version: ", SDL_GetVersion);//TODO: cómo pillar versión sdl
		ImGui::Separator();
		ImGui::Text("CPUs: %d (Cache: %d kb) ", SDL_GetCPUCount(),SDL_GetCPUCacheLineSize());
		ImGui::Text("System RAM: %d", SDL_GetSystemRAM);
		ImGui::Text("Caps: ", SDL_GetSystemRAM);
		ImGui::SameLine();
		if (SDL_Has3DNow)ImGui::Text("3Dnow ");
		ImGui::SameLine();
		if (SDL_HasAltiVec)ImGui::Text("AltiVec ");
		ImGui::SameLine();
		if (SDL_HasAVX)ImGui::Text("AVX ");
		ImGui::SameLine();
		if (SDL_HasAVX2)ImGui::Text("AVX2 ");
		ImGui::SameLine();
		if (SDL_HasMMX)ImGui::Text("MMX ");
		ImGui::SameLine();
		if (SDL_HasRDTSC)ImGui::Text("RDTSC ");
		ImGui::SameLine();
		if (SDL_HasSSE)ImGui::Text("SSE ");
		ImGui::SameLine();
		if (SDL_HasSSE2)ImGui::Text("SSE2 ");
		ImGui::SameLine();
		if (SDL_HasSSE3)ImGui::Text("SSE3 ");
		ImGui::SameLine();
		if (SDL_HasSSE41)ImGui::Text("SSE41 ");
		ImGui::SameLine();
		if (SDL_HasSSE42)ImGui::Text("SSE42");
		ImGui::Separator();
		ImGui::Text("Number of drivers: %d", SDL_GetNumVideoDrivers());//TODO: cómo pillar info drivers/ram
		for (int n = 0; n < SDL_GetNumVideoDrivers(); n++)
			ImGui::Text(SDL_GetVideoDriver(n));
		
		ImGui::End();
	}
}

int Application::ListModulesSize()
{
	return list_modules.size();
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}