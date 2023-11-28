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
	updateMatrix = false;
}

C_Transform::C_Transform(GameObject* g, C_Transform* toCopy) : Component(C_TYPE::TRANSFORM, g, g->GetUid(), toCopy->isActive, "Transform")
{
	position = toCopy->position;
	rotation = toCopy->rotation;
	eulerRot = rotation.ToEulerXYZ();
	scale = toCopy->scale;

	globalMatrix = math::float4x4::FromTRS(position, rotation, scale);
	localMatrix = math::float4x4::FromTRS(position, rotation, scale);
	updateMatrix = false;
}

C_Transform::~C_Transform()
{
}

void C_Transform::ShowInInspector()
{
	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		//TODO: Change with window size
		/*ImGui::Dummy(ImVec2(150, 0));	ImGui::SameLine(150);
		ImGui::Text("X");				ImGui::SameLine(250);
		ImGui::Text("Y");				ImGui::SameLine(350);
		ImGui::Text("Z");*/

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
	updateMatrix = true;
}

void C_Transform::SetRotation(float3 vec)
{
	rotation = Quat::FromEulerXYZ(vec[0] * DEGTORAD, vec[1] * DEGTORAD, vec[2] * DEGTORAD);
	updateMatrix = true;
}

void C_Transform::SetScale(float3 vec)
{
	scale = float3(vec);
	updateMatrix = true;
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

void C_Transform::UpdateTransformsChilds()
{
	if (!gameObject->vChildren.empty())
	{
		for (auto i = 0; i < gameObject->vChildren.size(); i++)
		{
			this->gameObject->vChildren[i]->transform->UpdateGlobalMatrix();
		}
	}
	updateMatrix = false;
}

void C_Transform::UpdateGlobalMatrix()
{
	localMatrix = float4x4::FromTRS(position, rotation, scale);

	if (this->gameObject->pParent != nullptr)
	{
		float4x4 Global_parent = this->gameObject->pParent->transform->GetGlobalTransform();
		globalMatrix = Global_parent * localMatrix;//Your global matrix = your parent’s global matrix * your local Matrix
	}
}
