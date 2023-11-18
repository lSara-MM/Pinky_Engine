#pragma once
#include "C_Camera.h"

#include "GameObject.h"
#include "Component.h"
#include "External Libraries/ImGui/misc/cpp/imgui_stdlib.h"
//
#include "External Libraries/ImGui/imgui_custom.h"

C_Camera::C_Camera(GameObject* g, unsigned int i, bool start_enabled) : Component(C_TYPE::CAM, g, i, start_enabled, "Cam")
{

}

C_Camera::~C_Camera()
{
}

void C_Camera::ShowInInspector()
{

}

void C_Camera::LookAt(const float3& position)
{
	float3 dir = position - frustum.pos;

	float3x3 matrix = float3x3::LookAt(frustum.front, dir.Normalized(), frustum.up, float3::unitY);

	frustum.front = matrix.MulDir(frustum.front).Normalized();
	frustum.up = matrix.MulDir(frustum.up).Normalized();
}