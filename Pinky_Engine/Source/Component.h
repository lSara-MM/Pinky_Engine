#pragma 
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

class Component
{
public:
	Component(C_TYPE t = C_TYPE::NONE, bool start_enabled = true, std::string n = "Component")
		: type(t), active(start_enabled), name(n) {};
	~Component();

	virtual void ShowInInspector();

public:
	bool active;
	std::string name;
	C_TYPE type;
};

class C_Transform : public Component
{
public:
	C_Transform(bool start_enabled = true);
	C_Transform(float3 pos, Quat rot, float3 sc, bool start_enabled = true);
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
	C_Mesh(bool start_enabled = true);
	~C_Mesh();

public:
	ai::mesh* m;
private:

};

class C_Material : public Component
{
public:
	C_Material(bool start_enabled = true);
	~C_Material();

private:

};