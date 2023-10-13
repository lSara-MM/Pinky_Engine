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


namespace ai
{
	struct mesh
	{
		//indices
		uint id_index = 0; // index in VRAM
		uint num_index = 0;
		uint* index = nullptr;

		//vertices
		uint id_vertex = 0; // unique vertex in VRAM
		uint num_vertex = 0;
		float* vertex = nullptr;

		//normals
		uint id_normals = 0;
		uint num_normals = 0;
		float* normals = nullptr;

		//textures
		uint id_tex = 0;
		uint num_tex = 0;
		math::float2* tex = nullptr;

		GLuint VBO; // vertex buffer object
		GLuint EBO; // element buffer object
		GLuint VAO; // attribute buffer object
	};

	void EnableDebug();
	void DisableDebug();

	bool ImportMesh(const char* fileDir);
	bool InitMesh(mesh* m);
	void DeleteBuffers();

	//std::vector<mesh*> meshes;
}
#endif //!__MESH_H__