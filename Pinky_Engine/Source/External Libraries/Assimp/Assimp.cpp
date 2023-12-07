#pragma once
#include "Assimp.h"
#include "../../Application.h"
#include "../../parsingJSON.h"
#include "../../GameObject.h"
#include "../../ModuleScene.h"

#include "../ModuleResource.h"
#include "../R_Mesh.h"
#include "../Resource.h"

#pragma comment (lib, "Source/External Libraries/Assimp/libx86/assimp.lib")

#include <gl/GL.h>
#include <gl/GLU.h>

#include "../I_Mesh.h"


void ai::EnableDebug()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
	LOG("Enable debug mode");
}

void ai::DisableDebug()
{
	// Detach log stream
	aiDetachAllLogStreams();
	LOG("Disable debug mode");
}


GameObject* ai::ImportMesh(const char* meshfileDir, GameObject* go, bool component)
{
	const aiScene* scene = aiImportFile(meshfileDir, aiProcessPreset_TargetRealtime_MaxQuality);
	GameObject* ret = nullptr;

	if (scene != nullptr && scene->HasMeshes())
	{
		if (go != nullptr)
		{
			(MeshHierarchy(scene, scene->mRootNode->mChildren, scene->mRootNode->mNumChildren, go, component) != nullptr) ?
				ret = go : ret = nullptr;
		}

		// If the imported 3D model has many meshes at the same level, create a new GameObject with the
		// file name as a parent to group them all
		else if (scene->mRootNode->mNumChildren > 1)
		{
			// --- Get file name ---
			std::string filePath, fileName, fileExt, tempName, finalPath;
			App->fs->SplitFilePath(meshfileDir, &filePath, &fileName, &fileExt);
			// ---------------------------------------------

			GameObject* obj = new GameObject(fileName);
			MeshHierarchy(scene, scene->mRootNode->mChildren, scene->mRootNode->mNumChildren, obj);
			ret = obj;
			obj = nullptr;
		}
		else
		{
			ret = MeshHierarchy(scene, scene->mRootNode->mChildren, scene->mRootNode->mNumChildren, App->scene->rootNode);
		}

		if (ret != nullptr)
		{
			LOG("%d meshes loaded.", scene->mNumMeshes);
			App->parson->CreatePrefabFromGO(ret);
		}
		else { LOG("[ERROR] Couldn't load mesh.", scene->mNumMeshes); }

		aiReleaseImport(scene);
	}
	else
	{
		LOG("[ERROR] loading scene % s", meshfileDir);
		return nullptr;
	}

	return ret;
}

GameObject* ai::MeshHierarchy(const aiScene* s, aiNode** children, int num, GameObject* parent, bool component, bool foundParent)
{
	GameObject* obj = nullptr;

	static float3 ai_position = { 0,0,0 };
	static float3 ai_rotation = { 0, 0, 0 };
	static float3 ai_scale = { 0,0,0 }; 
	
	//------
	for (int i = 0; i < num; i++)
	{
		if (component)
		{
			obj = parent;
		}
		else
		{
			// --- Create new GameObject to store the mesh ---
			std::string name = children[i]->mName.C_Str();
			int posInString = name.find_first_of("$");

			//(posInString == std::string::npos) ? obj = new GameObject(name, parent) : obj = parent;

			if (posInString == std::string::npos)
			{
				obj = new GameObject(name, parent);
				LoadTranslation(false, children[i], obj, parent, name);
			}
			else
			{
				obj = parent;
				LoadTranslation(true, children[i], obj, parent, name);
			}

			if (!foundParent && posInString == std::string::npos)
			{
				foundParent = true;
			}
		}

		if (children[i]->mChildren != NULL)
		{
			if (!foundParent)
			{
				obj = MeshHierarchy(s, children[i]->mChildren, children[i]->mNumChildren, obj, component, foundParent);
			}
			else
			{
				MeshHierarchy(s, children[i]->mChildren, children[i]->mNumChildren, obj, component, foundParent);
			}
		}

		if (children[i]->mNumMeshes > 0)
		{
			const aiMesh* m = s->mMeshes[children[i]->mMeshes[0]];
			R_Mesh* mesh = new R_Mesh();

			if (!I_Mesh::Import(m, mesh))
			{
				//obj->~GameObject();
				obj = nullptr;
				return nullptr;
			}

			std::string path = App->resource->SaveToLibrary(mesh);
			App->resource->vResources.push_back(mesh);

			/*RELEASE(mesh);
			mesh = static_cast<R_Mesh*>(App->resource->LoadFromLibrary(path, R_TYPE::MESH));*/

			ai_b_position = false;
			ai_b_rotation = false;
			ai_b_scale = false;

			obj->transform->globalMatrix = math::float4x4::FromTRS(obj->transform->position,
				obj->transform->rotation, obj->transform->scale);

			//---Mesh---
			obj->AddComponent(C_TYPE::MESH, mesh);

			//---Material---
			if (!component) { obj->AddComponent(C_TYPE::MATERIAL); }
			m = nullptr;
		}
	}

	return obj;
}

