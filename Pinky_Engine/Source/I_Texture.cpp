#include "I_Texture.h"

#pragma once

void I_Texture::Import(const aiTexture* Texture, R_Texture* ourTexture)
{

}

uint64 I_Texture::Save(const R_Texture* ourTexture, char** fileBuffer)
{
	ILuint size;
	ILubyte* data;

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5); // To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, nullptr, 0); // Get the size of the data buffer
	
	if (size > 0) 
	{
		data = new ILubyte[size]; // allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
		{
			*fileBuffer = (char*)data;

			//size += sizeof(uint);
			//uint id = 0;
			//uint bytes = sizeof(id);
			//memcpy(data, &id, bytes);
			//data += bytes;

			//*fileBuffer = (char*)data;
			//RELEASE_ARRAY(data);
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

void I_Texture::Load(R_Texture* ourTex, const char* fileBuffer)
{
	ILuint imageID = 0;
	ILuint textureID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ILubyte* data = ilGetData();

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	ILboolean success;
	success = ilLoadImage(fileBuffer);

	if (ilLoadImage(fileBuffer))
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);

		//Flip the image into the right way
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		if (!ilConvertImage(ilGetInteger(IL_IMAGE_FORMAT), IL_UNSIGNED_BYTE))
		{
			LOG("[ERROR] %s", iluErrorString(ilGetError()));
		}

		ourTex->tex_width = ilGetInteger(IL_IMAGE_WIDTH);
		ourTex->tex_height = ilGetInteger(IL_IMAGE_HEIGHT);
		ourTex->tex_type = ilGetInteger(IL_IMAGE_TYPE);
		ourTex->tex_format = ilGetInteger(IL_IMAGE_FORMAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, ourTex->tex_format, ourTex->tex_width, ourTex->tex_height, 0, ourTex->tex_format, GL_UNSIGNED_BYTE, ilGetData());
	}
	else
	{
		LOG("[ERROR] %s", iluErrorString(ilGetError()));
	}

	/*const char* cursor = fileBuffer;
	uint ranges;
	uint bytes = sizeof(ranges) + sizeof(uint) * 4;
	memcpy(&ranges, cursor, bytes);
	cursor += bytes;*/

	//ilBindImage(0);
	ilDeleteImages(1, &textureID);

	ourTex->path = fileBuffer;
	ourTex->tex_id = textureID;
}
