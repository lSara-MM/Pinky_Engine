#pragma once

#include "C_Transform.h"

class GameObject;

class C_TransformUI : public C_Transform
{
public:
	C_TransformUI();
	C_TransformUI(GameObject* g, float3 pos, Quat rot, float3 sc, bool start_enabled = true);
	C_TransformUI(GameObject* g, C_Transform* toCopy);
	~C_TransformUI();

	void ShowInInspector();


public:
	//In unity, UI transforms use the z axis, even if it is not needed with an ortographic camera (up to user to use the component correctly
	float3 anchor1, anchor2, anchor3, anchor4, pivot;
	float2 bounds, size;
};