#pragma once
#include "Globals.h"
#include "Light.h"

#include "Resource.h"

#include "External Libraries/MathGeoLib/include/Math/float3x3.h"
#include "External Libraries/MathGeoLib/include/Math/float4x4.h"
#include "External Libraries/MathGeoLib/include/Math/Quat.h"

#include "External Libraries/Assimp/Assimp.h"

enum class POLY_PRIMITIVE_TYPE
{
	CUBE,
	SPHERE,
	CYLINDER,
	PLANE
};

enum class CUSTOM_MESH
{
	LAW,
	KURO,
	SHARK,
};

class R_Mesh : public Resource
{
public:
	R_Mesh() {};
	virtual ~R_Mesh() { };

public:

	//indices
	uint num_index = 0;
	uint* index = nullptr;

	//vertices
	uint num_vertex = 0;
	float* vertex = nullptr;

	//normals
	GLuint id_normals = 0;
	uint num_normals = 0;
	float* normals = nullptr;

	//texture UVs
	GLuint id_tex_uvs = 0;
	uint num_tex_uvs = 0;
	math::float2* tex_uvs = nullptr;

	//texture tex;

	GLuint VBO; // vertex buffer object
	GLuint EBO; // element buffer object
	GLuint VAO; // attribute buffer object

	//Frustum culling
	AABB local_aabb;//local AABB

private:
};