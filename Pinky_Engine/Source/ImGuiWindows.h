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

public:
	bool show;
};

class Hierarchy : public ImGuiWindows
{
public:
	Hierarchy();
	~Hierarchy();

	void ShowWindow();
	GameObject* GetSelected();

private:
	bool ShowChildren(std::vector<GameObject*> children, int num);

private:
	int selection_mask = (1 << 0);
	int node_clicked = -1;

	GameObject* selectedGO = nullptr;
};

class Inspector : public ImGuiWindows
{
public:
	Inspector();
	~Inspector();

	void ShowWindow();

public:
};