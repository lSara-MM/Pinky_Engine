#ifndef __MESH_H__
#define __MESH_H__

#pragma once
#include "include/cimport.h"
#include "include/scene.h"
#include "include/postprocess.h"

#include "../../Globals.h"
#include "../Glew/include/glew.h"
#include "../MathGeoLib/include/MathGeoLib.h"

#include "../MathGeoLib/include/Math/float3x3.h"
#include "../MathGeoLib/include/Math/float4x4.h"
#include "../MathGeoLib/include/Math/Quat.h"

#include "../DevIL/include/il.h"
#include "../DevIL/include/ilu.h"
#include "../DevIL/include/ilut.h"

#pragma comment (lib, "../DevIL/libx86/DevIL.lib")
#pragma comment (lib, "../DevIL/libx86/ILU.lib")
#pragma comment (lib, "../DevIL/libx86/ILUT.lib")

#include <array>

class GameObject;

namespace ai
{
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

	struct texture
	{
		uint tex_id = 0;
		uint tex_width = 0;
		uint tex_height = 0;
		uint tex_type = 0;
		uint tex_format = 0;

		const char* path = "";

		~texture()
		{
			glDeleteBuffers(1, &tex_id);
			tex_id = 0;
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

		//texture UVs
		GLuint id_tex_uvs = 0;
		uint num_tex_uvs = 0;
		math::float2* tex_uvs = nullptr;

		texture tex;

		GLuint VBO; // vertex buffer object
		GLuint EBO; // element buffer object
		GLuint VAO; // attribute buffer object

		//Frustum culling
		AABB local_aabb;//local AABB

		~mesh()
		{
			RELEASE_ARRAY(index);
			RELEASE_ARRAY(vertex);
			RELEASE_ARRAY(normals);
			RELEASE_ARRAY(tex_uvs);

			tex.~texture();
		}
	};

	void EnableDebug();
	void DisableDebug();

	void ImportFile(const char* fileDir);

	bool ImportMesh(const char* meshfileDir, GameObject* go = nullptr, bool component = false);
	GameObject* MeshHierarchy(const aiScene* s, aiNode** children, int num, GameObject* parent, bool component = false);
	
	bool InitMesh(mesh* m);
	void CreatePolyPrimitive(POLY_PRIMITIVE_TYPE obj, GameObject* go = nullptr, bool component = false);
	void CreateCustomMehses(CUSTOM_MESH obj, GameObject* go = nullptr);

	void DeleteSelectedMesh(mesh* m);
	void DeleteMeshBuffers(mesh* m);

	// Textures
	void LoadCheckers(GLuint& buffer);
	bool BindTexture(mesh* m);
	void ImportTexture(mesh* m, const char* texturefileDir);
}
#endif //!__MESH_H__