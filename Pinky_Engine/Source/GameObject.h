#pragma once
#include "Globals.h"
#include "Light.h"

#include "Application.h"
#include "ModuleScene.h"


#include "Component.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Camera.h"

#include "R_Mesh.h"

class GameObject
{
public:
	//GameObject();
	GameObject(std::string n = "Empty GameObject", GameObject* parent = App->scene->rootNode, bool start_enabled = true);
	GameObject(GameObject* go, int size, GameObject* parent);
	~GameObject();

	//
	u32 GetUID();
	void SetUID(u32 id);
	void ReSetUID();

	//
	update_status Update(float dt);

	//---Components---
	bool AddComponent(C_TYPE type, void* var = nullptr, ai::POLY_PRIMITIVE_TYPE poly = ai::POLY_PRIMITIVE_TYPE::SPHERE);

	// Add existing component
	bool AddComponent(Component* component);
	void RemoveComponent(Component* component);

	C_Mesh* GetComponentMesh();
	std::vector<C_Material*> GetComponentsMaterial();
	C_Camera* GetComponentCamera();

	Component* GetComponentByType(C_TYPE type);

	//---Resources---
	bool ChangeComponentResource(Resource* oldResource, Resource* newResource);

	//---Parent/Child---
	void ReParent(GameObject* newParent);
	void AddChild(GameObject* go);
	void DeleteChild(GameObject* go);
	//Remove from children vector (do not use)
	void RemoveChild(GameObject* go);

	// Return nullptr if gameobject not found
	GameObject* FindChild(u32 idToFind, GameObject* go = nullptr);

public:
	GameObject* pParent;
	std::vector<GameObject*> vChildren;
	std::vector<Component*> vComponents;

	std::string name;
	bool isActive, isStatic;

	C_Transform* transform;
	C_Mesh* mesh;
	C_Camera* camera;

	bool selected;
	bool hidden;
	//Discard mesh if the global AABB is outside frustum
	bool isCulled;

	uint numMaterials;

private:
	u32 uid;
};