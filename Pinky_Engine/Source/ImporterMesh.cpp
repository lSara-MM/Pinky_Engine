#include "ImporterMesh.h"

#pragma once

bool I_Mesh::Import(const aiMesh* mesh, ai::mesh* ourMesh)
{
	// copy vertices
	ourMesh->num_vertex = mesh->mNumVertices;
	ourMesh->vertex = new float[ourMesh->num_vertex * 3];
	memcpy(ourMesh->vertex, mesh->mVertices, sizeof(float) * ourMesh->num_vertex * 3);

	LOG("New mesh %s with %d vertices", mesh->mName.C_Str(), mesh->mNumVertices);

	// copy faces
	if (mesh->HasFaces())
	{
		ourMesh->num_index = mesh->mNumFaces * 3;
		ourMesh->index = new uint[ourMesh->num_index]; // assume each face is a triangle

		for (uint i = 0; i < mesh->mNumFaces; ++i)
		{
			if (mesh->mFaces[i].mNumIndices != 3)
			{
				LOG("[WARNING], geometry face with != 3 indices!");
				
				mesh = nullptr;
				ourMesh = nullptr;
				return false;
			}
			else
			{
				memcpy(&ourMesh->index[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}
		}
	}

	//copy normals
	ourMesh->num_normals = mesh->mNumVertices;
	ourMesh->normals = new float[ourMesh->num_normals * 3];
	memcpy(ourMesh->normals, mesh->mNormals, sizeof(float) * ourMesh->num_normals * 3);

	////copy texture coordinates
	uint uv_index = 0;

	if (mesh->HasTextureCoords(uv_index))
	{
		ourMesh->num_tex_uvs = mesh->mNumVertices;
		ourMesh->tex_uvs = new math::float2[ourMesh->num_tex_uvs * 3];
		for (uint i = 0; i < ourMesh->num_tex_uvs; i++)
		{
			ourMesh->tex_uvs[i].x = mesh->mTextureCoords[uv_index][i].x;
			ourMesh->tex_uvs[i].y = mesh->mTextureCoords[uv_index][i].y;
		}
	}

	return true;
}

uint64 I_Mesh::Save(const ai::mesh* ourMesh, char** fileBufferCursor)
{
	// amount of indices / vertices / colors / normals / texture_coords / AABB
	uint ranges[4] = { ourMesh->num_index, ourMesh->num_vertex, ourMesh->num_normals, ourMesh->num_tex_uvs };
	uint size = sizeof(ranges) + sizeof(uint) * ourMesh->num_index + (sizeof(float) * 3) * (ourMesh->num_vertex + ourMesh->num_normals)
		+ sizeof(math::float2*) * ourMesh->num_tex_uvs;

	char* fileBuffer = new char[size]; // Allocate
	char** cursor = fileBufferCursor;
	
	// First store ranges
	uint bytes = sizeof(ranges); 
	memcpy(cursor, ranges, bytes);
	cursor += bytes;
	
	// Store indices
	bytes = sizeof(uint) * ourMesh->num_index;
	memcpy(cursor, ourMesh->index, bytes);
	cursor += bytes;

	// Store vertex
	bytes = sizeof(float) * ourMesh->num_vertex;
	memcpy(cursor, ourMesh->vertex, bytes);
	cursor += bytes;

	// Store normals
	bytes = sizeof(float) * ourMesh->num_normals;
	memcpy(cursor, ourMesh->normals, bytes);
	cursor += bytes;

	// Store texture UVs
	bytes = sizeof(math::float2) * ourMesh->num_tex_uvs;
	memcpy(cursor, ourMesh->tex_uvs, bytes);
	cursor += bytes;

	return size;
}

void I_Mesh::Load(ai::mesh* ourMesh)
{
	char* cursor = nullptr;
	// amount of indices / vertices / colors / normals / texture_coords
	uint ranges[5];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	ourMesh->num_index = ranges[0];
	ourMesh->num_vertex = ranges[1];
	ourMesh->num_normals = ranges[2];
	ourMesh->num_tex_uvs = ranges[3];

	// Load indices
	bytes = sizeof(uint) * ourMesh->num_index;
	ourMesh->index = new uint[ourMesh->num_index];
	memcpy(ourMesh->index, cursor, bytes);
	cursor += bytes;

	// Load vertex
	bytes = sizeof(float) * ourMesh->num_vertex * 3;
	ourMesh->vertex = new float[ourMesh->num_vertex * 3];
	memcpy(ourMesh->vertex, cursor, bytes);
	cursor += bytes;

	// Load normals
	bytes = sizeof(uint) * ourMesh->num_normals * 3;
	ourMesh->normals = new float[ourMesh->num_normals * 3];
	memcpy(ourMesh->normals, cursor, bytes);
	cursor += bytes;

	// Load textures UVs
	bytes = sizeof(math::float2) * ourMesh->num_tex_uvs;
	ourMesh->tex_uvs = new math::float2[ourMesh->num_tex_uvs];
	memcpy(ourMesh->tex_uvs, cursor, bytes);
	cursor += bytes;

	LOG("Mesh %s Loaded");
	RELEASE_ARRAY(cursor);
}