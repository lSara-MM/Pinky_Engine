#include "GameObject.h"
#include "ModuleScene.h"
#include "Component.h"

#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Camera.h"
#include "External Libraries/ImGuizmo/ImGuizmo.h"

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
		// If root node, id = 0
		uid = 0;
	}

	name = n;
	isActive = start_enabled;
	isStatic = false;
	isCulled = false;

	// ---
	selected = false;
	hidden = false;

	AddComponent(C_TYPE::TRANSFORM);

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

u32 GameObject::GetUID() { return uid; }

void GameObject::SetUID(u32 id) { uid = id; }

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
			std::vector<C_Material*> vMaterials = GetComponentsMaterial();
			std::vector<C_Camera*> vCams = GetComponentsCamera();

			for (auto i = 0; i < vCams.size(); i++)
			{
				if (vCams[i]->isActive)
				{
					vCams[i]->DrawDebug();
					vCams[i]->UpdateCameraFrustum();
					if (vCams[i]->isCullEnabled)
					{
						vCams[i]->FrustumCulling();
					}
				}
			}

			if (mesh != nullptr)
			{
				if (mesh->isActive && !isCulled)
				{
					if (!vMaterials.empty() && vMaterials[0]->isActive)//TODO: mirar materials bé
					{
						mesh->Draw(vMaterials[0]->checkered, vMaterials[0]->color);

						if (mesh->showVertexNormals)
						{
							mesh->DrawVertexNormals();
						}

						if (mesh->showFacesNormals)
						{
							mesh->DrawFaceNormals();
						}

						if (mesh->showAABB)
						{
							mesh->DrawAABB();
						}

						if (mesh->showOBB)
						{
							mesh->DrawOBB();
						}
					}
					else
					{
						// If it has no material, draw checkers
						mesh->Draw(true);
					}
				}
			}
		}
	}

	return UPDATE_CONTINUE;
}

//---Components---
bool GameObject::AddComponent(C_TYPE type, void* var, ai::POLY_PRIMITIVE_TYPE poly)
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
		if (mesh == nullptr)
		{
			R_Mesh* m = static_cast<R_Mesh*>(var);
			if (m != nullptr)
			{
				temp = new C_Mesh(this, m);
				mesh = (C_Mesh*)temp;
				vComponents.push_back(temp);
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
			R_Texture* t = static_cast<R_Texture*>(var);
			if (t != nullptr)
			{
				temp = new C_Material(this, t);
				vComponents.push_back(temp);
			}
			else
			{
				temp = new C_Material(this, nullptr);
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

bool GameObject::AddComponent(Component* component)
{
	component->gameObject = this;
	switch (component->type)
	{
	case C_TYPE::TRANSFORM:
		RemoveComponent(transform);
		transform = (C_Transform*)component;
		vComponents.push_back(transform);
		break;
	case C_TYPE::MESH:
		mesh = (C_Mesh*)component;
		vComponents.push_back(mesh);
		break;
	case C_TYPE::MATERIAL:
		vComponents.push_back(component);
		numMaterials++;
		break;
	case C_TYPE::CAM:
		vComponents.push_back(component);
		break;
	default:
		break;
	}

	return true;
}

void GameObject::RemoveComponent(Component* component)
{
	vComponents.erase(std::find(vComponents.begin(), vComponents.end(), component));
	component->~Component();

	switch (component->type)
	{
	case C_TYPE::TRANSFORM:
		transform = nullptr;
		break;
	case C_TYPE::MESH:
		mesh = nullptr;
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

C_Mesh* GameObject::GetComponentMesh()
{
	for (auto i = 0; i < vComponents.size(); i++)
	{
		if (vComponents[i]->type == C_TYPE::MESH)
		{
			mesh = (C_Mesh*)vComponents[i];
		}
	}
	return mesh;
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

Component* GameObject::GetComponentByType(C_TYPE type)
{
	for (int i = 0; i < vComponents.size(); i++)
	{
		if (vComponents[i]->type == type)
		{
			return vComponents[i];
		}
	}

	return nullptr;
}

//---Parents/Children---
void GameObject::ReParent(GameObject* newParent)
{
	pParent->RemoveChild(this);

	pParent = newParent;
	pParent->vChildren.push_back(this);

	//Update transform values
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
		if (idToFind == vChildren[i]->GetUID())
		{
			go = vChildren[i];
			return go;
		}
	}

	return go;
}
