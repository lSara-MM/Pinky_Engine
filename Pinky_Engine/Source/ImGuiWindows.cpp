#include "ImGuiWindows.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "GameObject.h"


#include "../SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"
#include "../ImGui/misc/cpp/imgui_stdlib.h"

#ifdef _DEBUG
#pragma comment (lib, "MathGeoLib/libx86/libDebug/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#else
#pragma comment (lib, "MathGeoLib/libx86/libRelease/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#endif // _DEBUG

ImGuiWindows::~ImGuiWindows()
{
	selectedGO = nullptr;
}

GameObject* ImGuiWindows::GetSelected()
{
	return selectedGO;
}

void ImGuiWindows::SetSelected(GameObject* go)
{
	selectedGO = go;
}

Hierarchy::Hierarchy()
{
}

Hierarchy::~Hierarchy()
{
}

void Hierarchy::ShowWindow()
{
	// Always center this window when appearing
	/*ImVec2 center = ImGui::GetMainViewport()->Pos;
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.7f));*/
	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);
	if (ImGui::Begin("Hierarchy"), &show)
	{
		if (!App->scene->rootNode->vChildren.empty())
		{
			ShowChildren(App->scene->rootNode->vChildren, App->scene->rootNode->vChildren.size());
			if (node_clicked != -1)
			{
				// Update selection state
				// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
				if (ImGui::GetIO().KeyCtrl)
					selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
				else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
					selection_mask = (1 << node_clicked);           // Click to single-select
			}
		}
		ImGui::End();
	}
}

bool Hierarchy::ShowChildren(std::vector<GameObject*> current, int num)
{
	bool ret = true;
	ImGuiTreeNodeFlags node_flags;
	
	for (int i = 0; i < num; i++)
	{
		std::string a = current[i]->name.c_str();
		/*a.append(" - ");
		a.append(std::to_string(current[i]->id));	*/	

		if (!current[i]->vChildren.empty())
		{
			node_flags = ImGuiTreeNodeFlags_OpenOnArrow |	ImGuiTreeNodeFlags_OpenOnDoubleClick
				| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

			const bool is_selected = (selection_mask & (1 << current[i]->id)) != 0;
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			bool open = ImGui::TreeNodeEx((void*)(intptr_t)current[i]->id, node_flags, a.c_str());

			if (ImGui::IsItemClicked())
			{
				node_clicked = current[i]->id;
				SetSelected(current[i]);
			}

			if (open)
			{
				ShowChildren(current[i]->vChildren, current[i]->vChildren.size());
				ImGui::TreePop();
			}
		}
		else
		{
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			
			const bool is_selected = (selection_mask & (1 << current[i]->id)) != 0;

			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			if (ImGui::TreeNodeEx((void*)(intptr_t)current[i]->id, node_flags, a.c_str()))
			{
				
			}

			if (ImGui::IsItemClicked())
			{
				node_clicked = current[i]->id;
				SetSelected(current[i]);
			}
		}
	}

	return ret;
}

//

Inspector::Inspector()
{
}

Inspector::~Inspector()
{
}

void Inspector::ShowWindow()
{
	if (ImGui::Begin("Inspector"), &show)
	{
		if (GetSelected() != nullptr)
		{
			// --- Set ImGui ids ---
			std::string name = GetSelected()->name;
			name.insert(name.begin(), 2, '#');
			name.append(std::to_string(GetSelected()->id));

			 ImGui::Checkbox(name.c_str(), &GetSelected()->active);		ImGui::SameLine();
			
			if (!GetSelected()->active) { ImGui::BeginDisabled(); }
			
			ImGui::InputText(name.c_str(), &GetSelected()->name);
			ImGui::Dummy(ImVec2(0, 10));

			for (auto i = 0; i < GetSelected()->vComponents.size(); i++)
			{
				GetSelected()->vComponents[i]->ShowInInspector();
				ImGui::Dummy(ImVec2(0, 10));
			}

			// --- Add components button ---
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 10));
			std::array<std::string, 3> components = { "Transform", "Mesh", "Material" };
			
			if (ImGui::Button("Add Component", ImVec2(110, 30)))
			{
				ImGui::OpenPopup("AddComponents");
				ImGui::SameLine();
				ImGui::TextUnformatted(selected_fish == -1 ? "<None>" : components[selected_fish].c_str());
			}

			if (ImGui::BeginPopup("AddComponents"))
			{
				ImGui::SeparatorText("Components");

				// Skip transform
				// --- Add component Mesh ---
				 
				if (GetSelected()->numMeshes == 0)
				{
					if (ImGui::BeginMenu("Mesh"))
					{
						std::array<std::string, 4> components = { "Cube", "Sphere", "Cylinder", "Plane" };

						for (int i = 0; i < components.size(); i++)
						{
							if (ImGui::MenuItem(components[i].c_str()))
							{
								GetSelected()->AddComponent(C_TYPE::MESH, nullptr, ai::POLY_PRIMITIVE_TYPE(i));
							}
						}
						ImGui::EndMenu();
					}
				}

				// --- Add component Material ---

				if (GetSelected()->numMaterials == 0)
				{
					if (ImGui::MenuItem("Material"))
					{
						GetSelected()->AddComponent(C_TYPE::MATERIAL);
					}
				}
				ImGui::EndPopup();
			}

			if (!GetSelected()->active) { ImGui::EndDisabled(); }
		}
		
		ImGui::End();
	}
}