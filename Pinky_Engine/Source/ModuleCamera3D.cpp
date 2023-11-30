#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "External Libraries/MathGeoLib/include/Math/Quat.h"
#include "External Libraries/MathGeoLib/include/Math/float3.h"
#include "External Libraries/MathGeoLib/include/Math/float4x4.h"
#include "C_Camera.h"


ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	MainCamera = new C_Camera();
	MainCamera->frustum.pos = float3(5.0f, 5.0f, 5.0f);
	MainCamera->LookAt(float3(0.0f, 0.0f, 0.0f));
	MainCamera->frustum.farPlaneDistance = 1000.0f;
	Reference = float3(0.0f, 0.0f, 0.0f);//change to selected game object
	centerReference = float3(0.0f, 0.0f, 0.0f);//TODO: delete when selected game object
	App->renderer3D->SetCamActive(MainCamera);
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

	//TODO: treure, es fa en window scene editor
	//CameraInput();

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

void ModuleCamera3D::MousePick(float xNorm, float yNorm, float w, float h)
{
	pickingRay = MainCamera->frustum.UnProjectLineSegment(xNorm, yNorm);
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

	else if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && !ImGui::GetIO().WantTextInput)
	{
		Focus();
	}

	else if (!ImGui::GetIO().WantCaptureMouse && App->input->GetMouseZ() != 0)
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