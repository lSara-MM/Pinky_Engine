#pragma once
#include "C_Camera.h"

#include "GameObject.h"
#include "Component.h"
#include "External Libraries/ImGui/misc/cpp/imgui_stdlib.h"
//
#include "External Libraries/ImGui/imgui_custom.h"

C_Camera::C_Camera(GameObject* g, unsigned int i, bool start_enabled) : Component(C_TYPE::CAM, g, i, start_enabled, "Cam")
{
	//aspect ratio 16:9 
	width = 16;
	height = 9;
	aspect_ratio = width / height; 
	fov = 60; 

	//frustum settings
	frustum.type = PerspectiveFrustum;
	frustum.pos.Set(0, 0, 0);
	frustum.front= float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 1000.0f;
	frustum.verticalFov = fov * DEGTORAD;
	frustum.horizontalFov = Atan(aspect_ratio * Tan(frustum.verticalFov / 2)) * 2;
}

C_Camera::~C_Camera()
{
}

void C_Camera::ShowInInspector()
{
	// --- Set ImGui ids ---
	std::string checkbox = name.c_str();
	std::string header = name.c_str();

	checkbox.insert(checkbox.begin(), 2, '#');
	checkbox.append(std::to_string(GetID()));

	header.append("##");
	header.append(std::to_string(GetID()));

	// ---------------------------------------------

	ImGui::Checkbox(checkbox.c_str(), &active);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!active) { ImGui::BeginDisabled(); }

		// Edit frustum values ------------------
		if (ImGui::DragFloat("Near Plane", &frustum.nearPlaneDistance, 0.5f, 0.01f, frustum.farPlaneDistance));
		
		if (ImGui::DragFloat("Far Plane", &frustum.farPlaneDistance, 0.5f, frustum.nearPlaneDistance, 1000.0f));//TODO: see if max value makes sense

		if (ImGui::DragFloat("FOV", &fov, 0.5f, 1.0f, 179.0f))
		{
			SetFOV(fov);
		}

		if (!active) { ImGui::EndDisabled(); }
	}
}

void C_Camera::LookAt(const float3& position)
{
	float3 dir = position - frustum.pos;

	float3x3 matrix = float3x3::LookAt(frustum.front, dir.Normalized(), frustum.up, float3::unitY);

	frustum.front = matrix.MulDir(frustum.front).Normalized();
	frustum.up = matrix.MulDir(frustum.up).Normalized();
}

float* C_Camera::GetViewMatrix() const
{
	static float4x4 Matrix;
	Matrix = frustum.ViewMatrix();
	Matrix.Transpose();

	return (float*)Matrix.v;
}

float* C_Camera::GetProjectionMatrix() const
{
	static float4x4 Matrix;
	Matrix = frustum.ProjectionMatrix();
	Matrix.Transpose();

	return (float*)Matrix.v;
}

void C_Camera::DrawDebug()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(frustum.Edge(i).a.x, frustum.Edge(i).a.y, frustum.Edge(i).a.z);
		glVertex3f(frustum.Edge(i).b.x, frustum.Edge(i).b.y, frustum.Edge(i).b.z);
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

FrustumCulling C_Camera::ContainsAABox(const AABB& refBox) const
{
	float3 vCorner[8];
	int iTotalIn = 0;
	refBox.GetCornerPoints(vCorner); // get the corners of the box into the vCorner array

	// test all 8 corners against the 6 sides of the frustum.
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
	for (uint p = 0; p < 6; p++)
	{
		int iInCount = 8;
		int iPtIn = 1;

		for (uint i = 0; i < 8; i++)
		{
			// test this point against the planes
			if (frustum.GetPlane(p).IsOnPositiveSide(vCorner[i]))
			{
				// the point is behind the plane
				iPtIn = 0;
				iInCount--;
			}
		}

		// were all the points outside of plane p?
		if (iInCount == 0)
		{
			return FrustumCulling::CULLING_OUT;
		}

		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}

	// so if iTotalIn is 6, then all are inside the view
	if (iTotalIn == 6)
	{
		return FrustumCulling::CULLING_IN;
	}

	// we must be partly in then otherwise
	return FrustumCulling::CULLING_INTERSECT;
}

void C_Camera::SetAspectRatio(int width, int height)
{
	aspect_ratio = float(width)/float(height);
	frustum.horizontalFov = 2 * Atan(Tan(frustum.verticalFov / 2) * aspect_ratio);
}

void C_Camera::UpdateCameraFrustum()
{
	C_Transform* transformComponent = this->gameObject->transform;

	float4x4 transform = transformComponent->GetGlobalTransform();

	frustum.pos = transform.TranslatePart();
	frustum.front = transform.WorldZ().Normalized();
	frustum.up = frustum.front.Cross(-frustum.WorldRight()).Normalized();
}

void C_Camera::FrustumCulling()
{
	std::vector<GameObject*> objectsToCull;
	GetObjectsToCull(App->scene->rootNode, objectsToCull);
	
	for (auto i = 0; i < objectsToCull.size(); i++)
	{
		std::vector<C_Mesh*> vMeshes = objectsToCull[i]->GetComponentsMesh();

		for (auto i = 0; i < vMeshes.size(); i++)
		{
			if (ContainsAABox(vMeshes[i]->global_aabb) == FrustumCulling::CULLING_OUT)
			{
				vMeshes[i]->gameObject->isCulled = true;
			}
			else
			{
				vMeshes[i]->gameObject->isCulled = false;
			}
		}
	}

}

void C_Camera::GetObjectsToCull(GameObject* go, std::vector<GameObject*>& vgo)
{
	vgo.push_back(go);
	if (go->active)//TODO: needed?
	{
		if (!go->vChildren.empty())
		{
			for (auto i = 0; i < go->vChildren.size(); i++)
			{
				GameObject* parentgo = go->vChildren[i];
				GetObjectsToCull(parentgo, vgo);
			}
		}
	}
}

void C_Camera::SetFOV(float horizontalFOV)
{
	frustum.horizontalFov = horizontalFOV * DEGTORAD;
	frustum.verticalFov = 2 * (Atan(Tan(frustum.horizontalFov / 2) * (1 / aspect_ratio)));
}

