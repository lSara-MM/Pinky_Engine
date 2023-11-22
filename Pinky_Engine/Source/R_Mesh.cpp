#include "R_Mesh.h"

R_Mesh::R_Mesh()
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

bool R_Mesh::InitBuffers(R_Mesh* m)
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
	glGenBuffers(1, &m->id_tex_uvs);

	if (m->VBO == 0 || m->EBO == 0 || /*m->VAO == 0 ||*/
		m->id_normals == 0 || m->id_tex_uvs == 0)
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * m->num_index, m->index, GL_STATIC_DRAW);
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
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->id_tex_uvs);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * m->num_tex_uvs * 2, m->tex_uvs, GL_STATIC_DRAW);

	return true;
}

void R_Mesh::DeleteBuffers(R_Mesh* m)
{
	glDeleteBuffers(1, &m->VBO);
	m->VBO = 0;
	glDeleteBuffers(1, &m->EBO);
	m->EBO = 0;
	glDeleteBuffers(1, &m->id_normals);
	m->id_normals = 0;
	glDeleteBuffers(1, &m->id_tex_uvs);
	m->id_tex_uvs = 0;
}
