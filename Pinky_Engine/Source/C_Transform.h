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

	void SetTransform(float3 vec);
	void SetRotation(float3 vec);
	void SetScale(float3 vec);
	float4x4 GetGlobalTransform() const;
	float4x4 GetLocalTransform() const;
	GLfloat* GetGLTransform() const;
	float3 GetGlobalPosition() const;
	Quat GetLocalRotation() const;
	void UpdateTransformsChilds();
	void UpdateGlobalMatrix();
	//update global AABB and OBB when updating transform
	void UpdateBoundingBoxes();

public:
	//local
	float3 position;
	Quat rotation;
	float3 eulerRot;
	float3 scale;
	float4x4 localMatrix;

	//global
	float4x4 globalMatrix;

	bool updateMatrix;

};