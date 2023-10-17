#pragma once
#include "Globals.h"
#include "Light.h"

#include "Application.h"
#include "ModuleScene.h"

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"

#include <vector>
#include "Component.h"

class GameObject
{
public:
	//GameObject();
	GameObject(GameObject* parent = App->scene->rootNode, std::string n = "EmptyGameObject", bool start_enabled = true);
	GameObject(bool start_enabled);
	~GameObject();

	void AddComponent(C_TYPE type);

public:
	GameObject* pParent;
	std::vector<GameObject*> vChildren;
	std::vector<Component*> vComponents;

	std::string name;
	int id;
	bool active;
	C_Transform* transform;
};