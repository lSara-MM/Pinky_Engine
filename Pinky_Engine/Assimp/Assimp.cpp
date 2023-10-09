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
	glGenVertexArrays(1, &m->VAO);

	if (m->VBO == 0 || m->EBO == 0 || m->VAO == 0)
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * m->num_index * 3, m->index, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// VAO
	/*glBindVertexArray(m->VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);*/

	return true;
}

void ai::DeleteBuffers()
{
	for (auto i = 0; i < App->renderer3D->meshes.size(); i++)
	{
		if (App->renderer3D->meshes.at(i)->VBO != 0)
		{
			glDeleteBuffers(1, &App->renderer3D->meshes.at(i)->VBO);
			App->renderer3D->meshes.at(i)->VBO = 0;
		}
	}
}