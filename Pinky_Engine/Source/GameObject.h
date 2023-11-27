#pragma once
#include "Globals.h"
#include "Light.h"

#include "Application.h"
#include "ModuleScene.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"

#include "Component.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Camera.h"

class GameObject
{
public:
	//GameObject();
	GameObject(std::string n = "Empty GameObject", GameObject* parent = App->scene->rootNode, bool start_enabled = true);
	GameObject(GameObject* go, int size, GameObject* parent);
	~GameObject();


	u32 GetUid();

	update_status Update(float dt);

	//---Components---
	bool AddComponent(C_TYPE type, ai::mesh* m = nullptr, ai::POLY_PRIMITIVE_TYPE poly = ai::POLY_PRIMITIVE_TYPE::SPHERE);
	void RemoveComponent(Component* component);

	std::vector<C_Mesh*> GetComponentsMesh();
	std::vector<C_Material*> GetComponentsMaterial();
	std::vector<C_Camera*> GetCompoonentsCamera();//TODO: this bad me thinks


	//---Parent/Child---
	void ReParent(GameObject* newParent);
	void AddChild(GameObject* go);
	void DeleteChild(GameObject* go);
	void RemoveChild(GameObject* go);
	// Return nullptr if gameobject not found
	GameObject* FindChild(u32 idToFind, GameObject* go);

public:
	GameObject* pParent = nullptr;
	std::vector<GameObject*> vChildren;
	std::vector<Component*> vComponents;

	std::string name;
	bool active;
	C_Transform* transform = nullptr;

	bool selected;
	bool hidden;

	uint numMeshes;
	uint numMaterials;

private:
	u32 uid;
};