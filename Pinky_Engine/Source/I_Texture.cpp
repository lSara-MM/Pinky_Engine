#include "I_Texture.h"

bool I_Texture::Import(const char* path, R_Texture* ourTexture)
{
	ILuint imageID = 0;
	ILuint textureID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ILubyte* data = ilGetData();

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	if (ilLoadImage(path))
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);

		// Flip the image into the right way
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		if (!ilConvertImage(ilGetInteger(IL_IMAGE_FORMAT), IL_UNSIGNED_BYTE))
		{
			LOG("[ERROR] %s", iluErrorString(ilGetError()));
		}

		ourTexture->tex_width = ilGetInteger(IL_IMAGE_WIDTH);
		ourTexture->tex_height = ilGetInteger(IL_IMAGE_HEIGHT);
		ourTexture->tex_type = ilGetInteger(IL_IMAGE_TYPE);
		ourTexture->tex_format = ilGetInteger(IL_IMAGE_FORMAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, ourTexture->tex_format, ourTexture->tex_width, ourTexture->tex_height, 0, ourTexture->tex_format, GL_UNSIGNED_BYTE, ilGetData());
	}
	else
	{
		LOG("[ERROR] %s", iluErrorString(ilGetError()));
		return false;
	}

	//ilBindImage(0);
	ilDeleteImages(1, &textureID);
	ourTexture->path = path;
	ourTexture->tex_id = textureID;

	return true;
}

uint64 I_Texture::Save(const R_Texture* ourTexture, char** fileBuffer)
{
	ILuint size;
	ILubyte* cursor;

	ilEnable(IL_FILE_OVERWRITE);
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5); // To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, nullptr, 0); // Get the size of the data buffer

	if (ilLoadImage(ourTexture->path))
	{
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
				LOG("[ERROR] when saving: %d - %s", error, iluErrorString(error));
			}
		}
	}

	ilBindImage(0);
	return size;
}

void I_Texture::Load(R_Texture* ourTex, const char* buffer)
{
	ILuint imageID = 0;
	ILuint textureID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ILubyte* data = ilGetData();

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	if (ilLoadImage(buffer))
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);

		// Flip the image into the right way
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

	ilBindImage(0);
	ilDeleteImages(1, &textureID);
	ourTex->path = buffer;
	ourTex->tex_id = textureID;
}
