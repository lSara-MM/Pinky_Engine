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
	selectedDir = ".";
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

		// TODO: Set a default width but make it resizeable
		ImGui::SetColumnWidth(0, 100);

		if (ImGui::TreeNodeEx("Assets", node_flags))
		{
			// TODO: taria bien que se pudiera quitar eso y funcionar igualmente
			if (ImGui::IsItemClicked() || selectedDir == ".")
			{
				std::vector<std::string> vDirs, vFiles;
				selectedDir = ".";

				App->fs->DiscoverFiles("Assets", vFiles, vDirs);
				vSelectedDirFiles = vFiles;
			}

			// ---RMB Click event---
			if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
			{
				ImGui::MenuItem("Assets");
				ImGui::Separator();
				if (ImGui::MenuItem("Create Folder"))
				{

				}
				if (ImGui::MenuItem("Delete Folder"))
				{

				}

				selectedDir = ".";
				ImGui::EndPopup();
			}

			ShowDirectories("Assets");
			ImGui::TreePop();
		}

		ImGui::NextColumn();

		for (int i = 0; i < vSelectedDirFiles.size(); i++)
		{
			//ImGui::Text(vSelectedDirFiles[i].c_str());
			ImGui::TreeNodeEx(vSelectedDirFiles[i].c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
			FilesMouseEvents(vSelectedDirFiles[i]);
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
			DirsMouseEvents(vDirs[i], vChildrenFiles);

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

			DirsMouseEvents(vDirs[i], vChildrenFiles);
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

void ProjectFiles::DirsMouseEvents(std::string current, std::vector<std::string> files)
{
	// ---Click event---
	if (ImGui::IsItemClicked())
	{
		selectedDir = current;
		vSelectedDirFiles = files;
	}
	// ------

	// ---RMB Click event---
	if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
	{
		ImGui::MenuItem(current.c_str());
		ImGui::Separator();
		if (ImGui::MenuItem("Create Folder"))
		{

		}
		if (ImGui::MenuItem("Delete Folder"))
		{

		}

		selectedDir = current;
		ImGui::EndPopup();
	}
}

void ProjectFiles::FilesMouseEvents(std::string current)
{
	// ---RMB Click event---
	if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
	{
		ImGui::MenuItem(current.c_str());
		ImGui::Separator();
		if (ImGui::MenuItem("Import to Scene"))
		{
			App->resource->ImportToScene(current);
		}
		if (ImGui::MenuItem("Create File"))
		{

		}
		if (ImGui::MenuItem("Delete File"))
		{

		}

		selectedFile = current;
		ImGui::EndPopup();
	}
}

Console::Console(int i) : ImGuiWindows(i)
{
	vLog = {};
	consoleWin = true;
}

Console::~Console()
{
}

void Console::ShowWindow()
{
	// Console window
	ImVec2 pos = ImGui::GetMainViewport()->WorkPos;
	ImVec2 size = ImGui::GetMainViewport()->Size;
	pos.y += size.y;
	ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, ImVec2(-0.01f, 1.0f));
	ImGui::SetNextWindowSize(ImVec2(size.x - 15, 200), ImGuiCond_Appearing);

	if (ImGui::Begin("Console", &consoleWin, ImGuiWindowFlags_MenuBar))
	{
		ImGui::BeginMenuBar();
		if (ImGui::Button("Clear"))
		{
			ClearVec(vLog);
		}
		if (ImGui::Button("Add Error message"))
		{
			vLog.push_back("[ERROR] debug error message");
		}
		if (ImGui::Button("Add Warning message"))
		{
			vLog.push_back("[WARNING] debug warning message");
		}
		ImGui::Dummy(ImVec2(225, 0));
		static ImGuiTextFilter filter;
		filter.Draw("Search", ImGui::GetFontSize() * 15);
		ImGui::EndMenuBar();

		//const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("##output", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		for (int n = 0; n < vLog.size(); n++)
		{
			if (filter.PassFilter(vLog[n].c_str()))
			{
				if (strstr(vLog[n].c_str(), "[ERROR")) { ImGui::TextColored(ImVec4(0.9f, 0.0f, 0.0f, 1.0f), vLog[n].c_str()); }
				else if (strstr(vLog[n].c_str(), "[WARNING")) { ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), vLog[n].c_str()); }
				else { ImGui::Text(vLog[n].c_str(), n); }
			}
		}

		// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
		// Using a scrollbar or mouse-wheel will take away from the bottom edge.
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) { ImGui::SetScrollHereY(1.0f); }

		ImGui::EndChild();
	} ImGui::End();
}
