#pragma once
#include "C_Transform.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "GameObject.h"
#include "Component.h"

C_Transform::C_Transform(GameObject* g, bool start_enabled) : Component(C_TYPE::TRANSFORM, g, g->GetUid(), start_enabled, "Transform")
{
}

C_Transform::C_Transform(GameObject* g, float3 pos, Quat rot, float3 sc, bool start_enabled) : Component(C_TYPE::TRANSFORM, g, g->GetUid(), start_enabled, "Transform")
{
	position = pos;
	rotation = rot;
	scale = sc;
}

C_Transform::~C_Transform()
{
}

void C_Transform::ShowInInspector()
{
	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		//TODO: buscar forma de hacerlo dependiendo del tamaño de la window
		/*ImGui::Dummy(ImVec2(150, 0));	ImGui::SameLine(150);
		ImGui::Text("X");				ImGui::SameLine(250);
		ImGui::Text("Y");				ImGui::SameLine(350);
		ImGui::Text("Z");*/

		float vec[3] = { position.x, position.y, position .z};
		ImGui::DragFloat3("Position", vec);
		SetTransform(vec);

		float vec1[3] = { rotation.x, rotation.y, rotation.z};
		ImGui::DragFloat3("Rotation", vec1);
		SetRotation(vec1);

		float vec2[3] = { scale.x, scale.y, scale.z};
		ImGui::DragFloat3("Scale", vec2);
		SetScale(vec2);
	}
}

void C_Transform::SetTransform(float vec[3])
{
	for (int i = 0; i < 3; i++)
	{
		position[i] = vec[i];
	}
}

void C_Transform::SetRotation(float vec[])
{
	rotation.x = vec[0];
	rotation.y = vec[1];
	rotation.z = vec[2];
}

void C_Transform::SetScale(float vec[3])
{
	for (int i = 0; i < 3; i++)
	{
		scale[i] = vec[i];
	}
}