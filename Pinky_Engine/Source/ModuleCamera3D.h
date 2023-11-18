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

	//frustrum camera functions
	void CameraMovement();
	void Orbit();
	void FPScamera();
	void Focus();
	void Zoom(float zoom, float scrollSpeed);

private:
	C_Camera* MainCamera = nullptr;
	int mouseX, mouseY;
	float speed, zoomPos;
	float3 Reference = { 0, 0, 0 };//change to focused object
};