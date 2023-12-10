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
	App->fs->SplitFilePath(meshfileDir, &dir);
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

			obj = new GameObject(name, parent);

			//---Transform---
			aiVector3D pos, scale;
			aiQuaternion rot;
			children[i]->mTransformation.Decompose(scale, rot, pos);


			float3 temp = { pos.x , pos.y, pos.z };
			obj->transform->SetTransform(temp);

			float temp1[4] = { rot.x , rot.y, rot.z, rot.w };
			float3 euler = Quat(temp1).ToEulerXYZ();
			float3 eulerF = { euler.x * RADTODEG, euler.y * RADTODEG, euler.z * RADTODEG };
			obj->transform->SetRotation(eulerF);

			float3 temp2 = { scale.x, scale.y, scale.z };
			obj->transform->SetScale(temp2);

			obj->transform->UpdateGlobalMatrix();

			if (!foundParent && posInString == std::string::npos)
			{
				GameObject* tempParent = obj->pParent;
				GameObject* tempAux = nullptr;

				while (tempParent->name.find_first_of("$") != std::string::npos)
				{
					if (tempParent->pParent->name.find_first_of("$") == std::string::npos)
					{
						tempAux = tempParent;
					}
					tempParent = tempParent->pParent;
				}
				if (tempAux != nullptr && tempAux->name.find_first_of("$") != std::string::npos)
				{
					obj->ReParent(tempParent);
					tempParent->DeleteChild(tempAux);
				}
				tempParent = nullptr;

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

			obj->transform->globalMatrix = math::float4x4::FromTRS(obj->transform->position,
				obj->transform->rotation, obj->transform->scale);

			//---Mesh---
			obj->AddComponent(C_TYPE::MESH, mesh);
			obj->mesh->mesh->name = obj->name;
			App->resource->vMeshesResources.push_back(mesh);
			App->resource->AddResource(mesh);


			//---Material---
			aiMaterial* mat = s->mMaterials[m->mMaterialIndex];
			uint numTex = mat->GetTextureCount(aiTextureType_DIFFUSE);

			if (!component)
			{
				// TODO: To test
				if (numTex != 0)
				{
					for (int i = 0; i < numTex; i++)
					{
						aiString aiPath;
						mat->GetTexture(aiTextureType_DIFFUSE, i, &aiPath);

						obj->AddComponent(C_TYPE::MATERIAL);
						std::string path = dir + aiPath.C_Str();

						// TODO: el path que da es del png, lo que se necesita es el dds por lo que se tiene que hacer un save y un load en el formato correcto
						static_cast<C_Material*>(obj->GetComponentByType(C_TYPE::MATERIAL))->tex->path = path.c_str();
						static_cast<C_Material*>(obj->GetComponentByType(C_TYPE::MATERIAL))->tex->ImportTexture(path.c_str());
					}
				}
				else
				{
					obj->AddComponent(C_TYPE::MATERIAL);
				}				
			}
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
		App->resource->ImportToScene("cube.fbx", PINKY_ASSETS_AUX "Custom\\", go, component);
		break;
	case ai::POLY_PRIMITIVE_TYPE::SPHERE:
		App->resource->ImportToScene("sphere.fbx", PINKY_ASSETS_AUX "3dObject\\", go, component);
		break;
	case ai::POLY_PRIMITIVE_TYPE::CYLINDER:
		App->resource->ImportToScene("cylinder.fbx", PINKY_ASSETS_AUX "3dObject\\", go, component);
		break;
	case ai::POLY_PRIMITIVE_TYPE::PLANE:
		App->resource->ImportToScene("plane.fbx", PINKY_ASSETS_AUX "3dObject\\", go, component);
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
		App->resource->ImportToScene("trafalgar-laws-hat.fbx", PINKY_ASSETS_AUX "Custom\\law_hat\\");
		App->resource->ImportToScene("law_hat_mat_BaseColor.dds", PINKY_ASSETS_AUX "Custom\\law_hat\\");
		break;
	case ai::CUSTOM_MESH::KURO:
		App->resource->ImportToScene("kuro.fbx", PINKY_ASSETS_AUX "Custom\\kuro\\");
		App->resource->ImportToScene("BODYKURO.dds", PINKY_ASSETS_AUX "Custom\\kuro\\");
		break;
	case ai::CUSTOM_MESH::SHARK:
		App->resource->ImportToScene("kingsharksketch.fbx", PINKY_ASSETS_AUX "Custom\\king_shark\\");
		App->resource->ImportToScene("king_shark_tex.dds", PINKY_ASSETS_AUX "Custom\\king_shark\\");
		break;
	default:
		break;
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