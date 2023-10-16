#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"

//GameObject::GameObject()
//{
//	// TODO: transform default es aixi? si es aixi, hi ha alguna forma rapida de comprobar
//	// si s'ha creat ja un transform o no o s'ha de fer el for si o si?
//	/*transform->position = { 0, 0, 0 };
//	transform->rotation = { 0, 0, 0, 0 };
//	transform->scale = { 1, 1, 1 };*/
//
//	transform = nullptr;
//}

GameObject::GameObject(/*GameObject* parent*/)
{
	//pParent = App->scene->rootNode;
	transform = nullptr;

	// TODO: transform default es aixi? si es aixi, hi ha alguna forma rapida de comprobar
//	// si s'ha creat ja un transform o no o s'ha de fer el for si o si?
//	/*transform->position = { 0, 0, 0 };
//	transform->rotation = { 0, 0, 0, 0 };
//	transform->scale = { 1, 1, 1 };*/
}

GameObject::~GameObject()
{
	RELEASE(transform);

	for (int i = 0; i < vComponents.size(); i++)
	{
		delete vComponents[i];
		vComponents[i] = nullptr;
	}

	for (int i = 0; i < vChildren.size(); i++)
	{
		delete vChildren[i];
		vChildren[i] = nullptr;
	}
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
