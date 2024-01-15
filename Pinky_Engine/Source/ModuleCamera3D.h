#pragma once
#include "Module.h"
#include "Globals.h"
#include "External Libraries/MathGeoLib/include/Math/float3.h"
#include "External Libraries/MathGeoLib/include/Math/float4x4.h"
#include "C_Camera.h"
#include "C_UI.h"

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
	//mouse picking
	void MousePick(LineSegment ray);
	void CheckIntersection(GameObject* go);
	void CheckGameObjectsIntersection(GameObject* go);
	bool CheckTriangleIntersection();
	void DebugRaycast();
	//UI mouse picking
	void MousePickUI(LineSegment ray);
	void CheckUICompIntersection(GameObject* go);
	void CheckIntersectionUI(GameObject* go);
	bool CheckTriangleIntersectionUI();

	//Handle all camera input, used in editor (isHovered)
	void CameraInput();
	LineSegment localrayCam;
	C_Camera* MainCamera;
	float3 objectReference;

private:
	float mouseX, mouseY, speed, zoomPos;
	float3 Reference;

	//Mouse Picking
	float entryDist;
	float exitDist;
	LineSegment rayCam;
	std::map<float, GameObject*> intersects;
	std::map<float, C_UI*> intersectsUI;

};