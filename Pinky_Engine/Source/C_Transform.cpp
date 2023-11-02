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
	scale = sc;

	globalMatrix = math::float4x4::FromTRS(pos, rot, sc);
	localMatrix = math::float4x4::identity;
}

C_Transform::C_Transform(GameObject* g, C_Transform* toCopy) : Component(C_TYPE::TRANSFORM, g, g->GetUid(), toCopy->active, "Transform")
{
	position = toCopy->position;
	rotation = toCopy->rotation;
	scale = toCopy->scale;

	globalMatrix = math::float4x4::FromTRS(position, rotation, scale);
	localMatrix = math::float4x4::identity;
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

		float vec[3] = { position.x, position.y, position .z};
		ImGui::DragFloat3("Position", vec, 0.1f);
		SetTransform(vec);

		float vec1[3] = { rotation.x, rotation.y, rotation.z};
		ImGui::DragFloat3("Rotation", vec1, 0.1f);
		SetRotation(vec1);

		float vec2[3] = { scale.x, scale.y, scale.z};
		ImGui::DragFloat3("Scale", vec2, 0.1f);
		SetScale(vec2);
	}
}

void C_Transform::SetTransform(float vec[3])
{
	position = float3(vec);
}

void C_Transform::SetRotation(float vec[])
{
	rotation = Quat(vec);
}

void C_Transform::SetScale(float vec[3])
{
	scale = float3(vec);
}