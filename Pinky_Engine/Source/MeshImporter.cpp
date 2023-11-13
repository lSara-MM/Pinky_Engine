#include "MeshImporter.h"

#pragma once

void ImpMesh::Import(const aiMesh* mesh, ai::mesh* ourMesh)
{
}

uint64 ImpMesh::Save(const ai::mesh* ourMesh, char** fileBuffer)
{
	return uint64();
}

void ImpMesh::Load(const char* fileBuffer, ai::mesh* ourMesh)
{
}
