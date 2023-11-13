#ifndef __IMPORTER_TEXTURE_H__
#define __IMPORTER_TEXTURE_H__

#pragma once
#include "../Assimp/Assimp.h"

#include "Globals.h"
#include <array>

class GameObject;

namespace ImpTexture
{
	void Import(const aiTexture* mesh, ai::texture* ourMesh);
	uint64 Save(const ai::texture* ourMesh, char** fileBuffer);
	void Load(const char* fileBuffer, ai::texture* ourMesh);
}
#endif //!__IMPORTER_TEXTURE_H__