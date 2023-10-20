#include "Component.h"

#include <gl/GL.h>
#include <gl/GLU.h>

Component::~Component()
{
}

void Component::ShowInInspector()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		if (ImGui::Checkbox(name.c_str(), &active))
		{

		}
	}
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

C_Mesh::C_Mesh(bool start_enabled) : Component(C_TYPE::MESH, start_enabled, "Mesh")
{

}

C_Mesh::~C_Mesh()
{
	m->~mesh();
}

C_Material::C_Material(bool start_enabled) : Component(C_TYPE::MATERIAL, start_enabled, "Material")
{
}

C_Material::~C_Material()
{
}
