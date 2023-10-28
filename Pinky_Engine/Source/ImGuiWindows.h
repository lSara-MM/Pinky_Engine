#pragma once
#include "Globals.h"

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"

#include "../MathGeoLib/include/Math/float3x3.h"
#include "../MathGeoLib/include/Math/float4x4.h"

#include <vector>

class GameObject;

class ImGuiWindows
{
public:
	ImGuiWindows(int i);
	~ImGuiWindows();

	virtual void ShowWindow() {};

	// Hierarchy and Inspector use the same functions
	std::vector<GameObject*> GetSelectedGOs();
	void SetSelected(std::vector<GameObject*> vSelected);
	void SetSelected(GameObject* go);
	void SetSelectedState(GameObject* go);

public:
	bool show;
	int id;

private:
	std::vector<GameObject*> selectedGOs;
};

class Hierarchy : public ImGuiWindows
{
public:
	Hierarchy(int i);
	~Hierarchy();

	void ShowWindow();

	GameObject* GetDragged();
	void SetDragged(GameObject* go);

private:
	bool ShowChildren(std::vector<GameObject*> children, int num);
	void MouseEvents(GameObject* current);

private:
	GameObject* draggedGO;
};

class Inspector : public ImGuiWindows
{
public:
	Inspector(int i) : ImGuiWindows(i) {};
	~Inspector();

	void ShowWindow();

public:
	int selected_fish = -1;
};