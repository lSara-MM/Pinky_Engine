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
	bool Import(const aiMesh* mesh, ai::mesh* ourMesh);	// TODO: delete this one with assimp
	uint64 Save(const R_Mesh* ourMesh, char** fileBufferCursor);
	void Load(R_Mesh* ourMesh);
}
#endif //!__IMPORTER_MESH_H__