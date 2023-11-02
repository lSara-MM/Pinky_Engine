#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "../MathGeoLib/include/Math/Quat.h"
#include "../MathGeoLib/include/Math/float3.h"
#include "../MathGeoLib/include/Math/float4x4.h"
#include "C_Camera.h"


ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	Position = float3(5.0f, 5.0f, 5.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);
	ViewMatrix = IdentityMatrix;

	LookAt(Reference);

	MainCamera = new C_Camera();
	MainCamera->frustum.pos = float3(0.0f, 10.0f, 5.0f);
	MainCamera->LookAt(float3(0.0f, 0.0f, 0.0f));

	CalculateViewMatrix();
}

ModuleCamera3D::~ModuleCamera3D()
{
	RELEASE(MainCamera);
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	mouseX = 0;
	mouseY = 0;
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
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	float3 newPos(0, 0, 0);
	float speed = 10.0f * dt;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 20.0f * dt;

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y -= speed;
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

		Position += newPos;

		//motion mouse
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.35f * dt;

		Position -= Reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			float3 rotationAxis(0.0f, 1.0f, 0.0f);
			Quat rotationQuat = Quat::RotateAxisAngle(rotationAxis, DeltaX);

			X = rotationQuat * X;
			Y = rotationQuat * Y;
			Z = rotationQuat * Z;
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Quat rotationQuat = Quat::RotateAxisAngle(X, DeltaY);

			Y = rotationQuat * Y;
			Z = rotationQuat * Z;

			if (Y.y < 0.0f)
			{
				Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = Z.Cross(X);
			}
		}
	}

	else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		//motion mouse
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.35f * dt;

		Position -= Reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			float3 rotationAxis(0.0f, 1.0f, 0.0f);
			Quat rotationQuat = Quat::RotateAxisAngle(rotationAxis, DeltaX);

			X = rotationQuat * X;
			Y = rotationQuat * Y;
			Z = rotationQuat * Z;
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Quat rotationQuat = Quat::RotateAxisAngle(X, DeltaY);

			Y = rotationQuat * Y;
			Z = rotationQuat * Z;

			if (Y.y < 0.0f)
			{
				Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = Z.Cross(X);
			}
		}
		math::float3 tempPosition = Reference;

		Position = tempPosition + Z * Position.Length();

		LookAt(Reference);
	}

	else if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && !ImGui::GetIO().WantTextInput)
	{
		Focus();
	}

	else if(!ImGui::GetIO().WantCaptureMouse)
	{
		Zoom(newPos, speed);
		Position += newPos;
		Reference += newPos;
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3&Position, const float3&Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = (Position - Reference).Normalized();
	X = (float3(0.0f, 1.0f, 0.0f).Cross(Z)).Normalized();
	Y = Z.Cross(X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.005f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const float3&Spot)
{
	Reference = Spot;

	Z = (Position - Reference).Normalized();
	X = (float3(0.0f, 1.0f, 0.0f).Cross(Z)).Normalized();
	Y = Z.Cross(X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3&Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

void ModuleCamera3D::Zoom(math::float3 newPosition, float vel)
{
	if (App->input->GetMouseZ() > 0)
	{
		newPosition -= Z * vel * 10;
	}
	else if (App->input->GetMouseZ() < 0)
	{
		newPosition += Z * vel * 10;
	}

	Position += newPosition;
}

void ModuleCamera3D::Focus()
{
	Position = float3(5.0f, 5.0f, 5.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);
	LookAt(Reference);
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return ViewMatrix.M;
}

void ModuleCamera3D::CameraMovement()
{
	float3 move(float3::zero);
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) move -= float3::unitY;
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) move += float3::unitY;

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) move -= MainCamera->frustum.front;
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) move += MainCamera->frustum.front;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) move -= MainCamera->frustum.WorldRight();
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) move += MainCamera->frustum.WorldRight();

	if (!move.Equals(float3::zero))
	{
		MainCamera->frustum.Translate(move * speed);
		Reference += move * speed;
	}
}

void ModuleCamera3D::Orbit()
{
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
		Reference = MainCamera->frustum.pos + MainCamera->frustum.front * dist.Length();
	}
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	//todo: USE MATHGEOLIB here BEFORE 2nd delivery! (TIP: Use MathGeoLib/Geometry/Frustum.h, view and projection matrices are managed internally.)
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -(X.Dot(Position)), -(Y.Dot(Position)), -(Z.Dot(Position)), 1.0f);
}