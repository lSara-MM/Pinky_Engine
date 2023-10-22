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

	if (vComponents.size() > 1)
	{
		std::vector<C_Mesh*> vMeshes = GetComponentsMesh();
		std::vector<C_Material*> vMaterials = GetComponentsMaterial();
		for (auto i = 0; i < vMeshes.size(); i++)
		{
			if (vMeshes[i]->active)
			{
				if (!vMaterials.empty())
				{
					vMeshes[i]->Draw(vMaterials[i]->checkered, vMaterials[i]->color);
				}
				else
				{
					// If it has no material, draw checkers
					vMeshes[i]->Draw(true);
				}
			}
		}
	}

	return UPDATE_CONTINUE;
}

bool GameObject::AddComponent(C_TYPE type, ai::mesh* m, ai::POLY_PRIMITIVE_TYPE poly)
{
	Component* temp;
	bool ret = true;

	switch (type)
	{
	case C_TYPE::TRANSFORM:
		if (transform == nullptr)
		{
			temp = new C_Transform(this, float3(0, 0, 0), Quat(0, 0, 0, 0), float3(1, 1, 1));
			transform = (C_Transform*)temp;
			vComponents.push_back(transform);
		}
		else { ret = false; }
		break;
	case C_TYPE::MESH:
		// A gameObject can't have more than one mesh
		if (numMeshes == 0)	
		{
			if (m != nullptr)
			{
				temp = new C_Mesh(this, m, numMeshes);
				vComponents.push_back(temp);
				numMeshes++;
			}
			else
			{
				ai::CreatePolyPrimitive(poly, this);
			}
		}
		else { ret = false; }
		break;
	case C_TYPE::MATERIAL:
		// A gameObject can't have more than one material
		// In unity there can be more than one if embeded (?) see snowman for reference 
		if (numMaterials == 0)	
		{
			temp = new C_Material(this, nullptr, numMaterials);
			vComponents.push_back(temp);
			numMaterials++;
		}
		else { ret = false; }
		break;
	default:
		break;
	}

	return ret;
}

std::vector<C_Mesh*> GameObject::GetComponentsMesh()
{
	std::vector<C_Mesh*> vec = {};
	for (auto i = 0; i < vComponents.size(); i++)
	{
		if (vComponents[i]->type == C_TYPE::MESH)
		{
			vec.push_back((C_Mesh*)vComponents[i]);
		}
	}
	return vec;
}

std::vector<C_Material*> GameObject::GetComponentsMaterial()
{
	std::vector<C_Material*> vec = {};
	for (auto i = 0; i < vComponents.size(); i++)
	{
		if (vComponents[i]->type == C_TYPE::MATERIAL)
		{
			vec.push_back((C_Material*)vComponents[i]);
		}
	}
	return vec;
}

void GameObject::DeleteChild(GameObject* go)
{
	go->~GameObject();
	vChildren.erase(std::find(vChildren.begin(), vChildren.end(), go));
}
