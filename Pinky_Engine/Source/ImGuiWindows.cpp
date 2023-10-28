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
	selectedGOs = {};
	id = i;
}

ImGuiWindows::~ImGuiWindows()
{
	ClearVec(selectedGOs);
	show = true;
}

std::vector<GameObject*> ImGuiWindows::GetSelectedGOs()
{
	return selectedGOs;
}

void ImGuiWindows::SetSelected(std::vector<GameObject*> vSelected)
{
	selectedGOs = vSelected;
}

void ImGuiWindows::SetSelected(GameObject* go)
{
	if (go != nullptr)
	{
		// If ctrl not pressed, set everything to false clear and the selected go's vector 
		if (!ImGui::GetIO().KeyCtrl)
		{
			for (auto i = 0; i < selectedGOs.size(); i++)
			{
				selectedGOs[i]->selected = false;
			}
			ClearVec(selectedGOs);
		}

		// On click select or deselect item
		go->selected = !go->selected;
		
		// If the item was selected, add it to the vec, otherwise remove it
		if (go->selected)
		{
			selectedGOs.push_back(go);
		}
		else 
		{
			selectedGOs.erase(std::find(selectedGOs.begin(), selectedGOs.end(), go));
		}

		// Set selected go children to the same state as the clicked item
		for (auto i = 0; i < go->vChildren.size(); i++)
		{
			go->vChildren[i]->selected = go->selected;
		}
	}
	else
	{
		ClearVec(selectedGOs);
	}
}
//------

Hierarchy::Hierarchy(int i) : ImGuiWindows(i) 
{ draggedGO = nullptr; }

//---Hierarchy---
Hierarchy::~Hierarchy()
{
	draggedGO = nullptr;
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
		// ---Root node---
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

		ImGui::TreeNodeEx((void*)(intptr_t)App->scene->rootNode->GetUid(), node_flags, App->scene->rootNode->name.c_str());

		// Root node only target, can't be dragged
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
			{
				GetDragged()->ReParent(App->scene->rootNode);
			}
			ImGui::EndDragDropTarget();
		}

		if (!App->scene->rootNode->vChildren.empty())
		{
			ShowChildren(App->scene->rootNode->vChildren, App->scene->rootNode->vChildren.size());
		}

		ImGui::TreePop();
		ImGui::End();
	}
}

bool Hierarchy::ShowChildren(std::vector<GameObject*> current, int num)
{
	bool ret = true;
	ImGuiTreeNodeFlags node_flags;

	for (int i = 0; i < num; i++)
	{
		if (!current[i]->vChildren.empty())
		{
			node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
				| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

			if (current[i]->selected)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}

			bool open = ImGui::TreeNodeEx((void*)(intptr_t)current[i]->GetUid(), node_flags, current[i]->name.c_str());

			MouseEvents(current[i]);

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

			ImGui::TreeNodeEx((void*)(intptr_t)current[i]->GetUid(), node_flags, current[i]->name.c_str());
			MouseEvents(current[i]);
		}
	}

	return ret;
}

void Hierarchy::MouseEvents(GameObject* current)
{
	// ---Drag and Drop event---
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		// Set payload to carry the index of our item (could be anything)
		ImGui::SetDragDropPayload("GameObject", current, sizeof(GameObject*));

		SetDragged(current);

		// Display preview (could be anything, e.g. when dragging an image we could decide to display
		// the filename and a small preview of the image, etc.)
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
		{
			// If dragged go is parent of target go, don't do anything
			if (GetDragged()->FindChild(current->GetUid()) == nullptr)
			{
				GetDragged()->ReParent(current);
			}
		}
		ImGui::EndDragDropTarget();
	}
	// ------


	// ---Click event---
	if (ImGui::IsItemClicked())
	{
		SetSelected(current);
	}
	// ------
}

GameObject* Hierarchy::GetDragged()
{
	return draggedGO;
}

void Hierarchy::SetDragged(GameObject* go)
{
	draggedGO = go;
}
//------

//---Inspector---
Inspector::~Inspector()
{
}

void Inspector::ShowWindow()
{
	std::string title = "Inspector";
	title.append("##");
	title.append(std::to_string(id));

	ImGui::SetNextWindowSize(ImVec2(260, 500), ImGuiCond_Appearing);
	ImGui::SetWindowPos(ImVec2(700, 50), ImGuiCond_Appearing);
	if (ImGui::Begin(title.c_str(), &show))
	{
		if (!GetSelectedGOs().empty())
		{
			GameObject* go = GetSelectedGOs().back();

			// --- Set ImGui ids ---
			std::string name = go->name;
			name.insert(name.begin(), 2, '#');
			name.append(std::to_string(go->GetUid()));

			std::string checkbox = "##Checkbox ";
			checkbox.append(go->name + std::to_string(go->GetUid()));

			ImGui::ToggleButton(checkbox.c_str(), &go->active);		ImGui::SameLine();

			if (!go->active) { ImGui::BeginDisabled(); }

			ImGui::InputText(name.c_str(), &go->name);
			ImGui::Dummy(ImVec2(0, 10));

			for (auto i = 0; i < go->vComponents.size(); i++)
			{
				go->vComponents[i]->ShowInInspector();
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
				if (go->numMeshes == 0)
				{
					if (ImGui::BeginMenu("Mesh"))
					{
						std::array<std::string, 4> components = { "Cube", "Sphere", "Cylinder", "Plane" };

						for (int i = 0; i < components.size(); i++)
						{
							if (ImGui::MenuItem(components[i].c_str()))
							{
								go->AddComponent(C_TYPE::MESH, nullptr, ai::POLY_PRIMITIVE_TYPE(i));
							}
						}
						ImGui::EndMenu();
					}
				}

				// --- Add component Material ---
				if (go->numMaterials == 0)
				{
					if (ImGui::MenuItem("Material"))
					{
						go->AddComponent(C_TYPE::MATERIAL);
					}
				}
				ImGui::EndPopup();
			}

			if (!go->active) { ImGui::EndDisabled(); }


			go = nullptr;
		}

		ImGui::End();
	}
}
//------