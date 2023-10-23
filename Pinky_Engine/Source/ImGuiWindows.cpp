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

ImGuiWindows::ImGuiWindows(int i)
{
	id = i;
	selectedGO = nullptr;
	selectedGOs = {};
}

ImGuiWindows::~ImGuiWindows()
{
	selectedGO = nullptr;
	show = true;
}

GameObject* ImGuiWindows::GetSelected()
{
	return selectedGO;
}

void ImGuiWindows::SetSelected(GameObject* go)
{
	selectedGO = go;
	selectedGOs.push_back(go);
	go->selected = !go->selected;
	
	for (auto i = 0; i < go->vChildren.size(); i++)
	{
		go->vChildren[i]->selected = go->selected;
	}
}

void ImGuiWindows::SetUnselected()
{
	/*for (size_t i = 0; i < length; i++)
	{

	}*/
}

Hierarchy::~Hierarchy()
{
}

void Hierarchy::ShowWindow()
{
	// Always center this window when appearing
	/*ImVec2 center = ImGui::GetMainViewport()->Pos;
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.7f));*/
	
	std::string title = "Hierarchy";
	title.append("##");
	title.append(std::to_string(id));

	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);
	if (ImGui::Begin(title.c_str(), &show))
	{
		//static const char* names[9] =
		//{
		//	"Bobby", "Beatrice", "Betty",
		//	"Brianna", "Barry", "Bernard",
		//	"Bibi", "Blaine", "Bryn"
		//};
		//for (int n = 0; n < IM_ARRAYSIZE(names); n++)
		//{
		//	ImGui::PushID(n);

		//	// Our buttons are both drag sources and drag targets here!
		//	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		//	{
		//		// Set payload to carry the index of our item (could be anything)
		//		ImGui::SetDragDropPayload("a", &n, sizeof(int));

		//		// Display preview (could be anything, e.g. when dragging an image we could decide to display
		//		// the filename and a small preview of the image, etc.)
		//		ImGui::EndDragDropSource();
		//	}
		//	if (ImGui::BeginDragDropTarget())
		//	{
		//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("a"))
		//		{
		//			IM_ASSERT(payload->DataSize == sizeof(int));
		//			int payload_n = *(const int*)payload->Data;
		//			
		//			//if (mode == Mode_Move)
		//			{
		//			}
		//		}
		//		ImGui::EndDragDropTarget();
		//	}
		//	ImGui::PopID();
		//}

		if (!App->scene->rootNode->vChildren.empty())
		{
			ShowChildren(App->scene->rootNode->vChildren, App->scene->rootNode->vChildren.size());
			if (node_clicked != -1)
			{
				// Update selection state
				// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
				//if (ImGui::GetIO().KeyCtrl)
					//selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
				//else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
					//selection_mask = (1 << node_clicked);           // Click to single-select
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

		//ImGui::PushID(current[i]->id);

		if (!current[i]->vChildren.empty())
		{
			node_flags = ImGuiTreeNodeFlags_OpenOnArrow |	ImGuiTreeNodeFlags_OpenOnDoubleClick
				| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

			if (current[i]->selected)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}			

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
			
			if (current[i]->selected)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			
			if (ImGui::TreeNodeEx((void*)(intptr_t)current[i]->id, node_flags, a.c_str()))
			{
				
			}

			if (ImGui::IsItemClicked())
			{

				node_clicked = current[i]->id;
				SetSelected(current[i]);
			}
		}

		//ImGui::PopID();
	}

	return ret;
}

//

Inspector::~Inspector()
{
}

void Inspector::ShowWindow()
{
	std::string title = "Inspector";
	title.append("##");
	title.append(std::to_string(id));

	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_Appearing);
	ImGui::SetWindowPos(ImVec2(700, 50), ImGuiCond_Appearing);
	if (ImGui::Begin(title.c_str(), &show))
	{
		if (GetSelected() != nullptr)
		{
			// --- Set ImGui ids ---
			std::string name = GetSelected()->name;
			name.insert(name.begin(), 2, '#');
			name.append(std::to_string(GetSelected()->id));

			std::string checkbox = "##Checkbox ";
			checkbox.append(GetSelected()->name + std::to_string(GetSelected()->id));

			ImGui::Checkbox(checkbox.c_str(), &GetSelected()->active);		ImGui::SameLine();
			
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