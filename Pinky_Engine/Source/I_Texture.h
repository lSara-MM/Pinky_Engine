#ifndef __IMPORTER_TEXTURE_H__
#define __IMPORTER_TEXTURE_H__

#pragma once
#include "External Libraries/Assimp/Assimp.h"

#include "Globals.h"
#include <array>

#include "R_Texture.h"

namespace I_Texture
{
	bool Import(const char* path, R_Texture* ourTexture);
	uint64 Save(const R_Texture* ourTex, char** fileBuffer);
	void Load(R_Texture* ourTex, const char* buffer);
}
#endif //!__IMPORTER_TEXTURE_H__