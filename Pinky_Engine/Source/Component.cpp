#pragma once
#include "Component.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "GameObject.h"

int Component::GetID()
{
	return id;
}

//C_Transform::C_Transform(GameObject* g, bool start_enabled) : Component(C_TYPE::TRANSFORM, g, g->id, start_enabled, "Transform")
//{
//}
//
//C_Transform::C_Transform(GameObject* g, float3 pos, Quat rot, float3 sc, bool start_enabled) : Component(C_TYPE::TRANSFORM, g, g->id, start_enabled, "Transform")
//{
//	position = pos;
//	rotation = rot;
//	scale = sc;
//}
//
//C_Transform::~C_Transform()
//{
//}
//
//void C_Transform::ShowInInspector()
//{
//	std::string n = name.c_str();
//	n.append("##");
//	n.append(std::to_string(gameObject->id));
//
//	if (ImGui::CollapsingHeader(n.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
//	{
//		ImGui::Text("x");
//		ImGui::SameLine();
//		ImGui::Text("x");
//		ImGui::SameLine();
//		ImGui::Text("x");
//
//		float vec[3] = { position.x, position.y, position .z};
//		ImGui::DragFloat3("Position", vec);
//		SetTransform(vec);
//
//		float vec1[3] = { rotation.x, rotation.y, rotation.z};
//		ImGui::DragFloat3("Rotation", vec1);
//		SetRotation(vec1);
//
//		float vec2[3] = { scale.x, scale.y, scale.z};
//		ImGui::DragFloat3("Scale", vec2);
//		SetScale(vec2);
//	}
//}
//
//void C_Transform::SetTransform(float vec[3])
//{
//	for (int i = 0; i < 3; i++)
//	{
//		position[i] = vec[i];
//	}
//}
//
//void C_Transform::SetRotation(float vec[])
//{
//	rotation.x = vec[0];
//	rotation.y = vec[1];
//	rotation.z = vec[2];
//}
//
//void C_Transform::SetScale(float vec[3])
//{
//	for (int i = 0; i < 3; i++)
//	{
//		scale[i] = vec[i];
//	}
//}
//
//
//C_Mesh::C_Mesh(GameObject* g, ai::mesh* m, unsigned int i, bool start_enabled) : Component(C_TYPE::MESH, g, i, start_enabled, "Mesh")
//{
//	mesh = m;
//}
//
//C_Mesh::~C_Mesh()
//{
//	//mesh->~mesh();
//	ai::DeleteSelectedMesh(mesh);
//}
//
//void C_Mesh::ShowInInspector()
//{
//	// Set ImGui ids
//	std::string checkbox = name.c_str();
//	std::string header = name.c_str();
//
//	checkbox.insert(checkbox.begin(), 2, '#');
//	checkbox.append(std::to_string(GetID()));
//
//	header.append("##");
//	header.append(std::to_string(GetID()));
//
//
//	ImGui::Checkbox(checkbox.c_str(), &active);
//	ImGui::SameLine();
//
//	if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
//	{
//		if (!active) { ImGui::BeginDisabled(); }
//
//		// TODO: quitar cuando ponga que crear un mesh cree un cubo o una mesh 
//		if (mesh != nullptr)
//		{
//			ImGui::Text("Vertices: %d", mesh->num_vertex);
//		}
//
//		ImGui::Text("This is a drag and drop source");
//
//		if (!active) { ImGui::EndDisabled(); }
//	}
//}
//
//
//C_Material::C_Material(GameObject* g, unsigned int i, bool start_enabled) : Component(C_TYPE::MATERIAL, g, i, start_enabled, "Material")
//{
//}
//
//C_Material::~C_Material()
//{
//}
//
//void C_Material::ShowInInspector()
//{	
//	// Set ImGui ids
//	std::string checkbox = name.c_str();
//	std::string header = name.c_str();
//
//	checkbox.insert(checkbox.begin(), 2, '#');
//	checkbox.append(std::to_string(GetID()));
//	
//	header.append("##");
//	header.append(std::to_string(GetID()));
//
//
//	ImGui::Checkbox(checkbox.c_str(), &active);
//	ImGui::SameLine();
//
//	if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
//	{
//		if (!active) { ImGui::BeginDisabled(); }
//
//
//		ImGui::Text("This is a drag and drop source");
//		ImGui::Text("This is a drag and drop source");
//		ImGui::Text("This is a drag and drop source");
//
//		if (!active) { ImGui::EndDisabled(); }
//	}
//}