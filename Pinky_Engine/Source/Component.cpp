#include "Component.h"

#include <gl/GL.h>
#include <gl/GLU.h>

Component::Component(bool start_enabled)
{
}

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

C_Transform::C_Transform(bool start_enabled) : Component (active)
{
}

C_Transform::~C_Transform()
{
}

C_Mesh::C_Mesh(bool start_enabled) : Component(active)
{
}

C_Mesh::~C_Mesh()
{
}

C_Material::C_Material(bool start_enabled) : Component(active)
{
}

C_Material::~C_Material()
{
}
