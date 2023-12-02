#include "I_Texture.h"

#pragma once

void I_Texture::Import(const aiTexture* Texture, R_Texture* ourTexture)
{

}

uint64 I_Texture::Save(const R_Texture* ourTexture, char** fileBuffer)
{
	ILuint size;
	ILubyte* cursor;

	ilEnable(IL_FILE_OVERWRITE);

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5); // To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, nullptr, 0); // Get the size of the data buffer

	if (size > 0)
	{
		cursor = new ILubyte[size]; 
		if (ilSaveL(IL_DDS, cursor, size) > 0) // Save to buffer with the ilSaveIL function
		{
			*fileBuffer = (char*)cursor;
			//RELEASE_ARRAY(cursor);
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

bool I_Texture::Load(R_Texture* ourTex, const char* buffer, uint size)
{
	const char* cursor = buffer;
	// amount of indices / vertices / colors / normals / texture_coords

	ILuint imageID = 0;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	//ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size);

	if (ilLoadL(IL_DDS, buffer, size) == IL_FALSE)
	{
		LOG("[ERROR] Could not load texture %s - %d: %s", ourTex->libraryFile.c_str(), ilGetError(), iluErrorString(ilGetError()));

		ilBindImage(0);
		ilDeleteImages(1, &imageID);
		return false;
	}
	else
	{
		// Convert the image into a suitable format to work with
		if (!ilConvertImage(ilGetInteger(IL_IMAGE_FORMAT), IL_UNSIGNED_BYTE))
		{
			LOG("[ERROR] %s", iluErrorString(ilGetError()));
			return false;
		}

		ILinfo i;
		iluGetImageInfo(&i);

		ourTex->tex_width = ilGetInteger(IL_IMAGE_WIDTH);
		ourTex->tex_height = ilGetInteger(IL_IMAGE_HEIGHT);
		ourTex->tex_type = ilGetInteger(IL_IMAGE_TYPE);
		ourTex->tex_format = ilGetInteger(IL_IMAGE_FORMAT);
		
		ourTex->tex_id = ilutGLBindTexImage();
		ilDeleteImages(1, &imageID);
	}

	return true;
}
