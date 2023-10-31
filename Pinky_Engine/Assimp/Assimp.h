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

#include "../MathGeoLib/include/Math/float3x3.h"
#include "../MathGeoLib/include/Math/float4x4.h"
#include "../MathGeoLib/include/Math/Quat.h"

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

	struct texture
	{
		uint tex_id = 0;
		uint tex_width = 0;
		uint tex_height = 0;

		std::string path;

		~texture()
		{

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

		~mesh()
		{
			RELEASE_ARRAY(index);
			RELEASE_ARRAY(vertex);
			RELEASE_ARRAY(normals);
			RELEASE_ARRAY(tex_uvs);

			//tex_uvs.~texture();
		}
	};

	void EnableDebug();
	void DisableDebug();

	void ImportFile(const char* fileDir);

	bool ImportMesh(const char* meshfileDir, GameObject* go = nullptr, const char* texfileDir = nullptr);
	bool MeshHierarchy(const aiScene* s, aiNode** children, int num, GameObject* parent, bool component = false);
	
	bool InitMesh(mesh* m);
	void CreatePolyPrimitive(POLY_PRIMITIVE_TYPE obj, GameObject* go = nullptr);

	void DeleteLastMesh();	// TODO: remove cuando se pueda seleccionar una mesh
	void DeleteSelectedMesh(mesh* m);
	void DeleteMeshBuffers(mesh* m);

	// Textures
	void LoadCheckers(GLuint& buffer);
	bool BindTexture(mesh* m);
	GLuint ImportTexture(const char* texturefileDir);
}
#endif //!__MESH_H__