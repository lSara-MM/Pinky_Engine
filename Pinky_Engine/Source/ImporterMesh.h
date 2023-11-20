#ifndef __IMPORTER_MESH_H__
#define __IMPORTER_MESH_H__

#pragma once
#include "External Libraries/Assimp/Assimp.h"

#include "Globals.h"
#include <array>

class GameObject;

namespace I_Mesh
{
	bool Import(const aiMesh* mesh, ai::mesh* ourMesh);
	uint64 Save(const ai::mesh* ourMesh, char** fileBufferCursor);
	void Load(ai::mesh* ourMesh);
}
#endif //!__IMPORTER_MESH_H__