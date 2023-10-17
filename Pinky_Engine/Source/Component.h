#pragma 
#include "Globals.h"
#include "Light.h"

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"

#include "../MathGeoLib/include/Math/float3x3.h"
#include "../MathGeoLib/include/Math/float4x4.h"
#include "../MathGeoLib/include/Math/Quat.h"

#include <vector>

enum class C_TYPE
{
	TRANSFORM,
	MESH,
	MATERIAL
};

class Component
{
public:
	Component(bool start_enabled = true);
	~Component();

	virtual void ShowInInspector();

public:
	bool active;
	std::string name;
};

class C_Transform : public Component
{
public:
	C_Transform(bool start_enabled = true);
	C_Transform(float3 pos, Quat rot, float3 sc, bool start_enabled = true) :
		position(pos), rotation(rot), scale(sc) {};
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

private:

};

class C_Material : public Component
{
public:
	C_Material(bool start_enabled = true);
	~C_Material();

private:

};