#pragma once
#include "C_Mesh.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "GameObject.h"
#include "Component.h"

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