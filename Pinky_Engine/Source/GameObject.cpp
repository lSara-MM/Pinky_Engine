#include "GameObject.h"
#include "ModuleScene.h"
#include "Component.h"

#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Camera.h"

// If parent == nullptr --> its the root node
GameObject::GameObject(std::string n, GameObject* parent, bool start_enabled)
{
	if (parent != nullptr)
	{
		parent->AddChild(this);
		uid = App->randomLCG->Int();
	}
	else
	{
		uid = -1;
	}

	name = n;
	isActive = start_enabled;
	isStatic = false;
	isCulled = false;

	// ---
	selected = false;
	hidden = false;

	AddComponent(C_TYPE::TRANSFORM);

	numMeshes = 0;
	numMaterials = 0;

	vChildren = {};
}


// ---Copy Constructor---
GameObject::GameObject(GameObject* go, int size, GameObject* parent)
{
	uid = App->randomLCG->Int();

	parent->AddChild(this);
	name = go->name;
	isActive = go->isActive;
	
	for (int i = 0; i < size; i++)
	{
		GameObject* g = new GameObject(go->vChildren[i], go->vChildren[i]->vChildren.size(), this);
		// Does't need pushback because it does it above (addchild())
		//vChildren.push_back(g);
		g = nullptr;
	}

	// ---
	selected = false;
	hidden = false;

	//AddComponent(C_TYPE::TRANSFORM);

	for (int i = 0; i < go->vComponents.size(); i++)
	{
		vComponents.push_back(go->vComponents[i]->CopyComponent(this));
	}

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

	pParent = nullptr;
}

u32 GameObject::GetUid()
{
	return uid;
}

update_status GameObject::Update(float dt)
{
	if (isActive)
	{
		if (!vChildren.empty())
		{
			for (auto i = 0; i < vChildren.size(); i++)
			{
				vChildren[i]->Update(dt);
			}
		}

		if (transform->dirty_)
		{
			transform->UpdateTransformsChilds();
		}

		if (vComponents.size() > 1)
		{
			std::vector<C_Mesh*> vMeshes = GetComponentsMesh();
			std::vector<C_Material*> vMaterials = GetComponentsMaterial();
			std::vector<C_Camera*> vCams = GetComponentsCamera();

			for (auto i = 0; i < vCams.size(); i++)
			{
				if (vCams[i]->isActive)
				{
					vCams[i]->DrawDebug();
					vCams[i]->UpdateCameraFrustum();
					vCams[i]->FrustumCulling();
				}
			}

			for (auto i = 0; i < vMeshes.size(); i++)
			{
				if (vMeshes[i]->isActive && !isCulled)
				{
					if (!vMaterials.empty() && vMaterials[i]->isActive)
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

						if (vMeshes[i]->showAABB)
						{
							vMeshes[i]->DrawAABB();
						}

						if (vMeshes[i]->showOBB)
						{
							vMeshes[i]->DrawOBB();
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

//---Components---
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
				ai::CreatePolyPrimitive(poly, this, true);
			}
		}
		else { ret = false; }
		break;
	case C_TYPE::MATERIAL:
		// A gameObject can't have more than one material
		// In unity there can be more than one if embeded (?) see snowman for reference 
		if (numMaterials == 0)	
		{
			if (m != nullptr)
			{
				temp = new C_Material(this, &m->tex, numMaterials);
				vComponents.push_back(temp);
			}
			else 
			{
				temp = new C_Material(this, nullptr, numMaterials);
				vComponents.push_back(temp);
			}
			numMaterials++;
		}
		else { ret = false; }
		break;
	case C_TYPE::CAM:
		temp = new C_Camera(this);
		vComponents.push_back(temp);
		break;
	default:
		break;
	}

	temp = nullptr;
	return ret;
}

void GameObject::RemoveComponent(Component* component)
{
	vComponents.erase(std::find(vComponents.begin(), vComponents.end(), component));
	component->~Component();

	switch (component->type)
	{
	case C_TYPE::MESH:
		numMeshes--;
		break;
	case C_TYPE::MATERIAL:
		numMaterials--;
		break;
	case C_TYPE::CAM:
		break;
	case C_TYPE::NONE:
		break;
	default:
		break;
	}
}

//---Parent/Child---
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

std::vector<C_Camera*> GameObject::GetComponentsCamera()
{
	std::vector<C_Camera*> vec = {};
	for (auto i = 0; i < vComponents.size(); i++)
	{
		if (vComponents[i]->type == C_TYPE::CAM)
		{
			vec.push_back((C_Camera*)vComponents[i]);
		}
	}
	return vec;
}

void GameObject::ReParent(GameObject* newParent)
{
	pParent->RemoveChild(this);

	pParent = newParent;
	pParent->vChildren.push_back(this);
	
	//TODO: clean up this mess
	//recalculate local values and global matrix when reparenting
	//this->transform->localMatrix = pParent->transform->globalMatrix.Transposed() * this->transform->globalMatrix;
	//transform->localMatrix = pParent->transform->globalMatrix.Inverted() * transform->globalMatrix;
	//transform->SetLocalValues(transform->localMatrix);
	transform->SetLocalValues(pParent->transform->globalMatrix.Inverted() * transform->globalMatrix);

	transform->dirty_ = true;
}

void GameObject::AddChild(GameObject* go)
{
	go->pParent = this;
	vChildren.push_back(go);
}

void GameObject::DeleteChild(GameObject* go)
{
	RemoveChild(go);
	go->~GameObject();
	go = nullptr;
}

void GameObject::RemoveChild(GameObject* go)
{
	vChildren.erase(std::find(vChildren.begin(), vChildren.end(), go));
	vChildren.shrink_to_fit();
}

GameObject* GameObject::FindChild(u32 idToFind, GameObject* go)
{
	for (auto i = 0; i < vChildren.size(); i++)
	{
		if (go != nullptr)
		{
			break;
		}
		if (!vChildren[i]->vChildren.empty())
		{
			go = vChildren[i]->FindChild(idToFind, go);
		}
		if (idToFind == vChildren[i]->GetUid())
		{
			go = vChildren[i];
			return go;
		}
	}

	return go;
}
