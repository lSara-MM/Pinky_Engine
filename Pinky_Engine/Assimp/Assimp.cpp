#pragma once
#include "Assimp.h"
#include "../Source/Application.h"
#include "../Source/GameObject.h"
#include "../Source/ModuleScene.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "../DevIL/include/il.h"
#include "../DevIL/include/ilu.h"
#include "../DevIL/include/ilut.h"

#pragma comment (lib, "../DevIL/libx86/DevIL.lib")
#pragma comment (lib, "../DevIL/libx86/ILU.lib")
#pragma comment (lib, "../DevIL/libx86/ILUT.lib")


void ai::EnableDebug()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
	LOG("Enable debug mode");
}

void ai::DisableDebug()
{
	// detach log stream
	aiDetachAllLogStreams();
	LOG("Disable debug mode");
}

void ai::ImportFile(const char* fileDir)
{
	std::string dir = fileDir;
	std::array<std::string, 3> obj_ext = { ".fbx", ".FBX", ".obj", };
	std::array<std::string, 4> tex_ext = { ".png", ".PNG", ".jpg", ".JPG" };

	for (auto i = 0; i < obj_ext.size(); i++)
	{
		if (dir.find(obj_ext.at(i)) != std::string::npos)
		{
			ImportMesh(fileDir);
		}
	}

	for (auto i = 0; i < tex_ext.size(); i++)
	{
		if (dir.find(tex_ext.at(i)) != std::string::npos)
		{
			for each (ai::mesh * i in App->renderer3D->meshes)
			{
				i->tex.id_tex = ImportTexture(fileDir);
			}
		}
	}
}

bool ai::ImportMesh(const char* meshfileDir, const char* texfileDir)
{
	const aiScene* scene = aiImportFile(meshfileDir, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		if (scene->mNumMeshes > 1)
		{
			std::string name = meshfileDir;
			// Get file name
			int posI = name.find_last_of("\\") + 1;
			int posF = name.find_last_of(".");

			name = name.substr(posI, posF - posI);	// first position, size of the string to get
			//

			GameObject* obj = new GameObject(name);
			MeshHierarchy(scene, scene->mRootNode->mChildren, scene->mRootNode->mNumChildren, obj);
		}
		else
		{
			MeshHierarchy(scene, scene->mRootNode->mChildren, scene->mRootNode->mNumChildren, App->scene->rootNode);
		}

		{
			// Use scene->mNumMeshes to iterate on scene->mMeshes array
			//for (auto i = 0; i < scene->mNumMeshes; i++)
			//{
			//	const aiMesh* m = scene->mMeshes[i];
			//	mesh* ourMesh = new mesh;

			//	// copy vertices
			//	ourMesh->num_vertex = m->mNumVertices;
			//	ourMesh->vertex = new float[ourMesh->num_vertex * 3];
			//	memcpy(ourMesh->vertex, m->mVertices, sizeof(float) * ourMesh->num_vertex * 3);

			//	// TODO preguntar: como pillar el nombre del objeto y no la de dentro de la mesh "outliner id data operation" (abrir en blender pa explicar bien xd) 
			//	LOG("New mesh %s with %d vertices", m->mName.C_Str(), m->mNumVertices);

			//	// copy faces
			//	if (m->HasFaces())
			//	{
			//		ourMesh->num_index = m->mNumFaces * 3;
			//		ourMesh->index = new uint[ourMesh->num_index]; // assume each face is a triangle

			//		for (uint i = 0; i < m->mNumFaces; ++i)
			//		{
			//			if (m->mFaces[i].mNumIndices != 3)
			//			{
			//				LOG("[WARNING], geometry face with != 3 indices!");
			//			}
			//			else
			//			{
			//				memcpy(&ourMesh->index[i * 3], m->mFaces[i].mIndices, 3 * sizeof(uint));
			//			}
			//		}
			//	}

			//	//copy normals
			//	ourMesh->num_normals = m->mNumVertices;
			//	ourMesh->normals = new float[ourMesh->num_normals * 3];
			//	memcpy(ourMesh->normals, m->mNormals, sizeof(float) * ourMesh->num_normals * 3);

			//	////copy texture coordinates
			//	uint uv_index = 0;

			//	if (m->HasTextureCoords(uv_index))
			//	{
			//		ourMesh->tex.num_tex = m->mNumVertices;
			//		ourMesh->tex.tex = new math::float2[ourMesh->tex.num_tex * 3];
			//		for (uint i = 0; i < ourMesh->tex.num_tex; i++)
			//		{
			//			ourMesh->tex.tex[i].x = m->mTextureCoords[uv_index][i].x;
			//			ourMesh->tex.tex[i].y = m->mTextureCoords[uv_index][i].y;
			//		}
			//	}

			//	if (InitMesh(ourMesh))
			//	{
			//		BindTexture(ourMesh);

			//		//(texfileDir != nullptr) ? ourMesh->hasTex = true : ourMesh->hasTex = false;

			//		App->renderer3D->meshes.push_back(ourMesh);
			//	}
			//}
		}

		LOG("%d meshes loaded.", scene->mNumMeshes);
		aiReleaseImport(scene);
	}
	else
	{
		LOG("[ERROR] loading scene % s", meshfileDir);
		return false;
	}
	return true;
}

