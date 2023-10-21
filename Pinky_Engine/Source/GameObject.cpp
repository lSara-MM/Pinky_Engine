#include "GameObject.h"
#include "ModuleScene.h"


#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"


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
	//TODO: preguntar perque peta si esta aixo pero si es fa des del parent -> delete children si funciona

	App->scene->GO_num--;
}

update_status GameObject::Update(float dt)
{
	if (!vChildren.empty())
	{
		for (auto i = 0; i < vChildren.size(); i++)
		{
			vChildren[i]->Update(dt);
		}
	}

	for (auto i = 0; i < vMeshes.size(); i++)
	{
		if (vMeshes[i]->type == C_TYPE::MESH && vMeshes[i]->active)
		{
			vMeshes[i]->Draw();
		}
	}

	return UPDATE_CONTINUE;
}

void GameObject::AddComponent(C_TYPE type, ai::mesh* m, ai::POLY_PRIMITIVE_TYPE poly)
{
	Component* temp;

	switch (type)
	{
	case C_TYPE::TRANSFORM:
		if (transform == nullptr)
		{
			temp = new C_Transform(this, float3(0, 0, 0), Quat(0, 0, 0, 0), float3(1, 1, 1));
			transform = (C_Transform*)temp;
			vComponents.push_back(transform);
		}
		break;
	case C_TYPE::MESH:
		if (m != nullptr)
		{
			temp = new C_Mesh(this, m, numMeshes);
			vComponents.push_back(temp);
			vMeshes.push_back((C_Mesh*)temp);
			numMeshes++;
		}
		else
		{
			ai::CreatePolyPrimitive(poly, this);
		}
		break;
	case C_TYPE::MATERIAL:
		temp = new C_Material(this, numMaterials);
		vComponents.push_back(temp);
		numMaterials++;
		break;
	default:
		break;
	}
}

void GameObject::DeleteChild(GameObject* go)
{
	go->~GameObject();
	vChildren.erase(std::find(vChildren.begin(), vChildren.end(), go));
}
