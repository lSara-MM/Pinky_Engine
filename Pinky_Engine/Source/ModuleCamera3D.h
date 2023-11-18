#pragma once
#include "Module.h"
#include "Globals.h"
#include "External Libraries/MathGeoLib/include/Math/float3.h"
#include "External Libraries/MathGeoLib/include/Math/float4x4.h"
#include "C_Camera.h"

//todo: REMOVE this before 2nd delivery!!
#include "glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const float3 &Position, const float3&Reference, bool RotateAroundReference = false);
	void LookAt(const float3&Spot);
	void Move(const float3&Movement);
	void Zoom(math::float3 nPos, float s);
	float* GetViewMatrix();

	//frustrum camera functions
	void CameraMovement();
	void Orbit();
	void FPScamera();
	void Focus();

private:

	void CalculateViewMatrix();

public:
	
	//You won't need this after using Frustum
	float3 X, Y, Z, Position, Reference;

	C_Camera* MainCamera = nullptr;
	int mouseX, mouseY;
	float speed;

private:

	mat4x4 ViewMatrix;
	//Frustum mMainCamera; Some help here :)
};