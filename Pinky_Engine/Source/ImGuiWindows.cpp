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

#ifdef _DEBUG
#pragma comment (lib, "MathGeoLib/libx86/libDebug/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#else
#pragma comment (lib, "MathGeoLib/libx86/libRelease/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#endif // _DEBUG

ImGuiWindows::ImGuiWindows()
{
}

ImGuiWindows::~ImGuiWindows()
{
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
	ImGui::SetNextWindowSize(ImVec2(200, 600), ImGuiCond_Appearing);
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

GameObject* Hierarchy::GetSelected()
{
	return selectedGO;
}

bool Hierarchy::ShowChildren(std::vector<GameObject*> current, int num)
{
	bool ret = true;
	ImGuiTreeNodeFlags node_flags;
	
	for (int i = 0; i < num; i++)
	{
		std::string a = current[i]->name.c_str();
		a.append(" - ");
		a.append(std::to_string(current[i]->id));		

		if (!current[i]->vChildren.empty())
		{
			node_flags |= node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

			const bool is_selected = (selection_mask & (1 << current[i]->id)) != 0;
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			bool open = ImGui::TreeNodeEx((void*)(intptr_t)current[i]->id, node_flags, a.c_str());

			if (ImGui::IsItemClicked())
			{
				node_clicked = current[i]->id;
				selectedGO = current[i];
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
				selectedGO = current[i];
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

		ImGui::End();
	}
}