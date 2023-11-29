#include "ImGuiWindows.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "FileSystemManager.h"

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
#include "ImGuiPorjectFiles.h"


#ifdef _DEBUG
#pragma comment (lib, "Source/External Libraries/MathGeoLib/libx86/libDebug/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#else
#pragma comment (lib, "Source/External Libraries/MathGeoLib/libx86/libRelease/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#endif // _DEBUG


ProjectFiles::ProjectFiles(int i) : ImGuiWindows(i)
{

}

ProjectFiles::~ProjectFiles()
{
}

void ProjectFiles::ShowWindow()
{
	std::string title = "Project";
	title.append("##");
	title.append(std::to_string(id));


	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	ImGuiID dockspace_id = ImGui::GetID("DockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	static auto first_time = true;
	if (first_time)
	{
		first_time = false;

		ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

		// split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
		//   window ID to split, direction, fraction (between 0 and 1), the final two setting let's us choose which id we want (which ever one we DON'T set as NULL, will be returned by the function)
		//                                                              out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction
		auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);
		auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);

		// we now dock our windows into the docking node we made above
		ImGui::DockBuilderDockWindow("Down", dock_id_down);
		ImGui::DockBuilderDockWindow("Left", dock_id_left);
		ImGui::DockBuilderFinish(dockspace_id);
	}

	ImGui::Begin("Left");
	ImGui::Text("Hello, left!");
	ImGui::End();

	ImGui::Begin("Down");
	ImGui::Text("Hello, down!");
	ImGui::End();


	ImVec2 pos = ImGui::GetMainViewport()->WorkPos;
	//ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, ImVec2(-0.05f, -0.05f));
	//ImGui::SetNextWindowSize(ImVec2(200, 490), ImGuiCond_Appearing);
	if (ImGui::Begin(title.c_str(), &show))
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

		ImGui::Columns(2, "Folders", true);
		ShowDirectories("Assets");
		ImGui::Columns(1);

	} ImGui::End();
}

void ProjectFiles::ShowDirectories(std::string directory)
{
	std::vector<std::string> vDirs;
	std::vector<std::string> vFiles;

	std::string dir = directory + "/";

	App->fs->DiscoverFiles(dir.c_str(), vFiles, vDirs);

	for (int i = 0; i < vDirs.size(); i++)
	{
		if (ImGui::TreeNodeEx((void*)(intptr_t)i, ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen, vDirs[i].c_str()))
		{
			ShowDirectories(dir + vDirs[i]);
			ImGui::TreePop();
		}
	}
}
