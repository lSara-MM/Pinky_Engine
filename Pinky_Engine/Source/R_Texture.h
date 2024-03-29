#pragma once
#include "Globals.h"
#include "Light.h"

#include "Resource.h"

class R_Mesh;

class R_Texture : public Resource
{
public:
	R_Texture();
	~R_Texture();

	bool InitBuffers();
	void DeleteBuffers();

public:

	uint tex_id;
	uint tex_width;
	uint tex_height;
	uint tex_type;
	uint tex_format;
};