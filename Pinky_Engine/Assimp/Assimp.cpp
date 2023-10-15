#pragma once
#include "Assimp.h"
#include "../Source/Application.h"

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
	// detach log stream
	aiDetachAllLogStreams();
	LOG("Disable debug mode");
}

void ai::ImportFile(const char* fileDir)
{
	std::string dir = fileDir;
	std::array<std::string, 3> obj_ext = { ".fbx", ".FBX", ".obj", };
	std::array<std::string, 4> tex_ext = { ".png", ".PNG", ".jpg", ".JPG" };

	for (auto i = 0; i < obj_ext.size(); i++)
	{
		if (dir.find(obj_ext.at(i)) != std::string::npos)
		{
			ImportMesh(fileDir);
		}
	}
	
	for (auto i = 0; i < obj_ext.size(); i++)
	{
		if (dir.find(obj_ext.at(i)) != std::string::npos)
		{
			
		}
	}
}

bool ai::ImportMesh(const char* fileDir)
{
	const aiScene* scene = aiImportFile(fileDir, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (auto i = 0; i < scene->mNumMeshes; i++)
		{
			const aiMesh* m = scene->mMeshes[i];
			mesh* ourMesh = new mesh;

			// copy vertices
			ourMesh->num_vertex = m->mNumVertices;
			ourMesh->vertex = new float[ourMesh->num_vertex * 3];
			memcpy(ourMesh->vertex, m->mVertices, sizeof(float) * ourMesh->num_vertex * 3);

			// TODO preguntar: como pillar el nombre del objeto y no la de dentro de la mesh "outliner id data operation" (abrir en blender pa explicar bien xd) 
			LOG("New mesh %s with %d vertices", m->mName.C_Str(), m->mNumVertices);

			// copy faces
			if (m->HasFaces())
			{
				ourMesh->num_index = m->mNumFaces * 3;
				ourMesh->index = new uint[ourMesh->num_index]; // assume each face is a triangle

				for (uint i = 0; i < m->mNumFaces; ++i)
				{
					if (m->mFaces[i].mNumIndices != 3)
					{
						LOG("[WARNING], geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&ourMesh->index[i * 3], m->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}

			//copy normals
			ourMesh->num_normals = m->mNumVertices;
			ourMesh->normals = new float[ourMesh->num_normals * 3];
			memcpy(ourMesh->normals, m->mNormals, sizeof(float) * ourMesh->num_normals * 3);

			////copy texture coordinates
			uint uv_index = 0;

			if (m->HasTextureCoords(uv_index))
			{
				ourMesh->tex.num_tex = m->mNumVertices;
				ourMesh->tex.tex = new math::float2[ourMesh->tex.num_tex * 3];
				for (uint i = 0; i < ourMesh->tex.num_tex; i++)
				{
					ourMesh->tex.tex[i].x = m->mTextureCoords[uv_index][i].x;
					ourMesh->tex.tex[i].y = m->mTextureCoords[uv_index][i].y;
				}
			}

			if (InitMesh(ourMesh))
			{
				App->renderer3D->meshes.push_back(ourMesh);
			}
		}

		LOG("%d meshes loaded.", scene->mNumMeshes);
		aiReleaseImport(scene);
	}
	else
	{
		LOG("[ERROR] loading scene % s", fileDir);
		return false;
	}
	return true;
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
	glGenBuffers(1, &m->tex.id_tex);

	if (m->VBO == 0 || m->EBO == 0 || /*m->VAO == 0*/ ||
		m->id_normals == 0|| m->tex.id_tex == 0)
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * m->num_index, m->index, GL_STATIC_DRAW); //TODO: EN TEORIA FUNCIONA YA?
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

	//TODO: NO VA, ahora si¿?
	//texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, m->tex.id_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->tex.num_tex * 2, m->tex.tex, GL_STATIC_DRAW);

	App->renderer3D->LoadTexture(m->tex.id_tex);
	return true;
}

void ai::CreatePolyPrimitive(POLY_PRIMITIVE_TYPE obj)
{
	switch (obj)
	{
	case ai::POLY_PRIMITIVE_TYPE::CUBE:
		ImportMesh("../Assimp/3dObject/cube.fbx");
		break;
	case ai::POLY_PRIMITIVE_TYPE::SPHERE:
		ImportMesh("../Assimp/3dObject/sphere.fbx");
		break;
	case ai::POLY_PRIMITIVE_TYPE::CYLINDER:
		ImportMesh("../Assimp/3dObject/cylinder.fbx");
		break;
	case ai::POLY_PRIMITIVE_TYPE::PLANE:
		ImportMesh("../Assimp/3dObject/plane.fbx");
		break;
	default:
		break;
	}
}

void ai::DeleteLastMesh()
{
	std::vector<ai::mesh*>* m = &App->renderer3D->meshes;

	if (!m->empty())
	{
		LOG("Deleted last mesh created");

		DeleteMeshBuffers(*(m->end() - 1));

		m->back()->~mesh();
		m->erase(m->end() - 1);
		m->shrink_to_fit();
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
}