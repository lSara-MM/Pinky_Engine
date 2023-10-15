#ifndef __MESH_H__
#define __MESH_H__

#pragma once
#include "../Assimp/include/cimport.h"
#include "../Assimp/include/scene.h"
#include "../Assimp/include/postprocess.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "../Source/Globals.h"
#include "../Glew/include/glew.h"
#include "../MathGeoLib/include/MathGeoLib.h"

#include <array>

namespace ai
{
	enum class POLY_PRIMITIVE_TYPE
	{
		CUBE,	
		SPHERE,
		CYLINDER,
		PLANE
	};

	struct texture
	{
		//textures
		GLuint id_tex = 0;
		uint num_tex = 0;
		math::float2* tex = nullptr;

		~texture()
		{
			RELEASE_ARRAY(tex);

			glDeleteBuffers(1, &id_tex);
			id_tex = 0;
		}
	};

	struct mesh
	{
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

		texture tex;

		GLuint VBO; // vertex buffer object
		GLuint EBO; // element buffer object
		GLuint VAO; // attribute buffer object

		~mesh()
		{
			RELEASE_ARRAY(vertex);
			RELEASE_ARRAY(normals);
			tex.~texture();
			
			////DeleteMeshBuffers(this);
			//glDeleteBuffers(1, &VBO);
			//VBO = 0;
			//glDeleteBuffers(1, &EBO);
			//EBO = 0;
			//glDeleteBuffers(1, &id_normals);
			//id_normals = 0;
		}
	};

	void EnableDebug();
	void DisableDebug();

	void ImportFile(const char* fileDir);

	bool ImportMesh(const char* fileDir);
	bool InitMesh(mesh* m);
	void CreatePolyPrimitive(POLY_PRIMITIVE_TYPE obj);

	void DeleteLastMesh();	// TODO: remove cuando se pueda seleccionar una mesh
	void DeleteMeshBuffers(mesh* m);
}
#endif //!__MESH_H__