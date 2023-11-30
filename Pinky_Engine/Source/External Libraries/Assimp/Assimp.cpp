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

bool ai::ImportMesh(const char* meshfileDir, GameObject* go, bool component)
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
			std::string name = meshfileDir;
			// --- Get file name ---
			int posI = name.find_last_of("\\") + 1;
			int posF = name.find_last_of(".");

			name = name.substr(posI, posF - posI);	// first position, size of the string to get
			// ---------------------------------------------

			GameObject* obj = new GameObject(name);
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
			App->parson->CreateGOMetaFile(ret);
		}
		else { LOG("[ERROR] Couldn't load mesh.", scene->mNumMeshes); }

		aiReleaseImport(scene);
	}
	else
	{
		LOG("[ERROR] loading scene % s", meshfileDir);
		return false;
	}

	ret = nullptr;
	return true;
}

GameObject* ai::MeshHierarchy(const aiScene* s, aiNode** children, int num, GameObject* parent, bool component, bool foundParent)
{
	GameObject* obj = nullptr;

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
			int pos = name.find_first_of("$");

			(pos == std::string::npos) ? obj = new GameObject(name, parent) : obj = parent;

			if (!foundParent && pos == std::string::npos)
			{
				foundParent = true;
			}
		}

		if (children[i]->mChildren != NULL)
		{
			if (!foundParent)
			{
				obj = MeshHierarchy(s, children[i]->mChildren, children[i]->mNumChildren, obj, foundParent);
			}
			else
			{
				MeshHierarchy(s, children[i]->mChildren, children[i]->mNumChildren, obj, foundParent);
			}			
		}

		if (children[i]->mNumMeshes > 0)
		{
			const aiMesh* m = s->mMeshes[children[i]->mMeshes[0]];
			mesh* ourMesh = new mesh;
			R_Mesh* mesh = new R_Mesh();

			if (!I_Mesh::Import(m, mesh))
			{
				//obj->~GameObject();
				obj = nullptr;
				return nullptr;
			}

			
			if (mesh->InitBuffers()/*InitMesh(ourMesh)*/)
			{
				//BindTexture(ourMesh);

				//(texfileDir != nullptr) ? ourMesh->hasTex = true : ourMesh->hasTex = false;

				//---Transform---
				aiVector3D pos, scale;
				aiQuaternion rot;
				children[i]->mTransformation.Decompose(scale, rot, pos);
		

				float3 temp = { pos.x, pos.y, pos.z };
				obj->transform->SetTransform(temp);

				float temp1[4] = { rot.x , rot.y, rot.z, rot.w };
				float3 euler = Quat(temp1).ToEulerXYZ();
				float3 eulerF = { euler.x * RADTODEG, euler.y * RADTODEG, euler.z * RADTODEG };
				obj->transform->SetRotation(eulerF);
				
				float3 temp2 = { scale.x, scale.y, scale.z };
				obj->transform->SetScale(temp2);

				obj->transform->globalMatrix = math::float4x4::FromTRS(obj->transform->position, 
					obj->transform->rotation, obj->transform->scale);

				//---Local AABB---
				ourMesh->local_aabb.SetNegativeInfinity();
				ourMesh->local_aabb.Enclose((float3*)ourMesh->vertex, ourMesh->num_vertex);

				//---Mesh---
				obj->AddComponent(C_TYPE::MESH, ourMesh);				

				//---Material---
				if (!component) { obj->AddComponent(C_TYPE::MATERIAL, ourMesh); }

				//TODO: pushback elsewhere
				App->renderer3D->meshes.push_back(ourMesh);

				App->resource->SaveToLibrary(mesh);
			}

			//TODO: does it go here?
			ourMesh->local_aabb.SetNegativeInfinity();
			ourMesh->local_aabb.Enclose((float3*)ourMesh->vertex, ourMesh->num_vertex);

			m = nullptr;
			ourMesh = nullptr;
		}
	}

	return obj;
}

bool ai::InitMesh(mesh* m)
{
	m->VBO = 0;
	m->EBO = 0;
	m->VAO = 0;

	glGenBuffers(1, &m->VBO);
	glGenBuffers(1, &m->EBO);
	//glGenVertexArrays(1, &m->VAO);
	//normals
	glGenBuffers(1, &m->id_normals);
	//texture coordinates
	glGenBuffers(1, &m->id_tex_uvs);

	if (m->VBO == 0 || m->EBO == 0 || /*m->VAO == 0 ||*/
		m->id_normals == 0 || m->id_tex_uvs == 0)
	{
		LOG("[ERROR] buffer not created");
		return false;
	}

	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, m->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->num_vertex * 3, m->vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * m->num_index, m->index, GL_STATIC_DRAW); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// VAO
	/*glBindVertexArray(m->VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);*/

	//normals
	glBindBuffer(GL_ARRAY_BUFFER, m->id_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->num_normals * 3, m->normals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//texture coordinates
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->id_tex_uvs);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * m->num_tex_uvs * 2, m->tex_uvs, GL_STATIC_DRAW);

	return true;
}

