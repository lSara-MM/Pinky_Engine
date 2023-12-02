#ifndef __IMPORTER_TEXTURE_H__
#define __IMPORTER_TEXTURE_H__

#pragma once
#include "External Libraries/Assimp/Assimp.h"

#include "Globals.h"
#include <array>

#include "R_Texture.h"

namespace I_Texture
{
	void Import(const aiTexture* tex, R_Texture* ourTex);
	uint64 Save(const R_Texture* ourTex, char** fileBuffer);
	bool Load(R_Texture* ourTex, const char* buffer, uint size);
}
#endif //!__IMPORTER_TEXTURE_H__