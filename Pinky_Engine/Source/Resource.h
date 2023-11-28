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

enum class R_TYPE
{
	MESH,
	TEXTURE,
	SCENE,

	NONE
};

class GameObject;

class Resource
{
public:
	Resource(R_TYPE t);
	virtual ~Resource();

	int GetUID();
	R_TYPE GetType();

public:
	std::string name;
	std::string assetsFile;
	std::string libraryFile;

	uint count;
	R_TYPE type;

protected:
	u32 uid;
};