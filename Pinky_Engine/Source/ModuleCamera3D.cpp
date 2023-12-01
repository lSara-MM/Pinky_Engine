#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "External Libraries/MathGeoLib/include/Math/Quat.h"
#include "External Libraries/MathGeoLib/include/Math/float3.h"
#include "External Libraries/MathGeoLib/include/Math/float4x4.h"
#include "C_Camera.h"
#include "ModuleScene.h"
#include "GameObject.h"


ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	MainCamera = new C_Camera();
	MainCamera->frustum.pos = float3(5.0f, 5.0f, 5.0f);
	MainCamera->LookAt(float3(0.0f, 0.0f, 0.0f));
	MainCamera->frustum.farPlaneDistance = 1000.0f;
	Reference = float3(0.0f, 0.0f, 0.0f);//change to selected game object
	centerReference = float3(0.0f, 0.0f, 0.0f);//TODO: delete when selected game object
	App->renderer3D->editorCam = MainCamera;
}

ModuleCamera3D::~ModuleCamera3D()
{
	RELEASE(MainCamera);
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	mouseX = 0.0f;
	mouseY = 0.0f;
	speed = 0;

	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{

	float3 newPos(0, 0, 0);
	speed = 10.0f * dt;

	//motion mouse		
	mouseX = (float)-App->input->GetMouseXMotion() * speed * dt;
	mouseY = (float)-App->input->GetMouseYMotion() * speed * dt;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
	{
		speed = 40.0f * dt;
	}

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::CameraMovement()
{
	float3 move(float3::zero);
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) move -= float3::unitY;
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) move += float3::unitY;

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) move -= MainCamera->frustum.front;
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) move += MainCamera->frustum.front;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) move -= MainCamera->frustum.WorldRight();
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		move += MainCamera->frustum.WorldRight();
	}

	if (!move.Equals(float3::zero))
	{
		MainCamera->frustum.Translate(move * speed);
		Reference += move * speed;
	}
}

void ModuleCamera3D::Orbit()
{
	//Todo delete center Reference
	Reference = centerReference;

	float3 dir = MainCamera->frustum.pos - Reference;
	Quat quaternionY(MainCamera->frustum.up, mouseX);
	Quat quaternionX(MainCamera->frustum.WorldRight(), mouseY);

	dir = quaternionX.Transform(dir);
	dir = quaternionY.Transform(dir);
	
	MainCamera->frustum.pos = dir + Reference;
	MainCamera->LookAt(Reference);
}

void ModuleCamera3D::FPScamera()
{
	if (mouseX != 0)
	{
		Quat rotY = Quat::RotateY(mouseX);
		MainCamera->frustum.front = rotY.Mul(MainCamera->frustum.front).Normalized();
		MainCamera->frustum.up = rotY.Mul(MainCamera->frustum.up).Normalized();
	}

	if (mouseY != 0)
	{
		Quat rotX = Quat::RotateAxisAngle(MainCamera->frustum.WorldRight(),mouseY);
		float3 newPosition = rotX.Mul(MainCamera->frustum.up).Normalized();

		if (newPosition.y > 0)
		{
			MainCamera->frustum.up = newPosition;
			MainCamera->frustum.front = rotX.Mul(MainCamera->frustum.front).Normalized();
		}

		float3 dist = Reference - MainCamera->frustum.pos;
		Reference = MainCamera->frustum.pos + MainCamera->frustum.front * dist.Length();//not sure needed
	}
}

void ModuleCamera3D::Zoom(float zoom, float scrollSpeed)
{
	MainCamera->frustum.pos += MainCamera->frustum.front * zoom * scrollSpeed;
}

void ModuleCamera3D::MousePick(LineSegment ray)
{
	intersects.clear();

	rayCam = ray;
	entryDist = 0.0f;
	exitDist = rayCam.Length();

	CheckGameObjectsIntersection(App->scene->rootNode);
	
	if (intersects.size() > 0)
	{
		CheckTriangleIntersection();
	}
}

void ModuleCamera3D::CheckGameObjectsIntersection(GameObject* go)
{
	CheckIntersection(go);

	if (!go->vChildren.empty())
	{
		for (auto i = 0; i < go->vChildren.size(); i++)
		{
			CheckGameObjectsIntersection(go->vChildren[i]);
		}
	}
}

void ModuleCamera3D::CheckIntersection(GameObject* go)
{
	if (go->isActive)
	{
		if (go->mesh != nullptr)
		{
			bool hit = rayCam.Intersects(go->mesh->global_aabb, entryDist, exitDist);

			if (hit)
			{
				intersects.insert(std::pair<float, GameObject*>(entryDist, go));
			}
		}
	}
}

void ModuleCamera3D::CheckTriangleIntersection()
{
	bool hit = false;
	entryDist = 0;
	float3 hit_point = float3::zero;
	float minDistance = INFINITY;
	GameObject* closest = nullptr;
	const C_Transform* trans = nullptr;
	const C_Mesh* mesh = nullptr;
	Triangle tri;
	std::map<float, GameObject*>::iterator iterator;

	for (iterator = intersects.begin(); iterator != intersects.end(); ++iterator)
	{
		if (iterator->second->transform != nullptr && iterator->second->mesh)
		{
			trans = iterator->second->transform;
			mesh = (C_Mesh*)iterator->second->mesh;

			float4x4 object_transform = iterator->second->transform->GetGlobalTransform();
			rayCam.Transform(object_transform.Inverted());

			for (uint i = 0; i < mesh->mesh->num_index; i += 3)
			{
				uint index1 = mesh->mesh->index[i] * 3;
				uint index2 = mesh->mesh->index[i + 1] * 3;
				uint index3 = mesh->mesh->index[i + 2] * 3;

				math::float3 vec1(mesh->mesh->vertex[index1], mesh->mesh->vertex[index1 + 1], mesh->mesh->vertex[index1 + 2]);
				math::float3 vec2(mesh->mesh->vertex[index2], mesh->mesh->vertex[index2 + 1], mesh->mesh->vertex[index2 + 2]);
				math::float3 vec3(mesh->mesh->vertex[index3], mesh->mesh->vertex[index3 + 1], mesh->mesh->vertex[index3 + 2]);

				tri.a = vec1;
				tri.b = vec2;
				tri.c = vec3;

				hit = rayCam.Intersects(tri, &entryDist, &hit_point);

				if (hit)
				{
					if (entryDist < minDistance)
					{
						minDistance = entryDist;
						closest = iterator->second;
					}
				}
			}
		}
	}

	if (closest != nullptr)
	{
		App->scene->h->SetSelected(closest);
		//App->camera->SetFocus(closest);
	}
}


void ModuleCamera3D::CameraInput()
{
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		CameraMovement();
		FPScamera();
	}

	else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		Orbit();
	}

	else if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN /*&& !ImGui::GetIO().WantTextInput*/)
	{
		Focus();
	}

	else if (/*!ImGui::GetIO().WantCaptureMouse && */App->input->GetMouseZ() != 0)
	{
		zoomPos = App->input->GetMouseZ();
		Zoom(zoomPos, speed);
	}
}

void ModuleCamera3D::Focus()
{
	MainCamera->frustum.pos = float3(5.0f, 5.0f, 5.0f);
	MainCamera->LookAt(float3(0.0f, 0.0f, 0.0f));
}