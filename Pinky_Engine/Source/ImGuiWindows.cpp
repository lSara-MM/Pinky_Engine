#include "ImGuiWindows.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "GameObject.h"


#include "External Libraries/SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Source/External Libraries/Glew/libx86/glew32.lib")

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"
#include "External Libraries/ImGui/misc/cpp/imgui_stdlib.h"
//
#include "External Libraries/ImGui/imgui_custom.h"


#ifdef _DEBUG
#pragma comment (lib, "Source/External Libraries/MathGeoLib/libx86/libDebug/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#else
#pragma comment (lib, "Source/External Libraries/MathGeoLib/libx86/libRelease/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#endif // _DEBUG

ImGuiWindows::ImGuiWindows(int i) : id(i)
{
	selectedGOs = {};
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
				SetSelectedState(selectedGOs[i], false);
			}
			ClearVec(selectedGOs);
		}

		// On click select or deselect item
		go->selected = !go->selected;

		// If the item was selected, add it to the vec, otherwise remove it
		if (go->selected)
		{
			selectedGOs.push_back(go);
			// Set selected go children to the same state as the clicked item
			SetSelectedState(go, go->selected);
		}
		else if (!selectedGOs.empty())
		{
			SetSelectedState(go, false);
			selectedGOs.erase(std::find(selectedGOs.begin(), selectedGOs.end(), go));
		}
	}
	else
	{
		ClearVec(selectedGOs);
	}
}

void ImGuiWindows::SetSelectedState(GameObject* go, bool selected)
{
	// Must change go value manually. In "active" not necessary since it changes from the toggle
	go->selected = selected;
	for (auto i = 0; i < go->vChildren.size(); i++)
	{
		if (!go->vChildren.empty())
		{
			SetSelectedState(go->vChildren[i], selected);
		}

		go->vChildren[i]->selected = selected;
	}
}
//------

Hierarchy::Hierarchy(int i) : ImGuiWindows(i)
{
	draggedGO = nullptr;
	disabledColor = ImVec4(0.675f, 0.675f, 0.675f, 1.0f);
}

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

	ImVec2 pos = ImGui::GetMainViewport()->WorkPos;
	ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, ImVec2(-0.05f, -0.05f));
	ImGui::SetNextWindowSize(ImVec2(200, 490), ImGuiCond_Appearing);
	if (ImGui::Begin(title.c_str(), &show))
	{
		// ---Root node---
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.953f, 0.533f, 0.969f, 1.0f));
		ImGui::TreeNodeEx((void*)(intptr_t)App->scene->rootNode->GetUid(), node_flags, App->scene->rootNode->name.c_str());
		ImGui::PopStyleColor();

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
	} ImGui::End();
}

bool Hierarchy::ShowChildren(std::vector<GameObject*> current, int num)
{
	bool ret = true;
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None;

	for (int i = 0; i < num; i++)
	{
		if (!current[i]->vChildren.empty())
		{
			bool open = TreeNode(current[i], node_flags);
			MouseEvents(current[i]);

			if (open)
			{
				ShowChildren(current[i]->vChildren, current[i]->vChildren.size());
				ImGui::TreePop();
			}
		}
		else
		{
			TreeNode(current[i], node_flags);
			MouseEvents(current[i]);
		}
	}

	return ret;
}

