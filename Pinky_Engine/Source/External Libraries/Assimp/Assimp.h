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
#include <vector>

class GameObject;
class Resource;

namespace ai
{
	struct Transform
	{
		float3 pos = { 0,0,0 };
		math::float4 rotation = { 0, 0, 0, 0 };
		float3 scale = { 0,0,0 };
	};

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

	void EnableDebug();
	void DisableDebug();

	std::vector<Resource*>* ImportVMesh(const char* meshfileDir, GameObject* go = nullptr, bool component = false);
	GameObject* MeshVHierarchy(const aiScene* s, aiNode** children, int num, GameObject* parent,
		std::vector<Resource*>* vMeshes, bool component = false, bool foundParent = false);

	GameObject* ImportMesh(const char* meshfileDir, GameObject* go = nullptr, bool component = false);
	GameObject* MeshHierarchy(const aiScene* s, aiNode** children, int num, GameObject* parent, bool component = false, bool foundParent = false);
	
	void CreatePolyPrimitive(POLY_PRIMITIVE_TYPE obj, GameObject* go = nullptr, bool component = false);
	void CreateCustomMehses(CUSTOM_MESH obj);

	void LoadTranslation(bool assimp, aiNode* children, GameObject* obj, GameObject* parent, std::string name);
	
	// Textures
	void LoadCheckers(GLuint& buffer);

	//
	static float3 ai_position = { 0,0,0 };
	static float3 ai_rotation = { 0, 0, 0 };
	static float3 ai_scale = { 0,0,0 };

	static bool ai_b_position = false;
	static bool ai_b_rotation = false;
	static bool ai_b_scale = false;
}
#endif //!__MESH_H__