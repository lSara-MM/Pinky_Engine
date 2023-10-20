#include "GameObject.h"
#include "ModuleScene.h"

GameObject::GameObject(std::string n, GameObject* parent, bool start_enabled)
{
	pParent = parent;
	name = n;
	active = start_enabled;

	AddComponent(C_TYPE::TRANSFORM);

	parent->vChildren.push_back(this);

	id = App->scene->GO_num++;
}

GameObject::GameObject(std::string n, bool a)
{
	name = n;
	active = a;

	AddComponent(C_TYPE::TRANSFORM);

	id = App->scene->GO_num++;
}

GameObject::~GameObject()
{
	ClearVecPtr(vComponents);

	//RELEASE(transform);
	transform = nullptr;

	if (!vChildren.empty())
	{
		ClearVecPtr(vChildren);
	}

	//pParent->vChildren.erase(pParent->vChildren.begin() + FindInVec(pParent->vChildren, this));
	// TODO: preguntar perque peta si esta aixo pero si es fa des del parent -> delete children si funciona

	App->scene->GO_num--;
}

Component* GameObject::AddComponent(C_TYPE type, Component* c)
{
	Component* temp;

	switch (type)
	{
	case C_TYPE::TRANSFORM:
		if (transform == nullptr)
		{
			temp = new C_Transform(float3(0, 0, 0), Quat(0, 0, 0, 0), float3(1, 1, 1));
			transform = (C_Transform*)temp;
			vComponents.push_back(transform);
		}
		break;
	case C_TYPE::MESH:
		temp = new C_Mesh();
		vComponents.push_back(temp);
		break;
	case C_TYPE::MATERIAL:
		temp = new C_Material();
		vComponents.push_back(temp);
		break;
	default:
		break;
	}

	return c;
}

void GameObject::DeleteChild(GameObject* go)
{
	go->~GameObject();
	vChildren.erase(std::find(vChildren.begin(), vChildren.end(), go));
}
