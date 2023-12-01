#pragma once
#include "C_Mesh.h"

#include "External Libraries\SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "External Libraries/MathGeoLib/include/MathGeoLib.h"

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Source/External Libraries/Glew/libx86/glew32.lib")

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"
#include "External Libraries/DevIL/include/il.h"
#include "External Libraries/DevIL/include/ilu.h"
#include "External Libraries/DevIL/include/ilut.h"
#include "External Libraries/mmgr/mmgr.h"


#include "GameObject.h"
#include "Component.h"
#include "ModuleRenderer3D.h"	

C_Mesh::C_Mesh(GameObject* g, R_Mesh* m, uint i, bool start_enabled) : Component(C_TYPE::MESH, g, i, start_enabled, "Mesh")
{
	mesh = m; 
	showVertexNormals = false;
	showFacesNormals = false;
	showAABB = false;
	showOBB = false;

	//TODO: does it go here?
	obb = mesh->local_aabb;
	obb.Transform(gameObject->transform->GetGlobalTransform());
	global_aabb.SetNegativeInfinity();
	global_aabb.Enclose(obb);
}

C_Mesh::C_Mesh(GameObject* g, C_Mesh* toCopy, bool start_enabled, uint i) : Component(C_TYPE::MESH, g, i, toCopy->isActive, "Mesh")
{
	memcpy(&mesh, &toCopy->mesh, sizeof(*mesh));
	//mesh = toCopy->mesh;
	showVertexNormals = toCopy->showVertexNormals;
	showFacesNormals = toCopy->showFacesNormals;
}

C_Mesh::~C_Mesh()
{
	//ai::DeleteSelectedMesh(mesh);
	//mesh->~mesh();

	RELEASE(mesh);
	/*mesh->~R_Mesh();
	mesh = nullptr;*/
}

void C_Mesh::ShowInInspector()
{
	// --- Set ImGui ids ---
	std::string checkbox = name.c_str();
	std::string header = name.c_str();
	bool exists = true;

	checkbox.insert(checkbox.begin(), 2, '#');
	checkbox.append(std::to_string(GetUID()));

	header.append("##").append(std::to_string(GetUID()));
	// ---------------------------------------------

	ImGui::Checkbox(checkbox.c_str(), &isActive);		ImGui::SameLine();

	if (ImGui::CollapsingHeader(header.c_str(), &exists, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!isActive) { ImGui::BeginDisabled(); }
 
		if (mesh != nullptr)
		{
			ImGui::Text("Vertices: %d", mesh->num_vertex);
		}

		ImGui::Checkbox("Vertex Normals", &showVertexNormals);
		ImGui::Checkbox("Faces Normals", &showFacesNormals);
		ImGui::Checkbox("Show AABB", &showAABB);
		ImGui::Checkbox("Show OBB", &showOBB);
			
		ImGui::SameLine();

		if (!isActive) { ImGui::EndDisabled(); }
	}
	ImGui::SameLine();

	if (!exists) { gameObject->RemoveComponent(this); }
}

void C_Mesh::Draw(bool checkered, Color color)
{
	glPushMatrix();
	glMultMatrixf(gameObject->transform->GetGLTransform());

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

	// ---------------------------------------------


	glColor4f(color.r, color.g, color.b, color.a);

	// Textures
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_tex_uvs);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glActiveTexture(GL_TEXTURE0);

	//glBindTexture(GL_TEXTURE_2D, mesh->tex.tex_id);
	(!checkered) ? glBindTexture(GL_TEXTURE_2D, static_cast<C_Material*>(gameObject->GetComponentByType(C_TYPE::MATERIAL))->tex->tex_id) : glBindTexture(GL_TEXTURE_2D, App->renderer3D->texture_checker);

	// ---------------------------------------------

	// Draw mesh
	glDrawElements(GL_TRIANGLES, mesh->num_index, GL_UNSIGNED_INT, NULL);

	// Clean textures
	glBindTexture(GL_TEXTURE_2D, 0); // Cleanning bind buffer;
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

void C_Mesh::DrawVertexNormals()
{
	glPushMatrix();
	glMultMatrixf(gameObject->transform->globalMatrix.Transposed().ptr());
	float normal_lenght = 1.0f;
	glBegin(GL_LINES);
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	for (uint i = 0; i < mesh->num_vertex * 3; i += 3)
	{
		LineSegment NormalDirection(math::float3(mesh->vertex[i], mesh->vertex[i + 1], mesh->vertex[i + 2]),
			math::float3(mesh->vertex[i] + mesh->normals[i], mesh->vertex[i + 1] + mesh->normals[i + 1], mesh->vertex[i + 2] + mesh->normals[i + 2]));

		glVertex3f(NormalDirection.a.x * normal_lenght, NormalDirection.a.y * normal_lenght, NormalDirection.a.z * normal_lenght);
		glVertex3f(NormalDirection.b.x * normal_lenght, NormalDirection.b.y * normal_lenght, NormalDirection.b.z * normal_lenght);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();
	glPopMatrix();
}

void C_Mesh::DrawFaceNormals()
{
	glPushMatrix();
	glMultMatrixf(gameObject->transform->globalMatrix.Transposed().ptr());

	float normal_lenght = 1.0f;
	glBegin(GL_LINES);
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	for (size_t i = 0; i < mesh->num_index; i += 3)
	{
		uint index1 = mesh->index[i] * 3;
		uint index2 = mesh->index[i + 1] * 3;
		uint index3 = mesh->index[i + 2] * 3;

		math::float3 vec1(mesh->vertex[index1], mesh->vertex[index1 + 1], mesh->vertex[index1 + 2]);
		math::float3 vec2(mesh->vertex[index2], mesh->vertex[index2 + 1], mesh->vertex[index2 + 2]);
		math::float3 vec3(mesh->vertex[index3], mesh->vertex[index3 + 1], mesh->vertex[index3 + 2]);

		math::float3 v0 = vec2 - vec1;
		math::float3 v1 = vec3 - vec1;
		math::float3 crossV = math::Cross(v0, v1);

		math::float3 crossNorm = crossV.Normalized();

		GLfloat vx = (vec1.x + vec2.x + vec3.x) / 3;
		GLfloat vy = (vec1.y + vec2.y + vec3.y) / 3;
		GLfloat vz = (vec1.z + vec2.z + vec3.z) / 3;

		GLfloat vx_norm = crossNorm.x;
		GLfloat vy_norm = crossNorm.y;
		GLfloat vz_norm = crossNorm.z;

		glVertex3f(vx, vy, vz);
		glVertex3f(vx + (vx_norm * normal_lenght), vy + (vy_norm * normal_lenght), vz + (vz_norm * normal_lenght));
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();
	glPopMatrix();
}

void C_Mesh::DrawAABB()
{
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(global_aabb.Edge(i).a.x, global_aabb.Edge(i).a.y, global_aabb.Edge(i).a.z);
		glVertex3f(global_aabb.Edge(i).b.x, global_aabb.Edge(i).b.y, global_aabb.Edge(i).b.z);
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void C_Mesh::DrawOBB()
{
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(obb.Edge(i).a.x, obb.Edge(i).a.y, obb.Edge(i).a.z);
		glVertex3f(obb.Edge(i).b.x, obb.Edge(i).b.y, obb.Edge(i).b.z);
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}
