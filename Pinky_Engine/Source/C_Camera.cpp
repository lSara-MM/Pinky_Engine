#pragma once
#include "C_Camera.h"

#include "GameObject.h"
#include "Component.h"
#include "External Libraries/ImGui/misc/cpp/imgui_stdlib.h"
//
#include "External Libraries/ImGui/imgui_custom.h"

C_Camera::C_Camera(GameObject* g, bool start_enabled) : Component(C_TYPE::CAM, g, start_enabled, "Camera")
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
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = fov * DEGTORAD;
	frustum.horizontalFov = Atan(aspect_ratio * Tan(frustum.verticalFov / 2)) * 2;

	//buffers //TODO: fer constructor bonic
	FBO = 0;
	RBO = 0;
	textureColourBuffer = 0;
	isMainCam = false;
	isCullEnabled = false;
}

C_Camera::~C_Camera()
{
	DeleteBuffers();
	if (isMainCam)
	{
		App->renderer3D->SetGameCamera(nullptr);
	}
	
}

void C_Camera::ShowInInspector()
{
	// --- Set ImGui ids ---
	std::string checkbox = name.c_str();
	std::string header = name.c_str();
	bool exists = true;

	checkbox.insert(checkbox.begin(), 2, '#');
	checkbox.append(std::to_string(GetUID()));

	header.append("##");
	header.append(std::to_string(GetUID()));

	// ---------------------------------------------

	ImGui::Checkbox(checkbox.c_str(), &isActive);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader(header.c_str(), &exists, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!isActive) { ImGui::BeginDisabled(); }

		//Buffers info
		ImGui::Text("Camera buffers:");
		ImGui::Text("FBO: %d", FBO);
		ImGui::Text("RBO: %d", RBO);
		ImGui::Text("Texture ID: %d", textureColourBuffer);
		ImGui::Separator();

		// Edit frustum values ------------------
		ImGui::DragFloat("Near Plane", &frustum.nearPlaneDistance, 0.5f, 0.01f, frustum.farPlaneDistance);
		
		ImGui::DragFloat("Far Plane", &frustum.farPlaneDistance, 0.5f, frustum.nearPlaneDistance, 1000.0f);//TODO: see if max value makes sense

		if (ImGui::DragFloat("FOV", &fov, 0.5f, 1.0f, 179.0f))
		{
			SetFOV(fov);
		}

		if (ImGui::Checkbox("Main Camera", &isMainCam))
		{	
			SetAsMain(isMainCam);
		}

		if (ImGui::Checkbox("Frustum Culling", &isCullEnabled))
		{
			RestartCulling();
		}

		if (!isActive) { ImGui::EndDisabled(); }
	}
	ImGui::SameLine();

	if (!exists) { gameObject->RemoveComponent(this); }
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

FrustumCulling C_Camera::ContainsAABox(const AABB& refBox) const//TODO: donde llamo esto
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

void C_Camera::SetAsMain(bool mainCam)
{
	if (mainCam)
	{
		if (App->renderer3D->gameCam != nullptr)
		{
			App->renderer3D->gameCam->isMainCam = false;
		}

		App->renderer3D->SetGameCamera(this);
	}
	else
	{
		App->renderer3D->gameCam = nullptr;
	}

	App->renderer3D->editorCam->OnResize(SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);//TODO: no entenc per qué fa falta
}

void C_Camera::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);
	CreateCamBuffers(width, height);
}

void C_Camera::UpdateCameraFrustum()
{
	C_Transform* transformComponent = gameObject->transform;//TODO: not good
	frustum.pos = transformComponent->GetGlobalPosition();
	frustum.front = transformComponent->GetLocalRotation().WorldZ();
	frustum.up = transformComponent->GetLocalRotation().WorldY();
}

void C_Camera::FrustumCulling()
{
	std::vector<GameObject*> objectsToCull;
	GetObjectsToCull(App->scene->rootNode, objectsToCull);

	for (auto i = 0; i < objectsToCull.size(); i++)
	{
		if (objectsToCull[i]->mesh != nullptr)
		{
			if (ContainsAABox(objectsToCull[i]->mesh->global_aabb) == FrustumCulling::CULLING_OUT)
			{
				objectsToCull[i]->isCulled = true;
			}
			else
			{
				objectsToCull[i]->isCulled = false;
			}
		}
	}
}

void C_Camera::GetObjectsToCull(GameObject* go, std::vector<GameObject*>& vgo)
{
	vgo.push_back(go);

	if (go->isActive)//TODO: needed?
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

void C_Camera::RestartCulling()
{
	std::vector<GameObject*> objectsToCull;
	GetObjectsToCull(App->scene->rootNode, objectsToCull);

	for (auto i = 0; i < objectsToCull.size(); i++)
	{
		objectsToCull[i]->isCulled = false;
	}
}

void C_Camera::Draw()
{
	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(GetProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(GetViewMatrix());

	BindFrameBuffer();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void C_Camera::CreateCamBuffers(int width, int height)
{
	SetAspectRatio(width, height);
	DeleteBuffers();

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &textureColourBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColourBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColourBuffer, 0);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
	{
		LOG("[ERROR] Framebuffer is not complete!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void C_Camera::BindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//glViewport(0, 0, App->editor->m_ViewportSize.x, App->editor->m_ViewportSize.y);
}

void C_Camera::UnbindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
}

void C_Camera::DeleteBuffers()
{
	if (FBO!=0)
	{
		glDeleteFramebuffers(1, &FBO);
	}

	if (textureColourBuffer != 0)
	{
		glDeleteTextures(1, &textureColourBuffer);
	}

	if (RBO != 0)
	{
		glDeleteRenderbuffers(1, &RBO);
	}
}
