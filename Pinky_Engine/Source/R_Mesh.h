#pragma once
#include "Globals.h"
#include "Light.h"

#include "Resource.h"
#include "GameObject.h"


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
	R_Mesh();
	~R_Mesh();

	bool InitBuffers(R_Mesh* m);
	void DeleteBuffers(R_Mesh* m);

public:

	//indices
	uint num_index;
	uint* index;

	//vertices
	uint num_vertex;
	float* vertex;

	//normals
	GLuint id_normals;
	uint num_normals;
	float* normals;

	//texture UVs
	GLuint id_tex_uvs;
	uint num_tex_uvs;
	math::float2* tex_uvs;

	//texture tex;

	GLuint VBO; // vertex buffer object
	GLuint EBO; // element buffer object
	GLuint VAO; // attribute buffer object

	//Frustum culling
	AABB local_aabb;//local AABB

private:
};