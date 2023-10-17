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

private:
	bool ShowChildren(std::vector<GameObject*> children, int num);

public:
};

class Inspector : public ImGuiWindows
{
public:
	Inspector();
	~Inspector();

	void ShowWindow();

public:
};
