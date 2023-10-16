#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"

#include "../MathGeoLib/include/Math/float3x3.h"
#include "../MathGeoLib/include/Math/float4x4.h"
#include "../MathGeoLib/include/Math/Quat.h"
#include "Primitive.h"
#include<vector>

enum class C_TYPE
{
	TRANSFORM,
	MESH,
	MATERIAL
};

class GameObject;

class Component
{
public:
	Component();
	~Component();

	virtual void ShowInInspector();
	bool CleanUp();


public:
	bool active;
	std::string name;
};

class C_Transform : public Component
{
public:
	C_Transform();
	~C_Transform();

public:
	float3 position;
	Quat rotation;
	float3 scale;

	float4x4 globalMatrix;
	float4x4 localMatrix;
};

class C_Mesh : public Component
{
public:
	C_Mesh();
	~C_Mesh();

private:

};

class C_Material : public Component
{
public:
	C_Material();
	~C_Material();

private:

};