void ai::CreatePolyPrimitive(POLY_PRIMITIVE_TYPE obj, GameObject* go, bool component)
{
	switch (obj)
	{
	case ai::POLY_PRIMITIVE_TYPE::CUBE:
		ImportMesh("Assets\\3dObject\\cube.fbx", go, component);
		break;
	case ai::POLY_PRIMITIVE_TYPE::SPHERE:
		ImportMesh("Assets\\3dObject\\sphere.fbx", go, component);
		break;
	case ai::POLY_PRIMITIVE_TYPE::CYLINDER:
		ImportMesh("Assets\\3dObject\\cylinder.fbx", go);
		break;
	case ai::POLY_PRIMITIVE_TYPE::PLANE:
		ImportMesh("Assets\\3dObject\\plane.fbx", go);
		break;
	default:
		break;
	}
}

void ai::CreateCustomMehses(CUSTOM_MESH obj, GameObject* go)
{
	switch (obj)
	{
	case ai::CUSTOM_MESH::LAW:
		ImportMesh("Assets\\Custom\\law_hat\\trafalgar-laws-hat.fbx", go);
		App->resource->ImportFile("Assets\\Custom\\law_hat\\law_hat_mat_BaseColor.dds");
		break;
	case ai::CUSTOM_MESH::KURO:
		ImportMesh("Assets\\Custom\\kuro\\kuro.fbx", go);
		App->resource->ImportFile("Assets\\Custom\\kuro\\BODYKURO.dds");
		break;
	case ai::CUSTOM_MESH::SHARK:
		ImportMesh("Assets\\Custom\\king_shark\\kingsharksketch.fbx", go);
		App->resource->ImportFile("Assets\\Custom\\king_shark\\king_shark_tex.dds");
		break;
	default:
		break;
	}
}

void ai::DeleteSelectedMesh(mesh* m)
{
	std::vector<ai::mesh*>* vMesh = &App->renderer3D->meshes;

	LOG("Deleted mesh");
	DeleteMeshBuffers(m);

	if (!vMesh->empty())
	{
		App->renderer3D->meshes.erase(std::find(vMesh->begin(), vMesh->end(), m));
		App->renderer3D->meshes.shrink_to_fit();
	}
}

void ai::DeleteMeshBuffers(mesh* m)
{
	glDeleteBuffers(1, &m->VBO);
	m->VBO = 0;
	glDeleteBuffers(1, &m->EBO);
	m->EBO = 0;
	glDeleteBuffers(1, &m->id_normals);
	m->id_normals = 0;
	glDeleteBuffers(1, &m->id_tex_uvs);
	m->id_tex_uvs = 0;
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

bool ai::BindTexture(mesh* m)
{
	//texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, m->id_tex_uvs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->num_tex_uvs * 2, m->tex_uvs, GL_STATIC_DRAW);
	return true;
}

void ai::ImportTexture(mesh* m, const char* texturefileDir)
{
	ILuint imageID = 0;
	ILuint textureID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ILubyte* data = ilGetData();

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	ILboolean success;
	success = ilLoadImage(texturefileDir);

	if (ilLoadImage(texturefileDir))
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);

		//Flip the image into the right way
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		if (!ilConvertImage(ilGetInteger(IL_IMAGE_FORMAT), IL_UNSIGNED_BYTE))
		{
			LOG("[ERROR] %s", iluErrorString(ilGetError()));
		}

		m->tex.tex_width = ilGetInteger(IL_IMAGE_WIDTH);
		m->tex.tex_height = ilGetInteger(IL_IMAGE_HEIGHT);
		m->tex.tex_type = ilGetInteger(IL_IMAGE_TYPE);
		m->tex.tex_format = ilGetInteger(IL_IMAGE_FORMAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, m->tex.tex_format, m->tex.tex_width, m->tex.tex_height, 0, m->tex.tex_format, GL_UNSIGNED_BYTE, ilGetData());
	}
	else
	{
		LOG("[ERROR] %s", iluErrorString(ilGetError()));
	}

	//ilBindImage(0);
	ilDeleteImages(1, &textureID);

	m->tex.path = texturefileDir;
	m->tex.tex_id = textureID;
}
