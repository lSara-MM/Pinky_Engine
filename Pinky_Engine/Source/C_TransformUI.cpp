#pragma once
#include "C_TransformUI.h"

C_TransformUI::C_TransformUI() : C_Transform(C_TYPE::TRANSFORM_UI)
{
	anchor1 = float3(1.0f, 1.0f, 1.0f);
	anchor2 = float3(1.0f, 1.0f, 1.0f);
	anchor3 = float3(1.0f, 1.0f, 1.0f);
	anchor4 = float3(1.0f, 1.0f, 1.0f);
	pivot = float3(1.0f, 1.0f, 1.0f);
	bounds = float2(1.0f, 1.0f);
	size = float2(1.0f, 1.0f);
}

C_TransformUI::C_TransformUI(GameObject* g, float3 pos, Quat rot, float3 sc, bool start_enabled) : C_Transform(g, pos, rot, sc, C_TYPE::TRANSFORM_UI, start_enabled)
{
	anchor1 = float3(1.0f, 1.0f, 1.0f);
	anchor2 = float3(1.0f, 1.0f, 1.0f);
	anchor3 = float3(1.0f, 1.0f, 1.0f);
	anchor4 = float3(1.0f, 1.0f, 1.0f);
	pivot = float3(1.0f, 1.0f, 1.0f);
	bounds = float2(1.0f, 1.0f);
	size = float2(1.0f, 1.0f);
}

C_TransformUI::C_TransformUI(GameObject* g, C_Transform* toCopy) : C_Transform(g, toCopy, C_TYPE::TRANSFORM_UI)
{
	anchor1 = float3(1.0f, 1.0f, 1.0f);
	anchor2 = float3(1.0f, 1.0f, 1.0f);
	anchor3 = float3(1.0f, 1.0f, 1.0f);
	anchor4 = float3(1.0f, 1.0f, 1.0f);
	pivot = float3(1.0f, 1.0f, 1.0f);
	bounds = float2(1.0f, 1.0f);
	size = float2(1.0f, 1.0f);
}

C_TransformUI::~C_TransformUI()
{
}

void C_TransformUI::ShowInInspector()
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

