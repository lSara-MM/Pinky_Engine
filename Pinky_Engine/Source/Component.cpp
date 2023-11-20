#include "Component.h"
#include "GameObject.h"

#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_Camera.h"

Component* Component::CopyComponent(GameObject* go)
{
	Component* temp;

	switch (type)
	{
	case C_TYPE::TRANSFORM:
		temp = new C_Transform(go, static_cast<C_Transform*>(this));
		go->transform = (C_Transform*)temp;
		break;
	case C_TYPE::MESH:
		temp = new C_Mesh(go, static_cast<C_Mesh*>(this), go->numMeshes);
		go->numMeshes++;
		break;
	case C_TYPE::MATERIAL:
		temp = new C_Material(go, static_cast<C_Material*>(this), go->numMaterials);
		go->numMaterials++;
		break;
	case C_TYPE::CAM:
		break;
	case C_TYPE::NONE:
		break;
	default:
		break;
	}

	return temp;
}

int Component::GetID()
{
	return id;
}
