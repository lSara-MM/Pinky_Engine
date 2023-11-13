#ifndef __MESH_IMPORTER_H__
#define __MESH_IMPORTER_H__

#pragma once
#include "../Assimp/Assimp.h"

#include "Globals.h"
#include <array>

class GameObject;

namespace ImpMesh
{
	void Import(const aiMesh* mesh, ai::mesh* ourMesh);
	uint64 Save(const ai::mesh* ourMesh, char** fileBuffer);
	void Load(const char* fileBuffer, ai::mesh* ourMesh);
}
#endif //!__MESH_IMPORTER_H__