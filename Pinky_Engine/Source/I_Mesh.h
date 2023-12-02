#ifndef __IMPORTER_MESH_H__
#define __IMPORTER_MESH_H__

#pragma once
#include "External Libraries/Assimp/Assimp.h"

#include "Globals.h"
#include <array>

#include "R_Mesh.h"

namespace I_Mesh
{
	bool Import(const aiMesh* mesh, R_Mesh* ourMesh);
	uint64 Save(const R_Mesh* ourMesh, char** buffer);
	void Load(R_Mesh* ourMesh, const char* buffer);
}
#endif //!__IMPORTER_MESH_H__