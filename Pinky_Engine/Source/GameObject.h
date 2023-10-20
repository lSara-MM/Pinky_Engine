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
	GameObject(std::string n = "EmptyGameObject", GameObject* parent = App->scene->rootNode, bool start_enabled = true);
	GameObject(std::string n, bool a);
	~GameObject();

	void AddComponent(C_TYPE type, Component* c);

public:
	GameObject* pParent;
	std::vector<GameObject*> vChildren;
	std::vector<Component*> vComponents;

	std::string name;
	int id;
	bool active;
	C_Transform* transform;

	bool selected = false;
} /**rootNode*/;	// TODO: preguntar com es millor, si aixi o un rootnode al scene