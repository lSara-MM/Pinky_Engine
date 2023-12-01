#pragma once
#include "Globals.h"

#include "Resource.h"
#include "R_Texture.h"

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

	bool InitBuffers();
	void DeleteBuffers();
	void BindTexture(R_Texture* t);

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

	GLuint VBO; // vertex buffer object
	GLuint EBO; // element buffer object
	GLuint VAO; // attribute buffer object

	//Frustum culling
	AABB local_aabb;//local AABB

	R_Texture* tex;
	//ai::texture tex;	// TODO: change to materil resource o lo que sea
private:
};