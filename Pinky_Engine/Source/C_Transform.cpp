#pragma once
#include "C_Transform.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "GameObject.h"
#include "Component.h"


C_Transform::C_Transform(GameObject* g, float3 pos, Quat rot, float3 sc, bool start_enabled) : Component(C_TYPE::TRANSFORM, g, g->GetUid(), start_enabled, "Transform")
{
	position = pos;
	rotation = rot;
	eulerRot = rotation.ToEulerXYZ();
	scale = sc;

	globalMatrix = math::float4x4::FromTRS(pos, rot, sc);
	localMatrix = math::float4x4::identity;
	dirty_ = true;
}

C_Transform::C_Transform(GameObject* g, C_Transform* toCopy) : Component(C_TYPE::TRANSFORM, g, g->GetUid(), toCopy->isActive, "Transform")
{
	position = toCopy->position;
	rotation = toCopy->rotation;
	eulerRot = rotation.ToEulerXYZ();
	scale = toCopy->scale;

	globalMatrix = math::float4x4::FromTRS(position, rotation, scale);
	localMatrix = math::float4x4::FromTRS(position, rotation, scale);
	dirty_ = false;
}

C_Transform::~C_Transform()
{
}

void C_Transform::ShowInInspector()
{
	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Position", &position[0], 0.1f))
		{
			SetTransform(position);
		}

		if (ImGui::DragFloat3("Rotation", &eulerRot[0], 0.1f))
		{
			SetRotation(eulerRot);
		}

		if (ImGui::DragFloat3("Scale", &scale[0], 0.1f))
		{
			SetScale(scale);
		}
	}
}

void C_Transform::SetTransform(float3 vec)
{
	position = float3(vec);
	dirty_ = true;
}

void C_Transform::SetRotation(float3 vec)
{
	rotation = Quat::FromEulerXYZ(vec[0] * DEGTORAD, vec[1] * DEGTORAD, vec[2] * DEGTORAD);
	dirty_ = true;
}

void C_Transform::SetScale(float3 vec)
{
	scale = float3(vec);
	dirty_ = true;
}

void C_Transform::SetLocalValues(float4x4 matrix)
{
	float3 pos, sc;
	Quat rot;
	matrix.Decompose(pos, rot, sc);
	position = pos;
	rotation = rot.Normalized();
	eulerRot = rotation.ToEulerXYZ();
	eulerRot.x = RadToDeg(eulerRot.x);
	eulerRot.y = RadToDeg(eulerRot.y);
	eulerRot.z = RadToDeg(eulerRot.z);
	scale = sc;
}

float4x4 C_Transform::GetGlobalTransform() const
{
	return globalMatrix;
}

float4x4 C_Transform::GetLocalTransform() const
{
	return localMatrix;
}

GLfloat* C_Transform::GetGLTransform() const
{
	return globalMatrix.Transposed().ptr();
}

float3 C_Transform::GetGlobalPosition() const
{
	float3 pos, sc;
	Quat rot;
	globalMatrix.Decompose(pos, rot, sc);

	return pos;
}

Quat C_Transform::GetLocalRotation() const
{
	return rotation;
}

void C_Transform::UpdateTransformsChilds()
{
	gameObject->transform->UpdateGlobalMatrix();

	if (!gameObject->vChildren.empty())
	{
		for (auto i = 0; i < gameObject->vChildren.size(); i++)
		{
			gameObject->vChildren[i]->transform->UpdateTransformsChilds();
		}
	}
	dirty_ = false;
}

void C_Transform::UpdateGlobalMatrix()
{
	rotation = Quat::FromEulerXYZ(eulerRot.x * DEGTORAD, eulerRot.y * DEGTORAD, eulerRot.z * DEGTORAD);
	rotation.Normalize();

	localMatrix = float4x4::FromTRS(position, rotation, scale);

	if (gameObject->pParent != nullptr)
	{
		float4x4 Global_parent = gameObject->pParent->transform->GetGlobalTransform();
		globalMatrix = Global_parent * localMatrix;//Your global matrix = your parent’s global matrix * your local Matrix
		UpdateBoundingBoxes();
	}
}

void C_Transform::UpdateBoundingBoxes()
{
	if (gameObject->mesh != nullptr)
	{
		gameObject->mesh->obb = gameObject->mesh->mesh->local_aabb;
		gameObject->mesh->obb.Transform(gameObject->transform->GetGlobalTransform());
		gameObject->mesh->global_aabb.SetNegativeInfinity();
		gameObject->mesh->global_aabb.Enclose(gameObject->mesh->obb);
	}
}

void C_Transform::UpdateGlobalTransform(float4x4 matrix)
{
	float3 pos, sc;
	Quat rot;
	matrix.Decompose(pos, rot, sc);

	scale = sc;
	position = pos;
	rotation = rot;
	eulerRot = rotation.ToEulerXYZ();
	eulerRot.x = RadToDeg(eulerRot.x);
	eulerRot.y = RadToDeg(eulerRot.y);
	eulerRot.z = RadToDeg(eulerRot.z);
	
	dirty_ = true;
}
