#pragma once
#include "C_Mesh.h"

#include "../SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../MathGeoLib/include/MathGeoLib.h"

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"
#include "../DevIL/include/il.h"
#include "../DevIL/include/ilu.h"
#include "../DevIL/include/ilut.h"
#include "../mmgr/mmgr.h"


#include "GameObject.h"
#include "Component.h"
#include "ModuleRenderer3D.h"	// TODO: no se donde tendria que estar el gluint del checkers pa que no se repita por cada mesh

C_Mesh::C_Mesh(GameObject* g, ai::mesh* m, unsigned int i, bool start_enabled) : Component(C_TYPE::MESH, g, i, start_enabled, "Mesh")
{
	mesh = m;
}

C_Mesh::~C_Mesh()
{
	//mesh->~mesh();
	ai::DeleteSelectedMesh(mesh);
}

void C_Mesh::ShowInInspector()
{
	// Set ImGui ids
	std::string checkbox = name.c_str();
	std::string header = name.c_str();

	checkbox.insert(checkbox.begin(), 2, '#');
	checkbox.append(std::to_string(GetID()));

	header.append("##");
	header.append(std::to_string(GetID()));


	ImGui::Checkbox(checkbox.c_str(), &active);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!active) { ImGui::BeginDisabled(); }

		// TODO: quitar cuando ponga que crear un mesh cree un cubo o una mesh 
		if (mesh != nullptr)
		{
			ImGui::Text("Vertices: %d", mesh->num_vertex);
		}

		ImGui::Text("This is a drag and drop source");

		if (!active) { ImGui::EndDisabled(); }
	}
}

void C_Mesh::Draw(bool checkered)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	//normals
	glEnableClientState(GL_NORMAL_ARRAY);

	//texture
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// Mesh buffers
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);


	// Normales
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
	glNormalPointer(GL_FLOAT, 0, NULL);

	// Textures
	glBindBuffer(GL_ARRAY_BUFFER, mesh->tex.id_tex);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, mesh->tex.id_tex);
	(!checkered) ? glBindTexture(GL_TEXTURE_2D, mesh->tex.id_tex) : glBindTexture(GL_TEXTURE_2D, App->renderer3D->texture_checker);

	// Draw mesh
	glDrawElements(GL_TRIANGLES, mesh->num_index, GL_UNSIGNED_INT, NULL);

	// Clean textures
	glBindTexture(GL_TEXTURE_2D, 0); // Cleanning bind buffer;
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void C_Mesh::DrawVertexNormals()
{
	glBegin(GL_LINES);
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	for (uint i = 0; i < mesh->num_vertex * 3; i += 3)
	{
		LineSegment NormalDirection(math::float3(mesh->vertex[i], mesh->vertex[i + 1], mesh->vertex[i + 2]),
			math::float3(mesh->vertex[i] + mesh->normals[i], mesh->vertex[i + 1] + mesh->normals[i + 1], mesh->vertex[i + 2] + mesh->normals[i + 2]));

		//NormalDirection.Transform());

		glVertex3f(NormalDirection.a.x, NormalDirection.a.y, NormalDirection.a.z);
		glVertex3f(NormalDirection.b.x, NormalDirection.b.y, NormalDirection.b.z);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();
}
