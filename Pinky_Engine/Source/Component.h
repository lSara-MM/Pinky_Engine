#pragma once
#include "Globals.h"
#include "Light.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"

#include "External Libraries/MathGeoLib/include/Math/float3x3.h"
#include "External Libraries/MathGeoLib/include/Math/float4x4.h"
#include "External Libraries/MathGeoLib/include/Math/Quat.h"

#include "External Libraries/Assimp/Assimp.h"

enum class C_TYPE
{
	TRANSFORM,
	MESH,
	MATERIAL,
	CAM,
	NONE
};

class GameObject;

class Component
{
public:
	Component(C_TYPE t = C_TYPE::NONE, GameObject* g = nullptr, unsigned int i = 0, bool start_enabled = true, std::string n = "Component")
		: type(t), gameObject(g), id(i), active(start_enabled), name(n) {};
	
	Component(GameObject* g, bool start_enabled = true, std::string n = "Component") : gameObject(g), active(start_enabled), name(n) {};
	virtual ~Component() { gameObject = nullptr; };

	virtual void ShowInInspector() {};
	Component* CopyComponent(GameObject* go);

	int GetID() { return id; }

public:
	bool active;
	std::string name;
	C_TYPE type;
	GameObject* gameObject;

private:
	unsigned int id;
};