#include "GameObject.h"


GameObject::GameObject(std::string n, GameObject* parent, bool start_enabled)
{
	pParent = parent;
	name = n;
	active = start_enabled;

	C_Transform* temp = new C_Transform(float3(0, 0, 0), Quat(0, 0, 0, 0), float3(1, 1, 1));
	transform = temp;

	parent->vChildren.push_back(this);
}

GameObject::GameObject(std::string n, bool a)
{
	name = n;
	active = a;
	C_Transform* temp = new C_Transform(float3(0, 0, 0), Quat(0, 0, 0, 0), float3(1, 1, 1));
	transform = temp;
}

GameObject::~GameObject()
{
	RELEASE(transform);

	ClearVecPtr(vComponents);
	ClearVecPtr(vChildren);
}

void GameObject::AddComponent(C_TYPE type)
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
		break;
	case C_TYPE::MATERIAL:
		break;
	default:
		break;
	}
}