void ai::CreatePolyPrimitive(POLY_PRIMITIVE_TYPE obj, GameObject* go, bool component)
{
	switch (obj)
	{
	case ai::POLY_PRIMITIVE_TYPE::CUBE:
		ImportMesh("PinkyAssets\\3dObject\\cube.fbx", go, component);
		break;
	case ai::POLY_PRIMITIVE_TYPE::SPHERE:
		ImportMesh("PinkyAssets\\3dObject\\sphere.fbx", go, component);
		break;
	case ai::POLY_PRIMITIVE_TYPE::CYLINDER:
		ImportMesh("PinkyAssets\\3dObject\\cylinder.fbx", go);
		break;
	case ai::POLY_PRIMITIVE_TYPE::PLANE:
		ImportMesh("PinkyAssets\\3dObject\\plane.fbx", go);
		break;
	default:
		break;
	}
}

void ai::CreateCustomMehses(CUSTOM_MESH obj)
{
	switch (obj)
	{
	case ai::CUSTOM_MESH::LAW:
		App->resource->ImportModel("PinkyAssets\\Custom\\law_hat\\trafalgar-laws-hat.fbx", std::vector<const char*> {"PinkyAssets\\Custom\\law_hat\\law_hat_mat_BaseColor.dds"});
		break;
	case ai::CUSTOM_MESH::KURO:
		App->resource->ImportModel("PinkyAssets\\Custom\\kuro\\kuro.fbx", std::vector<const char*> {"PinkyAssets\\Custom\\kuro\\BODYKURO.dds"});
		break;
	case ai::CUSTOM_MESH::SHARK:
		App->resource->ImportModel("PinkyAssets\\Custom\\king_shark\\kingsharksketch.fbx", std::vector<const char*> {"PinkyAssets\\Custom\\king_shark\\king_shark_tex.dds"});
		break;
	default:
		break;
	}
}

void ai::LoadTranslation(bool assimp, aiNode* children, GameObject* obj, GameObject* parent, std::string name)
{
	//---Assimp translation---
	aiVector3D pos, scale;
	aiQuaternion rot;

	children->mTransformation.Decompose(scale, rot, pos);

	if (!assimp)
	{
		if (!ai_b_position)
		{
			ai_position = { pos.x, pos.y, pos.z };
		}
		obj->transform->SetTransform(ai_position);

		if (!ai_b_rotation)
		{
			float temp1[4] = { rot.x , rot.y, rot.z, rot.w };
			float3 euler = Quat(temp1).ToEulerXYZ();

			ai_rotation = { euler.x * RADTODEG, euler.y * RADTODEG, euler.z * RADTODEG };
		}
		obj->transform->SetRotation(ai_rotation);

		if (!ai_b_scale)
		{
			ai_scale = { scale.x, scale.y, scale.z };
		}
		obj->transform->SetScale(ai_scale);
	}
	else
	{
		if (name.find("_Translation\0") != std::string::npos)
		{
			ai_b_position = true;

			ai_position = { pos.x, pos.y, pos.z };
		}

		if (name.find("_Rotation\0") != std::string::npos || name.find("_RotationPivot") != std::string::npos)
		{
			ai_b_rotation = true;

			float temp1[4] = { rot.x , rot.y, rot.z, rot.w };
			float3 euler = Quat(temp1).ToEulerXYZ();

			ai_rotation = { euler.x * RADTODEG, euler.y * RADTODEG, euler.z * RADTODEG };
		}

		if (name.find("_Scaling") != std::string::npos)
		{
			ai_b_scale = false;

			ai_scale = { scale.x, scale.y, scale.z };
		}
	}
}

void ai::LoadCheckers(GLuint& buffer)
{
	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &buffer);
	glBindTexture(GL_TEXTURE_2D, buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
}