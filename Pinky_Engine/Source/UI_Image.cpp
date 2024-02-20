#include "UI_Image.h"
#include "ModuleResource.h"

#include "GameObject.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"
#include "External Libraries/ImGui/misc/cpp/imgui_stdlib.h"
//
#include "External Libraries/ImGui/imgui_custom.h"

UI_Image::UI_Image(GameObject* g, int x, int y, int w, int h) : C_UI(UI_TYPE::IMAGE, C_TYPE::UI, g, "Image", x, y, w, h)
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
	bool exists = true;

	checkbox.insert(checkbox.begin(), 2, '#');
	checkbox.append(std::to_string(GetUID()));

	header.append("##").append(std::to_string(GetUID()));
	// ---------------------------------------------

	ImGui::Checkbox(checkbox.c_str(), &isActive);		ImGui::SameLine();

	if (ImGui::CollapsingHeader(header.c_str(), &exists, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!isActive) { ImGui::BeginDisabled(); }

		ImGui::Checkbox("Draggeable", &isDraggable);
		if (mat->tex != nullptr)
		{
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
		}

		if (ImGuiCustom::ToggleButton("Checkered##", &mat->checkered))
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
				ImGui::Dummy(ImVec2(0, 10));
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

	// TODO: to test
	if (!exists) 
	{
		gameObject->ChangeComponentResource(mat->tex, nullptr, *this);
	}
}

void UI_Image::Draw(bool game)
{
	if (mat != nullptr && mat->tex != nullptr)
	{
		UIBounds* boundsDrawn = nullptr;

		if (game)
		{
			boundsDrawn = boundsGame;

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0.0, App->editor->gameViewSize.x, 0.0, App->editor->gameViewSize.y, 1.0, -1.0);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}

		else
		{
			boundsDrawn = boundsEditor;

			glPushMatrix();
			glMultMatrixf(gameObject->transform->GetGLTransform());
		}

		glEnableClientState(GL_VERTEX_ARRAY);

		//normals
		glEnableClientState(GL_NORMAL_ARRAY);

		//texture
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// Mesh buffers
		glBindBuffer(GL_ARRAY_BUFFER, boundsDrawn->VBO);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glBindVertexArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boundsDrawn->EBO);

		//alpha material
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);

		glColor4f(color.r, color.g, color.b, color.a);

		// Textures
		glBindBuffer(GL_ARRAY_BUFFER, boundsDrawn->id_tex_uvs);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		glActiveTexture(GL_TEXTURE0);

		(!mat->checkered) ? glBindTexture(GL_TEXTURE_2D, (mat->tex->tex_id))
			: glBindTexture(GL_TEXTURE_2D, App->renderer3D->texture_checker);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		// Clean textures
		glBindTexture(GL_TEXTURE_2D, 0); // Cleanning bind buffer;
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		if (!game)
		{
			glPopMatrix();
		}

		boundsDrawn = nullptr;
	}
}

update_status UI_Image::Update(float dt)
{
	return update_status();
}
