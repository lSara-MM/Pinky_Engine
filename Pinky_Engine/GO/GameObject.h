#pragma once
#include "Module.h"
#include "Globals.h"
#include "Application.h"
#include "Light.h"

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"

#include <vector>
#include "Component.h"

class GameObject
{
public:
	//GameObject();
	GameObject(/*GameObject* parent = App->scene->rootNode*/);
	~GameObject();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	void AddComponent(C_TYPE type);
	bool CleanUp();

public:
	GameObject* pParent;
	std::vector<GameObject*> vChildren;
	std::vector<Component*> vComponents;


	std::string name;
	bool active;
	C_Transform* transform;
};