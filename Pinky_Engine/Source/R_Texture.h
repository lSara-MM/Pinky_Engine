#pragma once
#include "Globals.h"
#include "Light.h"

#include "Resource.h"

#include "External Libraries/MathGeoLib/include/Math/float3x3.h"
#include "External Libraries/MathGeoLib/include/Math/float4x4.h"
#include "External Libraries/MathGeoLib/include/Math/Quat.h"

#include "External Libraries/Assimp/Assimp.h"

class R_Texture : public Resource
{
public:
	R_Texture();
	~R_Texture();

	bool InitBuffers();
	void DeleteBuffers();

public:

	uint tex_id;
	uint tex_width;
	uint tex_height;
	uint tex_type;
	uint tex_format;

	const char* path;

private:
};