#pragma once
#include "C_Material.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "GameObject.h"
#include "Component.h"
#include "External Libraries/ImGui/misc/cpp/imgui_stdlib.h"
//
#include "External Libraries/ImGui/imgui_custom.h"

C_Material::C_Material(GameObject* g, R_Texture* t, bool start_enabled) : Component(C_TYPE::MATERIAL, g, start_enabled, "Material")
{
	(t == nullptr) ? tex = new R_Texture() : tex = t;

	checkered = false;
	color = { 255, 255, 255, 255 };
}

C_Material::C_Material(GameObject* g, C_Material* toCopy, bool start_enabled) : Component(C_TYPE::MATERIAL, g, toCopy->isActive, "Material")
{
	//memcpy(&tex, &toCopy->tex, sizeof(*tex));
	tex = toCopy->tex;
	checkered = true;
	color = { 255, 255, 255, 255 };
}

C_Material::~C_Material()
{
	tex->CleanUp();
	tex = nullptr;
}

void C_Material::ShowInInspector()
{	
	// --- Set ImGui ids ---
	std::string checkbox = name.c_str();
	std::string header = name.c_str();
	std::string toogle = "Checkered##";
	bool exists = true;

	checkbox.insert(checkbox.begin(), 2, '#');
	checkbox.append(std::to_string(GetUID()));
	
	header.append("##");
	header.append(std::to_string(GetUID()));

	toogle.append(std::to_string(GetUID()));

	// ---------------------------------------------

	ImGui::Checkbox(checkbox.c_str(), &isActive);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader(header.c_str(), &exists, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!isActive) { ImGui::BeginDisabled(); }

		if (ImGuiCustom::ToggleButton(toogle.c_str(), &checkered))
		{

		}
		ImGui::SetItemTooltip("Use checkered texture");

		if (tex != nullptr)
		{
			if (tex->path != "")
			{
				ImGui::Text("Texture Width: %d", tex->tex_width);
				ImGui::Text("Texture Height: %d", tex->tex_height);
				ImGui::TextWrapped("Texture Path: %s", tex->assetsFile);
			}
			else
			{
				ImGui::Text("Material: NoLambert");
			}

			ImGui::Image((void*)(intptr_t)tex->tex_id, ImVec2(100, 100));
		}

		ImGui::ColorEdit3("Color", (float*)&color, ImGuiColorEditFlags_NoInputs);

		if (!isActive) { ImGui::EndDisabled(); }
	}

	if (!exists) { gameObject->RemoveComponent(this); }
}