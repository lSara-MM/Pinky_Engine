#include "UI_Image.h"
#include "ModuleResource.h"

#include "GameObject.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"
#include "External Libraries/ImGui/misc/cpp/imgui_stdlib.h"
//
#include "External Libraries/ImGui/imgui_custom.h"

UI_Image::UI_Image(GameObject* g) : C_UI(UI_TYPE::IMAGE, C_TYPE::UI, g, "Image")
{
	mat = new C_Material();
}

UI_Image::~UI_Image()
{
	RELEASE(mat);
}

void UI_Image::ShowInInspector()
{
	// --- Set ImGui ids ---
	std::string checkbox = name.c_str();
	std::string header = name.c_str();
	std::string toogle = "Checkered##";
	bool exists = true;

	checkbox.insert(checkbox.begin(), 2, '#');
	checkbox.append(std::to_string(GetUID()));

	header.append("##").append(std::to_string(GetUID()));
	// ---------------------------------------------

	ImGui::Checkbox(checkbox.c_str(), &isActive);		ImGui::SameLine();

	if (ImGui::CollapsingHeader(header.c_str(), &exists, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!isActive) { ImGui::BeginDisabled(); }

		// Change Texture
		if (ImGui::BeginCombo("##Texture", mat->tex->name.c_str()))
		{
			for (int i = 0; i < App->resource->vTexturesResources.size(); i++)
			{
				const bool is_selected = (App->resource->vTexturesResources[i] == mat->tex);
				if (ImGui::Selectable(App->resource->vTexturesResources[i]->name.c_str(), is_selected))
				{
					gameObject->ChangeComponentResource(mat->tex, App->resource->vTexturesResources[i], *mat);
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

		if (ImGuiCustom::ToggleButton(toogle.c_str(), &mat->checkered))
		{

		}
		ImGui::SetItemTooltip("Use checkered texture");

		if (mat->tex != nullptr)
		{
			if (mat->tex->assetsFile != "")
			{
				ImGui::Text("Texture Width: %d", mat->tex->tex_width);
				ImGui::Text("Texture Height: %d", mat->tex->tex_height);
				ImGui::TextWrapped("Texture Path: %s", mat->tex->assetsFile);
			}
			else
			{
				ImGui::Text("Material: NoLambert");
			}

			ImGui::Image((void*)(intptr_t)mat->tex->tex_id, ImVec2(100, 100));
		}

		ImGui::ColorEdit4("Color", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

		if (!isActive) { ImGui::EndDisabled(); }
	}
	ImGui::SameLine();

	if (!exists) { gameObject->RemoveComponent(this); }
}

void UI_Image::DrawEditor()
{
}

void UI_Image::DrawGame()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(0.0, App->editor->GameViewSize.x, App->editor->GameViewSize.y, 0.0, 1.0, -1.0);//TODO: orginal con 0,0 bien en pantalla pero mueve al revés
	glOrtho(App->editor->GameViewSize.x, 0.0, 0.0, App->editor->GameViewSize.y, 1.0, -1.0);

	//Initialize Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	//
	glColor4f(color.r, color.g, color.b, color.a);

	//
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D, bounds->textureID);

	glBindBuffer(GL_ARRAY_BUFFER, bounds->VBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, bounds->id_tex_uvs);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glActiveTexture(GL_TEXTURE0);
	if (static_cast<C_Material*>(gameObject->GetComponentByType(C_TYPE::MATERIAL)) != nullptr &&
		static_cast<C_Material*>(gameObject->GetComponentByType(C_TYPE::MATERIAL))->tex != nullptr)
	{
		(!mat->checkered) ? glBindTexture(GL_TEXTURE_2D, static_cast<C_Material*>(gameObject->GetComponentByType(C_TYPE::MATERIAL))->tex->tex_id)
			: glBindTexture(GL_TEXTURE_2D, App->renderer3D->texture_checker);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bounds->EBO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);
}
