#pragma once
#include "Globals.h"
#include "Light.h"


#include "Component.h"
#include "Color.h"

class C_Camera : public Component
{
public:
	C_Camera(GameObject* g = nullptr,unsigned int i = 0, bool start_enabled = true);
	~C_Camera();

	void ShowInInspector();
	void LookAt(const float3& position);

public:
	ai::texture* tex;
	bool checkered;
	Frustum frustum;

	Color color{};
};