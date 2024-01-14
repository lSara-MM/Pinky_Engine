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
	C_Transform(C_TYPE type = C_TYPE::TRANSFORM);
	C_Transform(GameObject* g, float3 pos, Quat rot, float3 sc, C_TYPE type = C_TYPE::TRANSFORM, bool start_enabled = true);
	C_Transform(GameObject* g, C_Transform* toCopy, C_TYPE type = C_TYPE::TRANSFORM);
	~C_Transform();

	void ShowInInspector();

	void SetTransform(float3 pos = float3(0, 0, 0), float3 rot = float3(0, 0, 0), float3 scale = float3(0, 0, 0));
	void SetTransform(C_Transform& t);
	
	void SetPosition(float3 vec = float3(0, 0, 0));
	void SetRotation(float3 vec = float3(0, 0, 0));
	void SetScale(float3 vec = float3(0, 0, 0));

	void ReparentTransform(float4x4 matrix);
	
	float4x4 GetGlobalTransform() const;
	float4x4 GetLocalTransform() const;
	GLfloat* GetGLTransform() const;
	float3 GetGlobalPosition() const;
	Quat GetLocalRotation() const;

	void UpdateTransformsChilds();
	void UpdateGlobalMatrix();
	void UpdateLocalMatrix();
	//update global AABB and OBB when updating transform
	void UpdateBoundingBoxes();
	void UpdateTransformGuizmo(float4x4 matrix);

public:
	//local
	float3 position;
	Quat rotation;
	float3 eulerRot;
	float3 scale;
	float4x4 localMatrix;

	//global
	float4x4 globalMatrix;

	//Flag to update game object transform
	bool dirty_;
};