#include "Globals.h"
#include "Application.h"
#include "ModuleResource.h"

#include "FileSystemManager.h"

#include "Resource.h"
#include "I_Mesh.h"
#include "I_Texture.h"

ModuleResource::ModuleResource(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleResource::~ModuleResource()
{
	
}

// -----------------------------------------------------------------
bool ModuleResource::Start()
{
	return true;
}

// -----------------------------------------------------------------
update_status ModuleResource::Update(float dt)
{

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
bool ModuleResource::CleanUp()
{
	LOG("Cleaning resources");

	return true;
}

void ModuleResource::ImportFile(const char* fileDir)
{
	std::string dir = fileDir;
	std::array<std::string, 3> obj_ext = { ".fbx", ".FBX", ".obj", };
	std::array<std::string, 6> tex_ext = { ".png", ".PNG", ".jpg", ".JPG", ".dds", ".DDS" };

	for (auto i = 0; i < obj_ext.size(); i++)
	{
		if (dir.find(obj_ext.at(i)) != std::string::npos)
		{
			ai::ImportMesh(fileDir);
			break;
		}
	}

	for (auto i = 0; i < tex_ext.size(); i++)
	{
		if (dir.find(tex_ext.at(i)) != std::string::npos)
		{
			for each (R_Mesh* i in App->renderer3D->meshes)
			{
				i->tex.ImportTexture(i, fileDir);
				//ai::ImportTexture(i, fileDir);
			}
		}
	}
}

std::string ModuleResource::SaveToLibrary(Resource* r)
{
	std::string path;

	char* buffer = nullptr;
	uint size = 0;

	switch (r->GetType())
	{
	case R_TYPE::MESH:
		path = MESHES_PATH;
		size = I_Mesh::Save(static_cast<R_Mesh*>(r), &buffer);
		break;
	case R_TYPE::TEXTURE:
		path = TEXTURES_PATH;
		size = I_Texture::Save(static_cast<R_Texture*>(r), &buffer);
		break;
	case R_TYPE::SCENE:
		break;
	case R_TYPE::NONE:
		break;
	default:
		break;
	}

	path += std::to_string(r->GetUID());
	path += ".pnk";

	App->fs->Save(path.c_str(), buffer, size);

	//RELEASE_ARRAY(buffer);	// TODO: peta jaja
	buffer = nullptr;
	return path;
}

Resource* ModuleResource::LoadFromLibrary(std::string path, std::string file, R_TYPE type)
{
	char* buffer = nullptr;

	Resource* r;
	App->fs->Load(path.c_str(), &buffer);

	switch (type)
	{
	case R_TYPE::MESH:
		r = new R_Mesh();
		I_Mesh::Load(static_cast<R_Mesh*>(r), buffer);
		break;
	case R_TYPE::TEXTURE:
		path = TEXTURES_PATH;
		//size = I_Texture::Load(static_cast<R_Texture*>(r), &buffer);
		break;
	case R_TYPE::SCENE:
		break;
	case R_TYPE::NONE:
		break;
	default:
		break;
	}

	//RELEASE_ARRAY(buffer);
	buffer = nullptr;
	return r;
}

bool ModuleResource::BindTexture(R_Mesh* m)
{
	//texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, m->id_tex_uvs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->num_tex_uvs * 2, m->tex_uvs, GL_STATIC_DRAW);
	return true;
}

void ModuleResource::ImportTexture(R_Mesh* m, const char* texturefileDir)
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
