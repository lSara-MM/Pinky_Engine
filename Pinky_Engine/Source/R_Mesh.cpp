#include "R_Mesh.h"
#include "GameObject.h"

R_Mesh::R_Mesh() : Resource(R_TYPE::MESH)
{
	//indices
	num_index = 0;
	index = nullptr;

	//vertices
	num_vertex = 0;
	vertex = nullptr;

	//normals
	id_normals = 0;
	num_normals = 0;
	normals = nullptr;

	//texture UVs
	id_tex_uvs = 0;
	num_tex_uvs = 0;
	tex_uvs = nullptr;
}

R_Mesh::~R_Mesh()
{
	RELEASE_ARRAY(index);
	RELEASE_ARRAY(vertex);
	RELEASE_ARRAY(normals);
	RELEASE_ARRAY(tex_uvs);

	//tex.~texture();
}

bool R_Mesh::InitBuffers()
{
	VBO = 0;
	EBO = 0;
	VAO = 0;

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//glGenVertexArrays(1, &VAO);
	
	//normals
	glGenBuffers(1, &id_normals);

	//texture coordinates
	glGenBuffers(1, &id_tex_uvs);

	if (VBO == 0 || EBO == 0 || /*VAO == 0 ||*/
		id_normals == 0 || id_tex_uvs == 0)
	{
		LOG("[ERROR] buffer not created");
		return false;
	}

	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertex * 3, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * num_index, index, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// VAO
	/*glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);*/

	//normals
	glBindBuffer(GL_ARRAY_BUFFER, id_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_normals * 3, normals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//texture coordinates
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_tex_uvs);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * num_tex_uvs * 2, tex_uvs, GL_STATIC_DRAW);

	return true;
}

void R_Mesh::DeleteBuffers()
{
	glDeleteBuffers(1, &VBO);
	VBO = 0;
	glDeleteBuffers(1, &EBO);
	EBO = 0;
	glDeleteBuffers(1, &id_normals);
	id_normals = 0;
	glDeleteBuffers(1, &id_tex_uvs);
	id_tex_uvs = 0;
}