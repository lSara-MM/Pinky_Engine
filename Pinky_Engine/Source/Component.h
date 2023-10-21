#pragma once
#include "Globals.h"
#include "Light.h"

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"

#include "../MathGeoLib/include/Math/float3x3.h"
#include "../MathGeoLib/include/Math/float4x4.h"
#include "../MathGeoLib/include/Math/Quat.h"

#include "../Assimp/Assimp.h"
#include <vector>

enum class C_TYPE
{
	TRANSFORM,
	MESH,
	MATERIAL,
	NONE
};

class GameObject;

class Component
{
public:
	Component(C_TYPE t = C_TYPE::NONE, GameObject* g = nullptr, unsigned int i = 0, bool start_enabled = true, std::string n = "Component")
		: type(t), gameObject(g), id(i), active(start_enabled), name(n) {};
	virtual ~Component() {};

	virtual void ShowInInspector() {};
	int GetID();

public:
	bool active;
	std::string name;
	C_TYPE type;
	GameObject* gameObject;

private:
	unsigned int id;
};

//class C_Transform : public Component
//{
//public:
//	C_Transform(GameObject* g = nullptr, bool start_enabled = true);
//	C_Transform(GameObject* g, float3 pos, Quat rot, float3 sc, bool start_enabled = true);
//	~C_Transform();
//
//	void ShowInInspector();
//
//private:
//	void SetTransform(float vec[3]);
//	void SetRotation(float vec[]);
//	void SetScale(float vec[3]);
//public:
//	float3 position;
//	Quat rotation;
//	float3 scale;
//
//	float4x4 globalMatrix;
//	float4x4 localMatrix;
//};
//
//class C_Mesh : public Component
//{
//public:
//	C_Mesh(GameObject* g = nullptr, ai::mesh* m = nullptr, unsigned int i = 0, bool start_enabled = true);
//	~C_Mesh();
//
//	void ShowInInspector();
//public:
//	ai::mesh* mesh = nullptr;
//};
//
//class C_Material : public Component
//{
//public:
//	C_Material(GameObject* g = nullptr, unsigned int i = 0, bool start_enabled = true);
//	~C_Material();
//
//	void ShowInInspector();
//
//private:
//};