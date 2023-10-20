#include "Component.h"

#include <gl/GL.h>
#include <gl/GLU.h>

Component::~Component()
{
}

C_Transform::C_Transform(bool start_enabled) : Component(C_TYPE::TRANSFORM, start_enabled, "Transform")
{
}

C_Transform::C_Transform(float3 pos, Quat rot, float3 sc, bool start_enabled) : Component(C_TYPE::TRANSFORM, start_enabled, "Transform")
{
	position = pos;
	rotation = rot;
	scale = sc;
}

C_Transform::~C_Transform()
{
}

void C_Transform::ShowInInspector()
{
	if (ImGui::CollapsingHeader(name.c_str()))
	{
		static float vec[3] = { position.x, position.y, position .z};
		ImGui::DragFloat3("Position", vec);
		SetTransform(vec);

		static float vec1[3] = { rotation.x, rotation.y, rotation.z};
		ImGui::DragFloat3("Rotation", vec1);
		SetRotation(vec1);

		static float vec2[3] = { scale.x, scale.y, scale.z};
		ImGui::DragFloat3("Scale", vec2);
		SetScale(vec2);

		ImGui::Text("This is a drag and drop source");
		ImGui::Text("This is a drag and drop source");
		ImGui::Text("This is a drag and drop source");
	}
}

void C_Transform::SetTransform(float vec[3])
{
	for (int i = 0; i < 3; i++)
	{
		position[i] = vec[i];
	}
}

void C_Transform::SetRotation(float vec[])
{
	rotation.x = vec[0];
	rotation.y = vec[1];
	rotation.z = vec[2];
}

void C_Transform::SetScale(float vec[3])
{
	for (int i = 0; i < 3; i++)
	{
		scale[i] = vec[i];
	}
}


C_Mesh::C_Mesh(bool start_enabled) : Component(C_TYPE::MESH, start_enabled, "Mesh")
{

}

C_Mesh::~C_Mesh()
{
	m->~mesh();
}

void C_Mesh::ShowInInspector()
{
	ImGui::Checkbox(" ", &active);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader(name.c_str()))
	{
		if (!active) { ImGui::BeginDisabled(); }


		ImGui::Text("This is a drag and drop source");
		ImGui::Text("This is a drag and drop source");
		ImGui::Text("This is a drag and drop source");

		if (!active) { ImGui::EndDisabled(); }
	}
}


C_Material::C_Material(bool start_enabled) : Component(C_TYPE::MATERIAL, start_enabled, "Material")
{
}

C_Material::~C_Material()
{
}

void C_Material::ShowInInspector()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		if (ImGui::Checkbox(name.c_str(), &active))
		{

		}
	}
}