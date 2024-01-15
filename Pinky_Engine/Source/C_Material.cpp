#pragma once
#include "C_Material.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "GameObject.h"
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

		// Change Texture
		if (ImGui::BeginCombo("##Texture", tex->name.c_str()))
		{
			for (int i = 0; i < App->resource->vTexturesResources.size(); i++)
			{
				const bool is_selected = (App->resource->vTexturesResources[i] == tex);
				if (ImGui::Selectable(App->resource->vTexturesResources[i]->name.c_str(), is_selected))
				{
					gameObject->ChangeComponentResource(tex, App->resource->vTexturesResources[i], *this);
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		ImGuiCustom::HelpMarker("If the current texture has only one instance you will not be able to change it back unless a that texture is imported to scene");

		if (ImGuiCustom::ToggleButton(toogle.c_str(), &checkered))
		{

		}
		ImGui::SetItemTooltip("Use checkered texture");

		if (tex != nullptr)
		{
			if (tex->assetsFile != "")
			{
				ImGui::Text("Texture Width: %d", tex->tex_width);
				ImGui::Text("Texture Height: %d", tex->tex_height);
				ImGui::TextWrapped("Texture Path: %s", tex->assetsFile);
				ImGui::Dummy(ImVec2(0, 10));
			}
			else
			{
				ImGui::Text("Material: NoLambert");
			}

			ImGui::Image((void*)(intptr_t)tex->tex_id, ImVec2(100, 100));
		}

		ImGui::ColorEdit4("Color", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

		if (!isActive) { ImGui::EndDisabled(); }
	}

	if (!exists) { gameObject->RemoveComponent(this); }
}