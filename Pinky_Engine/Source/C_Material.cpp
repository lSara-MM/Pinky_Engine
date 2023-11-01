#pragma once
#include "C_Material.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "GameObject.h"
#include "Component.h"
#include "../ImGui/misc/cpp/imgui_stdlib.h"
//
#include "../ImGui/imgui_custom.h"

C_Material::C_Material(GameObject* g, ai::texture* t, bool check, unsigned int i, bool start_enabled) : Component(C_TYPE::MATERIAL, g, i, start_enabled, "Material")
{
	tex = t;
	//(t->path != "") ? tex = t : tex = nullptr;
	//(t == nullptr || t->path == "") ? tex = nullptr : tex = t;

	checkered = check;
	color = { 255, 255, 255, 255 };
}

C_Material::C_Material(GameObject* g, C_Material* toCopy, unsigned int i, bool start_enabled) : Component(C_TYPE::MATERIAL, g, i, toCopy->active, "Material")
{
	//memcpy(&tex, &toCopy->tex, sizeof(*tex));
	tex = toCopy->tex;
	checkered = true;
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
	bool exists = true;

	checkbox.insert(checkbox.begin(), 2, '#');
	checkbox.append(std::to_string(GetID()));
	
	header.append("##");
	header.append(std::to_string(GetID()));

	toogle.append(std::to_string(GetID()));

	// ---------------------------------------------

	ImGui::Checkbox(checkbox.c_str(), &active);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader(header.c_str(), &exists, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!active) { ImGui::BeginDisabled(); }

		if (ImGuiCustom::ToggleButton(toogle.c_str(), &checkered))
		{

		}
		ImGui::SetItemTooltip("Use checkered texture");

		if (tex != nullptr && tex->path != "")
		{
			ImGui::Text("Texture Width: %d", tex->tex_width);
			ImGui::Text("Texture Height: %d", tex->tex_height);
			ImGui::TextWrapped("Texture Path: %s", tex->path);
			ImGui::Image((void*)(intptr_t)tex->tex_id, ImVec2(100, 100));
		}

		ImGui::ColorEdit3("Color", (float*)&color, ImGuiColorEditFlags_NoInputs);

		if (!active) { ImGui::EndDisabled(); }
	}

	if (!exists) { gameObject->RemoveComponent(this); }
}