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
	virtual GameObject* GetSelected();
	virtual void SetSelected(GameObject* go);
	virtual GameObject* GetDragged();
	virtual void SetDragged(GameObject* go);
	virtual void SetUnselected();

public:
	bool show;
	int id;

private:
	GameObject* selectedGO = nullptr;
	GameObject* draggedGO = nullptr;
	std::vector<GameObject*> selectedGOs;
};

class Hierarchy : public ImGuiWindows
{
public:
	Hierarchy(int i) : ImGuiWindows(i) {};
	~Hierarchy();

	void ShowWindow();

private:
	bool ShowChildren(std::vector<GameObject*> children, int num);
	void MouseEvents(GameObject* current);

private:
	int selection_mask = (1 << 1);
	int node_clicked = -1;
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