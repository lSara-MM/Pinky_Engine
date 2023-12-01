#include "R_Texture.h"
#include "R_Mesh.h"

R_Texture::R_Texture() : Resource(R_TYPE::TEXTURE)
{
	tex_id = 0;
	tex_width = 0;
	tex_height = 0;
	tex_type = 0;
	tex_format = 0;

	path = "";
}

R_Texture::~R_Texture()
{
	glDeleteBuffers(1, &tex_id);
	tex_id = 0;
}

bool R_Texture::InitBuffers()
{
	return false;
}

void R_Texture::DeleteBuffers()
{
}

bool R_Texture::BindTexture(R_Mesh* m)
{
	//texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, m->id_tex_uvs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->num_tex_uvs * 2, m->tex_uvs, GL_STATIC_DRAW);
	return true;
}

void R_Texture::ImportTexture(R_Mesh* m, const char* texturefileDir)
{
	ILuint imageID = 0;
	ILuint textureID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ILubyte* data = ilGetData();

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	ILboolean success;
	success = ilLoadImage(texturefileDir);

	if (ilLoadImage(texturefileDir))
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

		m->tex.tex_width = ilGetInteger(IL_IMAGE_WIDTH);
		m->tex.tex_height = ilGetInteger(IL_IMAGE_HEIGHT);
		m->tex.tex_type = ilGetInteger(IL_IMAGE_TYPE);
		m->tex.tex_format = ilGetInteger(IL_IMAGE_FORMAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, m->tex.tex_format, m->tex.tex_width, m->tex.tex_height, 0, m->tex.tex_format, GL_UNSIGNED_BYTE, ilGetData());
	}
	else
	{
		LOG("[ERROR] %s", iluErrorString(ilGetError()));
	}

	//ilBindImage(0);
	ilDeleteImages(1, &textureID);

	m->tex.path = texturefileDir;
	m->tex.tex_id = textureID;
}
