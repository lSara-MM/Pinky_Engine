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

C_Transform::C_Transform(bool start_enabled) : Component(C_TYPE::TRANSFORM, active, "Transform")
{
}

C_Transform::~C_Transform()
{
}

C_Mesh::C_Mesh(bool start_enabled) : Component(C_TYPE::MESH, active, "Mesh")
{

}

C_Mesh::~C_Mesh()
{
	m->~mesh();
}

C_Material::C_Material(bool start_enabled) : Component(C_TYPE::MATERIAL, active, "Material")
{
}

C_Material::~C_Material()
{
}
