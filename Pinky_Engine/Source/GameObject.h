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
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"

class GameObject
{
public:
	//GameObject();
	GameObject(std::string n = "EmptyGameObject", GameObject* parent = App->scene->rootNode, bool start_enabled = true);
	GameObject(std::string n, bool a);
	~GameObject();

	update_status Update(float dt);

	bool AddComponent(C_TYPE type, ai::mesh* m = nullptr, ai::POLY_PRIMITIVE_TYPE poly = ai::POLY_PRIMITIVE_TYPE::SPHERE);
	
	std::vector<C_Mesh*> GetComponentsMesh();
	std::vector<C_Material*> GetComponentsMaterial();
	
	void DeleteChild(GameObject* go);

public:
	GameObject* pParent = nullptr;
	std::vector<GameObject*> vChildren;
	std::vector<Component*> vComponents;

	std::string name;
	int id;
	bool active;
	C_Transform* transform = nullptr;

	bool selected = false;
	bool hidden = false;

	unsigned int numMeshes = 0;
	unsigned int numMaterials = 0;

private:
} /**rootNode*/;	// TODO: preguntar com es millor, si aixi o un rootnode al scene