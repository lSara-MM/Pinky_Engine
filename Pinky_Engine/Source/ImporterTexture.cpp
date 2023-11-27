#include "ImporterTexture.h"

#pragma once

void I_Texture::Import(const aiTexture* Texture, ai::texture* ourTexture)
{
}

uint64 I_Texture::Save(const ai::texture* ourTexture, char** fileBuffer)
{
	ILuint size;
	ILubyte* data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5); // To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, nullptr, 0); // Get the size of the data buffer
	if (size > 0) {
		data = new ILubyte[size]; // allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
		{
			*fileBuffer = (char*)data;
			RELEASE_ARRAY(data);
		}
	}
	else
	{
		ILenum error = ilGetError();
		if (error != IL_NO_ERROR)
		{
			LOG("[ERROR] when saving %s - %d: %s", ourTexture->path, error, iluErrorString(error));
		}
	}

	return size;
}

void I_Texture::Load(const char* fileBuffer, ai::texture* ourTexture)
{
}
