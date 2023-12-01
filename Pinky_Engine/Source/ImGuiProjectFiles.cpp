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
#include "ImGuiProjectFiles.h"


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

	ImVec2 pos = ImGui::GetMainViewport()->WorkPos;
	//ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, ImVec2(-0.05f, -0.05f));
	//ImGui::SetNextWindowSize(ImVec2(200, 490), ImGuiCond_Appearing);
	if (ImGui::Begin(title.c_str(), &show))
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

		ImGui::Columns(2, "Folders", true);
		if (ImGui::TreeNodeEx("Assets", node_flags))
		{
			if (ImGui::IsItemClicked())
			{
				std::vector<std::string> vDirs, vFiles;
				selectedDir = ".";

				App->fs->DiscoverFiles("Assets", vFiles, vDirs);
				vSelectedDirFiles = vFiles;
			}

			ShowDirectories("Assets");
			ImGui::TreePop();
		}

		ImGui::NextColumn();

		for (int i = 0; i < vSelectedDirFiles.size(); i++)
		{
			ImGui::Text(vSelectedDirFiles[i].c_str());
		}

		ImGui::Columns(1);

	} ImGui::End();
}

void ProjectFiles::ShowDirectories(std::string directory)
{
	std::vector<std::string> vDirs, vFiles;
	std::vector<std::string> vChildrenDirs, vChildrenFiles;

	std::string dir = directory;

	App->fs->DiscoverFiles(dir.c_str(), vFiles, vDirs);

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None;

	for (int i = 0; i < vDirs.size(); i++)
	{
		App->fs->DiscoverFiles((directory + "/" + vDirs[i]).c_str(), vChildrenFiles, vChildrenDirs);

		if (!vChildrenDirs.empty())
		{
			node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
				| ImGuiTreeNodeFlags_SpanAvailWidth;

			bool open = TreeNode(vDirs[i], node_flags, false);
			MouseEvents(vDirs[i], vChildrenFiles);

			if (open)
			{
				ShowDirectories(dir + "/" + vDirs[i]);
				ImGui::TreePop();
			}
		}
		else
		{
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			TreeNode(vDirs[i], node_flags, true);

			MouseEvents(vDirs[i], vChildrenFiles);
		}

		vChildrenFiles.clear();
		vChildrenDirs.clear();
	}
}

bool ProjectFiles::TreeNode(std::string currentDir, ImGuiTreeNodeFlags node_flags, bool finalDir)
{
	bool ret = false;

	if (!finalDir)
	{
		node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_SpanAvailWidth;
	}
	else
	{
		node_flags = ImGuiTreeNodeFlags_None;
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	if (currentDir == selectedDir)
	{
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}

	ret = ImGui::TreeNodeEx(currentDir.c_str(), node_flags);

	return ret;
}

void ProjectFiles::MouseEvents(std::string current, std::vector<std::string> files)
{
	// ---Click event---
	if (ImGui::IsItemClicked())
	{
		selectedDir = current;
		vSelectedDirFiles = files;
	}
	// ------
}
