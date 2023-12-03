#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"
#include "External Libraries/ImGuizmo/ImGuizmo.h"

#include "External Libraries/MathGeoLib/include/Math/float3x3.h"
#include "External Libraries/MathGeoLib/include/Math/float4x4.h"

#include "ImGuiWindows.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void PrimitivesMenu(GameObject* go = nullptr, bool component = false);

	void UseDockSpace(ImGuiIO& io);
	update_status Toolbar();
	void ConfigWindow(ImGuiIO& io);
	void ConsoleWindow();
	void FpsWindow(ImGuiIO& io);
	void MemWindow();
	void HardwareDetection(bool& infoOutputWin);
	//---Viewports---
	void AboutWindow();
	void EditorWindow();
	void ImGuizmoControl();
	void GameWindow();

	void AddFPS(std::vector<float>& vect, const float aFPS);
	void AddMem(std::vector<float>& vect, const float repMem);
	void OsOpenInShell(const char* path);	// https://stackoverflow.com/questions/71712920/how-to-open-a-webpage-in-imgui

public:

	SDL_GLContext context;
	CPlane Grid;

	// FPS graph
	std::vector<float> mFPSLog;
	std::vector<float> mSLog;
	//Memory graph
	std::vector<float> MemLog;

	//Frame cap
	bool frcap;

	//Log vector
	std::vector<std::string> vLog;

	//Viewport scene size
	ImVec2 ViewportSize;
	ImVec2 ViewportPos;
	ImVec2 origin;
	LineSegment pickingRay;

	//Game scene size
	ImVec2 GameViewSize;

	//guizmos
	ImGuizmo::OPERATION transformOperation;

private:

	std::vector<ImGuiWindows*> vImGuiWindows;

	bool docking;

	bool moduleSettingsWin;
	bool styleEditor;
	bool infoOutputWin;
	bool consoleWin;

	// Configuration OpenGL Options
	bool depthTest;
	bool cullFace;
	bool lightning;
	bool colorMaterial;
	bool texture2D;
	bool blend;
	bool lineSmooth;
	bool normalize;

	// About window
	ImVec4 aboutColor;
	bool aboutWin;

	//vram info
	sMStats memoryStats;
	Uint64 VRAM_budget;
	Uint64 VRAM_usage;
	Uint64 VRAM_available;
	Uint64 VRAM_reserved;

	//Struct to get sdl version info
	SDL_version versionSDL;
};