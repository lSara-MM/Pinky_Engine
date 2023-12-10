#pragma once
#include "Globals.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"

class GameObject;

enum class W_TYPE
{
	HIERARCHY,
	INSPECTOR,
	CONSOLE,
	PROJECT
};

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
	void SetSelectedState(GameObject* go, bool selected);

	// Do not use this one
	void SetSelectedP(GameObject* go);
public:
	bool show;
	int id;
	W_TYPE type;

private:
	std::vector<GameObject*> vSelectedGOs;
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
	bool TreeNode(GameObject* current, ImGuiTreeNodeFlags node_flags);
	void MouseEvents(GameObject* current);

private:
	GameObject* draggedGO;
	ImVec4 disabledColor;
	ImVec4 hiddenColor;
};

class Inspector : public ImGuiWindows
{
public:
	Inspector(int i) : ImGuiWindows(i) {};
	~Inspector();

	void ShowWindow();
	void SetActiveState(GameObject* go, bool isActive);

public:
	int selected_fish = -1;
};