#pragma once
#include "C_Material.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "GameObject.h"
#include "Component.h"

C_Material::C_Material(GameObject* g, ai::texture* t, bool check, unsigned int i, bool start_enabled) : Component(C_TYPE::MATERIAL, g, i, start_enabled, "Material")
{
	tex = t;
	checkered = check;
	color = { 255, 255, 255, 255 };
}

C_Material::~C_Material()
{
}

void C_Material::ShowInInspector()
{	
	// --- Set ImGui ids ---
	std::string checkbox = name.c_str();
	std::string header = name.c_str();
	std::string toogle = "Checkered##";

	checkbox.insert(checkbox.begin(), 2, '#');
	checkbox.append(std::to_string(GetID()));
	
	header.append("##");
	header.append(std::to_string(GetID()));

	toogle.append(std::to_string(GetID()));

	// ---------------------------------------------

	ImGui::Checkbox(checkbox.c_str(), &active);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!active) { ImGui::BeginDisabled(); }

		if (ImGui::ToggleButton(toogle.c_str(), &checkered))
		{

		}
		ImGui::SetItemTooltip("Use checkered texture");

		ImGui::ColorEdit3("Color", (float*)&color, ImGuiColorEditFlags_NoInputs);

		if (!active) { ImGui::EndDisabled(); }
	}
}