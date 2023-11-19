#pragma once
#include "Globals.h"
#include "Light.h"


#include "Component.h"
#include "Color.h"

enum class FrustumCulling
{
	CULLING_IN,
	CULLING_OUT,
	CULLING_INTERSECT
};

class C_Camera : public Component
{
public:
	C_Camera(GameObject* g = nullptr,unsigned int i = 0, bool start_enabled = true);
	~C_Camera();

	void ShowInInspector();
	void LookAt(const float3& position);
	float* GetViewMatrix() const;
	float* GetProjectionMatrix() const;
	void DrawDebug();
	FrustumCulling ContainsAABox(const AABB& refBox) const;
	void SetAspectRatio(float ratio);

public:
	ai::texture* tex;
	bool checkered;
	Frustum frustum;

	Color color{};

private:

	//Editor settings
	bool isCullEnabled = false;
	//bool is_main = false;
	float width = 0.0f;
	float height = 0.0f;
	float aspect_ratio = 0.0f;
	float near_plane = 0.0f;
	float far_plane = 0.0f;
	float vertical_fov = 0.0f;
};