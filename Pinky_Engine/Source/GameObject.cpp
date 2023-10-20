#include "GameObject.h"
#include "ModuleScene.h"

GameObject::GameObject(std::string n, GameObject* parent, bool start_enabled)
{
	pParent = parent;
	name = n;
	active = start_enabled;

	C_Transform* temp = new C_Transform(float3(0, 0, 0), Quat(0, 0, 0, 0), float3(1, 1, 1));
	transform = temp;

	parent->vChildren.push_back(this);

	id = App->scene->GO_num++;
}

GameObject::GameObject(std::string n, bool a)
{
	name = n;
	active = a;
	C_Transform* temp = new C_Transform(float3(0, 0, 0), Quat(0, 0, 0, 0), float3(1, 1, 1));
	transform = temp;

	id = App->scene->GO_num++;
}

GameObject::~GameObject()
{
	RELEASE(transform);

	ClearVecPtr(vComponents);

	if (!vChildren.empty())
	{
		ClearVecPtr(vChildren);
	}

	//pParent->vChildren.erase(pParent->vChildren.begin() + FindInVec(pParent->vChildren, this));
	App->scene->GO_num--;
}

void GameObject::AddComponent(C_TYPE type, Component* c)
{
	switch (type)
	{
	case C_TYPE::TRANSFORM:
		if (transform == nullptr)
		{
			C_Transform* temp = new C_Transform();
			transform = temp;
			vComponents.push_back(transform);
		}
		break;
	case C_TYPE::MESH:
		vComponents.push_back(new C_Mesh());
		break;
	case C_TYPE::MATERIAL:
		vComponents.push_back(new C_Material());
		break;
	default:
		break;
	}
}