bool Hierarchy::TreeNode(GameObject* current, ImGuiTreeNodeFlags node_flags)
{
	bool ret = false;

	if (!current->vChildren.empty())
	{
		node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_SpanAvailWidth;
	}
	else
	{
		node_flags = ImGuiTreeNodeFlags_None;
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	if (current->selected)
	{
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}


	// TODO: en release parpadea nose porque
	if (!current->isActive)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, disabledColor);
	}

	ret = ImGui::TreeNodeEx((void*)(intptr_t)current->GetUid(), node_flags, current->name.c_str());
	if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
	{
		ImGui::MenuItem(current->name.c_str());
		ImGui::Separator();
		if (ImGui::MenuItem("(WIP) Copy", "Ctrl + C"))
		{

		}
		if (ImGui::MenuItem("(WIP) Paste", "Ctrl + V"))
		{

		}
		if (ImGui::MenuItem("(WIP) Paste as Child", "Ctrl + Shift + V"))
		{

		}
		ImGui::Separator();
		if (ImGui::MenuItem("Create Empty GameObject"))
		{
			GameObject* go = new GameObject("Empty GameObject", current);
			go = nullptr;
		}

		if (ImGui::BeginMenu("Mesh"))
		{
			App->editor->PrimitivesMenu(current);
			ImGui::EndMenu();
		}

		SetSelected(current);
		ImGui::EndPopup();
	}

	if (!current->isActive)
	{
		ImGui::PopStyleColor(1);
	}

	return ret;
}

void Hierarchy::MouseEvents(GameObject* current)
{
	// ---Drag and Drop event---
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		// Set payload to carry the index of our item (could be anything)
		ImGui::SetDragDropPayload("GameObject", &current, sizeof(GameObject*));
		//ImGui::SetDragDropPayload("GameObject", current, sizeof(GameObject*));
		//ImGui::SetDragDropPayload("GameObject", current, sizeof(GameObject**));

		SetDragged(current);
		ImGui::Text(current->name.c_str());

		// Display preview (could be anything, e.g. when dragging an image we could decide to display
		// the filename and a small preview of the image, etc.)
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GameObject*));
			GameObject* payload_n = (GameObject*)payload->Data;

			// TODO:
			//payload_n->pParent->ReParent(current);
			
			// If dragged go is parent of target go, don't do anything
			GameObject* go = nullptr;
			if (GetDragged()->FindChild(current->GetUid(), go) == nullptr)
			{
				GetDragged()->ReParent(current);
			}
			go = nullptr;
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

	ImVec2 pos = ImGui::GetMainViewport()->WorkPos;
	pos.x += ImGui::GetMainViewport()->Size.x;
	ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, ImVec2(1.05f, -0.05f));
	ImGui::SetNextWindowSize(ImVec2(260, 490), ImGuiCond_Appearing);
	if (ImGui::Begin(title.c_str(), &show))
	{
		if (!GetSelectedGOs().empty())
		{
			GameObject* go = GetSelectedGOs().back();

			// --- Set ImGui ids ---
			std::string name = go->name;
			name.insert(name.begin(), 2, '#');
			name.append(std::to_string(go->GetUid()));

			std::string toggle = "##Toggle ";
			toggle.append(go->name + std::to_string(go->GetUid()));
			//------

			if (ImGuiCustom::ToggleButton(toggle.c_str(), &go->isActive))
			{
				SetActiveState(go, go->isActive);
			}
			ImGui::SameLine();

			if (!go->isActive) { ImGui::BeginDisabled(); }

			std::string temp = go->name;
			if (ImGui::InputText(name.c_str(), &temp, ImGuiInputTextFlags_EnterReturnsTrue)) { go->name = temp; }
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
				//ImGui::TextUnformatted(selected_fish == -1 ? "<None>" : components[selected_fish].c_str());
			}

			if (ImGui::BeginPopup("AddComponents"))
			{
				ImGui::SeparatorText("Components");

				// Skip transform
				// --- Add component Mesh ---
				if (go->mesh == nullptr)
				{
					if (ImGui::BeginMenu("Mesh"))
					{
						App->editor->PrimitivesMenu(go, true);
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

			if (!go->isActive) { ImGui::EndDisabled(); }


			go = nullptr;
		}


	} ImGui::End();
}

void Inspector::SetActiveState(GameObject* go, bool isActive)
{
	for (auto i = 0; i < go->vChildren.size(); i++)
	{
		if (!go->vChildren.empty())
		{
			SetActiveState(go->vChildren[i], isActive);
		}

		go->vChildren[i]->isActive = isActive;
	}
}
//------