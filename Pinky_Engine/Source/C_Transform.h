#pragma once
#include "Globals.h"
#include "Light.h"

#include "External Libraries/MathGeoLib/include/Math/float3x3.h"
#include "External Libraries/MathGeoLib/include/Math/float4x4.h"
#include "External Libraries/MathGeoLib/include/Math/Quat.h"

#include "Component.h"

class GameObject;

class C_Transform : public Component
{
public:
	C_Transform(GameObject* g, float3 pos, Quat rot, float3 sc, bool start_enabled = true);
	C_Transform(GameObject* g, C_Transform* toCopy);
	~C_Transform();

	void ShowInInspector();

	void SetTransform(float vec[3]);
	void SetRotation(float vec[]);
	void SetScale(float vec[3]);
	float4x4 GetGlobalTransform() const;
	float4x4 GetLocalTransform() const;

public:
	float3 position;
	Quat rotation;
	float3 scale;

	float4x4 globalMatrix;
	float4x4 localMatrix;
};