#include "Assimp.h"

#include "../Source/Globals.h"
#include "../Source/Application.h"
#include "../Source/ModuleInput.h"



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

bool ai::LoadObject(const char* fileDir)
{
	const aiScene* scene = aiImportFile(fileDir, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		const aiMesh* m = scene->mMeshes[0];
		mesh* ourMesh = new mesh;

		// copy vertices
		ourMesh->num_vertex = m->mNumVertices;
		ourMesh->vertex = new float[ourMesh->num_vertex * 3];
		memcpy(ourMesh->vertex, m->mVertices, sizeof(float) * ourMesh->num_vertex * 3);
		LOG("New mesh with %d vertices", m->mNumVertices);

		// copy faces
		if (m->HasFaces())
		{
			ourMesh->num_index = m->mNumFaces * 3;
			ourMesh->index = new uint[ourMesh->num_index]; // assume each face is a triangle
			for (uint i = 0; i < m->mNumFaces; ++i)
			{
				if (m->mFaces[i].mNumIndices != 3)
				{
					LOG("WARNING, geometry face with != 3 indices!");
				}
				else
				{
					memcpy(&ourMesh->index[i * 3], m->mFaces[i].mIndices, 3 * sizeof(uint));
				}
			}
		}
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene % s", fileDir);
		return false;
	}
	return true;
}
