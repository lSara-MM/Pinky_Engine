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
		if (ImGui::Button("Show Error"))
		{
			vLog.push_back("[ERROR] debug error message");
		}
		if (ImGui::Button("Show Warning"))
		{
			vLog.push_back("[WARNING] debug warning message");
		}
		if (ImGui::Button("Add Error"))
		{
			vLog.push_back("[ERROR] debug error message");
		}
		if (ImGui::Button("Add Warning"))
		{
			vLog.push_back("[WARNING] debug warning message");
		}

		ImGui::Dummy(ImVec2(size.x/4, 0));
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
//------

//---Project Files---
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
	ImVec2 size = ImGui::GetMainViewport()->Size;
	pos.y += size.y;
	ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, ImVec2(-0.01f, 1.0f));
	ImGui::SetNextWindowSize(ImVec2(size.x - 15, 200), ImGuiCond_Appearing);
	if (ImGui::Begin(title.c_str(), &show))
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

		ImGui::Columns(2, "Folders", true);

		// TODO: Set a default width but make it resizeable
		ImGui::SetColumnWidth(0, 300);

		ImGui::BeginChild("ProjectDirs");
		ShowDirectories("Assets");
		ShowDirectories("PinkyAssets");
		ImGui::EndChild();

		ImGui::NextColumn();

		ImGui::BeginChild("ProjectFiles");
		for (int i = 0; i < vSelectedDirFiles.size(); i++)
		{
			//ImGui::Text(vSelectedDirFiles[i].c_str());
			ImGui::TreeNodeEx(vSelectedDirFiles[i].c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
			FilesMouseEvents(vSelectedDirFiles[i], selectedDir + "/");
		}
		ImGui::EndChild();

		ImGui::Columns(1);

	} ImGui::End();
}

void ProjectFiles::ShowDirectories(std::string directory)
{
	std::vector<std::string> vDirs, vFiles;
	std::vector<std::string> vChildrenDirs, vChildrenFiles;

	App->fs->DiscoverFiles(directory.c_str(), vFiles, vDirs);

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None;

	if (TreeNode(directory, node_flags, false))
	{
		DirsMouseEvents(directory, vFiles);

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
					ShowDirectories(directory + "/" + vDirs[i]);
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

		ImGui::TreePop();
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
		ImGui::MenuItem(current.c_str(), NULL, false, false);
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

void ProjectFiles::FilesMouseEvents(std::string currentFile, std::string currentDir)
{
	// ---RMB Click event---
	if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
	{
		ImGui::MenuItem(currentFile.c_str(), NULL, false, false);
		ImGui::Separator();
		if (ImGui::MenuItem("Import to Scene"))
		{
			App->resource->ImportToScene(currentFile, currentDir);
		}
		if (ImGui::MenuItem("Create File"))
		{

		}
		if (ImGui::MenuItem("Delete File"))
		{

		}

		selectedFile = currentFile;
		ImGui::EndPopup();
	}
}
//------

//---Resources---
ResourcesManager::ResourcesManager(int i) : ImGuiWindows(i)
{
}

ResourcesManager::~ResourcesManager()
{
}

void ResourcesManager::ShowWindow()
{
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Appearing);
	if (ImGui::Begin("Resources", &show))
	{
		ImGui::BeginChild("left pane", ImVec2(150, 300), true);
		ImGui::TextWrapped("MIT LicenseCopyright\n(c) 2023\n");
		ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the 'Software'), to dealin the Software without restriction, including without limitation the rightsto use, copy, modify, merge, publish, distribute, sublicense, and/or sellcopies of the Software, and to permit persons to whom the Software isfurnished to do so, subject to the following conditions:The above copyright notice and this permission notice shall be included in allcopies or substantial portions of the Software.THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS ORIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THEAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHERLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THESOFTWARE.");
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::BeginChild("right pane", ImVec2(500 - 200, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us

		if (ImGui::BeginTabBar("Pinky Engine"), ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton)
		{
			if (ImGui::BeginTabItem("Pinky Engine"))
			{
				ImGui::TextWrapped("3D Game engine done for the Game Engines subject in the CITM bachelor's degree in Video Game Design and Development");
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
		ImGui::EndGroup();
	} ImGui::End();
}