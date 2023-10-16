#include "Component.h"

#include <gl/GL.h>
#include <gl/GLU.h>

void Component::ShowInInspector()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		if (ImGui::Checkbox(name.c_str(), &active))
		{

		}

	}
}
