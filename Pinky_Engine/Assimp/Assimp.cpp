#include "Assimp.h"

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
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
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
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&ourMesh->index[i * 3], m->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
		}

		LOG("%d meshes loaded", scene->mNumMeshes);
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene % s", fileDir);
		return false;
	}
	return true;
}