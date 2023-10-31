#include "GameObject.h"
#include "ModuleScene.h"


#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Camera.h"


GameObject::GameObject(std::string n, GameObject* parent, bool start_enabled)
{
	uid = App->randomLCG->Int();

	if (parent != nullptr)
	{
		parent->AddChild(this);
	}

	name = n;
	active = start_enabled;

	// ---
	selected = false;
	hidden = false;

	AddComponent(C_TYPE::TRANSFORM);

	numMeshes = 0;
	numMaterials = 0;
}


// ---Copy Constructor---
GameObject::GameObject(GameObject* go, int size)
{
	uid = App->randomLCG->Int();

	go->pParent->AddChild(this);
	name = go->name;
	active = go->active;
	
	for (auto i = 0; i < size; i++)
	{
		GameObject* g = new GameObject(go->vChildren[i], go->vChildren.size());
		vChildren.push_back(g);
		g = nullptr;
	}

	// ---
	selected = false;
	hidden = false;

	AddComponent(C_TYPE::TRANSFORM);

	numMeshes = go->numMeshes;
	numMaterials = go->numMaterials;
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
}

u32 GameObject::GetUid()
{
	return uid;
}

update_status GameObject::Update(float dt)
{
	if (active)
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
					if (!vMaterials.empty() && vMaterials[i]->active)
					{
						vMeshes[i]->Draw(vMaterials[i]->checkered, vMaterials[i]->color);

						if (vMeshes[i]->showVertexNormals)
						{
							vMeshes[i]->DrawVertexNormals();
						}
						
						if (vMeshes[i]->showFacesNormals)
						{
							vMeshes[i]->DrawFaceNormals();
						}
					}
					else
					{
						// If it has no material, draw checkers
						vMeshes[i]->Draw(true);
					}
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

	temp = nullptr;
	return ret;
}

void GameObject::ReParent(GameObject* newParent)
{
	pParent->RemoveChild(this);

	pParent = newParent;
	pParent->vChildren.push_back(this);
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

void GameObject::AddChild(GameObject* go)
{
	go->pParent = this;
	vChildren.push_back(go);
}

void GameObject::DeleteChild(GameObject* go)
{
	go->~GameObject();
	RemoveChild(go);
}

void GameObject::RemoveChild(GameObject* go)
{
	vChildren.erase(std::find(vChildren.begin(), vChildren.end(), go));
	vChildren.shrink_to_fit();
}

GameObject* GameObject::FindChild(u32 idToFind)
{
	for (auto i = 0; i < vChildren.size(); i++)
	{
		if (!vChildren[i]->vChildren.empty())
		{
			vChildren[i]->FindChild(idToFind);
		}
		if (idToFind == vChildren[i]->GetUid())
		{
			return vChildren[i];
		}
	}
	return nullptr;
}
