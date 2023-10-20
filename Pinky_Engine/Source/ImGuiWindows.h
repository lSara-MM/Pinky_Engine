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
	ImGuiWindows();
	~ImGuiWindows();

	virtual void ShowWindow() {};
	virtual GameObject* GetSelected();
	virtual void SetSelected(GameObject* go);

public:
	bool show;

private:
	GameObject* selectedGO = nullptr;
};

class Hierarchy : public ImGuiWindows
{
public:
	Hierarchy();
	~Hierarchy();

	void ShowWindow();

private:
	bool ShowChildren(std::vector<GameObject*> children, int num);

private:
	int selection_mask = (1 << 1);
	int node_clicked = -1;
};

class Inspector : public ImGuiWindows
{
public:
	Inspector();
	~Inspector();

	void ShowWindow();
	void AddComponentByType(int i);

public:
	int selected_fish = -1;
};