void ai::MeshHierarchy(const aiScene* s, aiNode** children, int num, GameObject* parent)
{
	for (int i = 0; i < num; i++)
	{
		if (children[i]->mNumMeshes > 0)
		{
			GameObject* obj = new GameObject(s->mMeshes[children[i]->mMeshes[0]]->mName.C_Str(), parent);
			if (children[i]->mChildren != NULL)
			{
				MeshHierarchy(s, children[i]->mChildren, children[i]->mNumChildren, obj);
			}

			const aiMesh* m = s->mMeshes[children[i]->mMeshes[0]];
			mesh* ourMesh = new mesh;

			// copy vertices
			ourMesh->num_vertex = m->mNumVertices;
			ourMesh->vertex = new float[ourMesh->num_vertex * 3];
			memcpy(ourMesh->vertex, m->mVertices, sizeof(float) * ourMesh->num_vertex * 3);

			// TODO preguntar: como pillar el nombre del objeto y no la de dentro de la mesh "outliner id data operation" (abrir en blender pa explicar bien xd) 
			LOG("New mesh %s with %d vertices", m->mName.C_Str(), m->mNumVertices);

			// copy faces
			if (m->HasFaces())
			{
				ourMesh->num_index = m->mNumFaces * 3;
				ourMesh->index = new uint[ourMesh->num_index]; // assume each face is a triangle

				for (uint i = 0; i < m->mNumFaces; ++i)
				{
					if (m->mFaces[i].mNumIndices != 3)
					{
						LOG("[WARNING], geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&ourMesh->index[i * 3], m->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}

			//copy normals
			ourMesh->num_normals = m->mNumVertices;
			ourMesh->normals = new float[ourMesh->num_normals * 3];
			memcpy(ourMesh->normals, m->mNormals, sizeof(float) * ourMesh->num_normals * 3);

			////copy texture coordinates
			uint uv_index = 0;

			if (m->HasTextureCoords(uv_index))
			{
				ourMesh->tex.num_tex = m->mNumVertices;
				ourMesh->tex.tex = new math::float2[ourMesh->tex.num_tex * 3];
				for (uint i = 0; i < ourMesh->tex.num_tex; i++)
				{
					ourMesh->tex.tex[i].x = m->mTextureCoords[uv_index][i].x;
					ourMesh->tex.tex[i].y = m->mTextureCoords[uv_index][i].y;
				}
			}

			if (InitMesh(ourMesh))
			{
				//BindTexture(ourMesh);

				//(texfileDir != nullptr) ? ourMesh->hasTex = true : ourMesh->hasTex = false;
				
				obj->AddComponent(C_TYPE::MESH, ourMesh);

				App->renderer3D->meshes.push_back(ourMesh);
			}
		}
	}
}

bool ai::InitMesh(mesh* m)
{
	m->VBO = 0;
	m->EBO = 0;
	m->VAO = 0;

	glGenBuffers(1, &m->VBO);
	glGenBuffers(1, &m->EBO);
	//glGenVertexArrays(1, &m->VAO);
	//normals
	glGenBuffers(1, &m->id_normals);
	//texture coordinates
	glGenBuffers(1, &m->tex.id_tex);

	if (m->VBO == 0 || m->EBO == 0 || /*m->VAO == 0 ||*/
		m->id_normals == 0 || m->tex.id_tex == 0)
	{
		LOG("[ERROR] buffer not created");
		return false;
	}

	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, m->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->num_vertex * 3, m->vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * m->num_index, m->index, GL_STATIC_DRAW); //TODO: EN TEORIA FUNCIONA YA?
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// VAO
	/*glBindVertexArray(m->VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);*/

	//normals
	glBindBuffer(GL_ARRAY_BUFFER, m->id_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->num_normals * 3, m->normals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, m->tex.id_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->tex.num_tex * 2, m->tex.tex, GL_STATIC_DRAW);

	return true;
}

void ai::CreatePolyPrimitive(POLY_PRIMITIVE_TYPE obj)
{
	switch (obj)
	{
	case ai::POLY_PRIMITIVE_TYPE::CUBE:
		ImportMesh("../Assimp/3dObject/cube.fbx");
		break;
	case ai::POLY_PRIMITIVE_TYPE::SPHERE:
		ImportMesh("../Assimp/3dObject/sphere.fbx");
		break;
	case ai::POLY_PRIMITIVE_TYPE::CYLINDER:
		ImportMesh("../Assimp/3dObject/cylinder.fbx");
		break;
	case ai::POLY_PRIMITIVE_TYPE::PLANE:
		ImportMesh("../Assimp/3dObject/plane.fbx");
		break;
	default:
		break;
	}
}

void ai::DeleteLastMesh()
{
	std::vector<ai::mesh*>* m = &App->renderer3D->meshes;

	if (!m->empty())
	{
		LOG("Deleted last mesh created");

		DeleteMeshBuffers(*(m->end() - 1));

		m->back()->~mesh();
		m->erase(m->end() - 1);
		m->shrink_to_fit();
	}
}

void ai::DeleteSelectedMesh(mesh* m)
{
	std::vector<ai::mesh*>* vMesh = &App->renderer3D->meshes;

	LOG("Deleted mesh");
	DeleteMeshBuffers(m);

	App->renderer3D->meshes.erase(std::find(vMesh->begin(), vMesh->end(), m));
	App->renderer3D->meshes.shrink_to_fit();
}

void ai::DeleteMeshBuffers(mesh* m)
{
	glDeleteBuffers(1, &m->VBO);
	m->VBO = 0;
	glDeleteBuffers(1, &m->EBO);
	m->EBO = 0;
	glDeleteBuffers(1, &m->id_normals);
	m->id_normals = 0;
}

void ai::LoadCheckers(GLuint& buffer)
{
	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &buffer);
	glBindTexture(GL_TEXTURE_2D, buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
}

bool ai::BindTexture(mesh* m)
{
	//texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, m->tex.id_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->tex.num_tex * 2, m->tex.tex, GL_STATIC_DRAW);
	return true;
}

GLuint ai::ImportTexture(const char* texturefileDir)
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
			LOG("%s", iluErrorString(ilGetError()));
		}

		int const width = ilGetInteger(IL_IMAGE_WIDTH);
		int const height = ilGetInteger(IL_IMAGE_HEIGHT);
		int const type = ilGetInteger(IL_IMAGE_TYPE);
		int const format = ilGetInteger(IL_IMAGE_FORMAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, ilGetData());
	}
	else
	{
		LOG("%s", iluErrorString(ilGetError()));
	}

	//ilBindImage(0);
	ilDeleteImages(1, &textureID);

	return textureID;

}
