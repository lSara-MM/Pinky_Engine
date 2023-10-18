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
		if (ImGui::TreeNode(App->scene->rootNode->name.c_str()))
		{
			ShowChildren(App->scene->rootNode->vChildren, App->scene->rootNode->vChildren.size());
			ImGui::TreePop();
		}
		ShowChildren(App->scene->rootNode->vChildren, App->scene->GO_num);

		ImGui::End();
	}
}

bool Hierarchy::ShowChildren(std::vector<GameObject*> children, int num)
{
	bool ret = true;

	for (int i = 0; i < num; i++)
	{
		if (children[i]->vChildren.size() > 0)
		{
			ShowChildren(children[i]->vChildren, children[i]->vChildren.size());
		}

		if (ImGui::TreeNode(children[i]->name.c_str()))
		{
			ImGui::TreePop();
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