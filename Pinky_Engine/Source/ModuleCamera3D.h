#pragma once
#include "Module.h"
#include "Globals.h"
#include "External Libraries/MathGeoLib/include/Math/float3.h"
#include "External Libraries/MathGeoLib/include/Math/float4x4.h"
#include "C_Camera.h"


class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	//frustum camera functions
	void CameraMovement();
	void Orbit();
	void FPScamera();
	void Focus();
	void Zoom(float zoom, float scrollSpeed);
	void MousePick(float xNorm, float yNorm, float w, float h);
	//Handle all camera input, used in editor (isHovered)
	void CameraInput();

private:
	C_Camera* MainCamera;//TODO: eliminat = nullptr
	float mouseX, mouseY, speed, zoomPos;
	float3 Reference = { 0, 0, 0 };//change to focused object
	float3 centerReference = { 0, 0, 0 };
	LineSegment pickingRay;
};