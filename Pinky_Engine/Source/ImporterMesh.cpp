#include "ImporterMesh.h"

#pragma once

void ImpMesh::Import(const aiMesh* mesh, ai::mesh* ourMesh)
{
}

uint64 ImpMesh::Save(const ai::mesh* ourMesh, char** fileBuffer)
{
	ILuint size;
	ILubyte* data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, nullptr, 0); // Get the size of the data buffer
	if (size > 0) {
		data = new ILubyte[size]; // allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
			*fileBuffer = (char*)data;
			RELEASE_ARRAY(data);
	}

	return uint64();
}

void ImpMesh::Load(const char* fileBuffer, ai::mesh* ourMesh)
{
}