#pragma once
#include "C_Camera.h"

#include "GameObject.h"
#include "Component.h"
#include "External Libraries/ImGui/misc/cpp/imgui_stdlib.h"
//
#include "External Libraries/ImGui/imgui_custom.h"

C_Camera::C_Camera(GameObject* g, unsigned int i, bool start_enabled) : Component(C_TYPE::CAM, g, i, start_enabled, "Cam")
{
	//aspect ratio 16:9 by now
	width = 16;
	height = 9;
	aspect_ratio = width / height; 

	near_plane = 0.2;
	far_plane = 1000;
	vertical_fov = 60; 

	//frustum settings
	frustum.type = PerspectiveFrustum;
	frustum.pos.Set(0, 0, 0);
	frustum.front.Set(0, 0, 1);
	frustum.up.Set(0, 1, 0);
	frustum.nearPlaneDistance = near_plane;
	frustum.farPlaneDistance = far_plane;
	frustum.verticalFov = vertical_fov * DEGTORAD;
	frustum.horizontalFov = Atan(aspect_ratio * Tan(frustum.verticalFov / 2)) * 2;
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
	glColor4f(0.25f, 1.0f, 0.0f, 1.0f);

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

void C_Camera::SetAspectRatio(float ratio)
{
	aspect_ratio = ratio;
	frustum.horizontalFov = 2 * Atan(Tan(frustum.verticalFov / 2) * aspect_ratio);
}
