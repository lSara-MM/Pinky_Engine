#include "R_Texture.h"
#include "R_Mesh.h"

R_Texture::R_Texture() : Resource(R_TYPE::TEXTURE)
{
	name = "None";

	tex_id = 0;
	tex_width = 0;
	tex_height = 0;
	tex_type = 0;
	tex_format = 0;

	assetsFile = "";
	libraryFile = "";
}

R_Texture::~R_Texture()
{
	DeleteBuffers();
	tex_id = 0;
}

bool R_Texture::InitBuffers()
{
	return false;
}

void R_Texture::DeleteBuffers()
{
	glDeleteBuffers(1, &tex_id);
}