#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"

#include "../MathGeoLib/include/Math/float3x3.h"
#include "../MathGeoLib/include/Math/float4x4.h"
#include "Primitive.h"
#include<vector>
//todo: REMOVE this before 1st delivery!!
#include "glmath.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	update_status Toolbar();
	void ConfigWindow();
	void OutputWindow();
	void FpsWindow(ImGuiIO& io);

	void About();

	void AddFPS(std::vector<float>& vect, const float aFPS);
	void OsOpenInShell(const char* path);	// https://stackoverflow.com/questions/71712920/how-to-open-a-webpage-in-imgui

public:

	SDL_GLContext context;
	CPlane Grid;
	

	// 2nd window state
	bool show_demo_window;
	ImVec4 clear_color;
	float col[3];

	// FPS graph
	std::vector<float> mFPSLog;
	std::vector<float> mSLog;

	//Log vector
	std::vector<const char*> logVec;
private:
	ImVec4 aboutColor;
	bool aboutWin;
};