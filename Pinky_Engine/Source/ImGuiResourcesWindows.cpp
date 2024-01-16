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
#include "ImGuiResourcesWindows.h"
#include "ModuleResource.h"

//---Project Files---
ProjectFiles::ProjectFiles(int i) : ImGuiWindows(i)
{
	selectedDir = "Assets";
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
	//ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, ImVec2(-0.01f, 1.0f));
	//ImGui::SetNextWindowSize(ImVec2(size.x - 15, 200), ImGuiCond_Appearing);
	if (ImGui::Begin(title.c_str(), &show))
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		ImGui::Columns(2, "Folders", true);

		// TODO: Set a default width but make it resizeable
		ImGui::SetColumnWidth(0, 300);

		ImGui::BeginChild("ProjectDirs");
		ShowDir("Assets");
		ShowDir("PinkyAssets");
		ImGui::EndChild();

		ImGui::NextColumn();

		ImGui::BeginChild("ProjectFiles");
		for (int i = 0; i < vSelectedDirFiles.size(); i++)
		{
			if (selectedFile == vSelectedDirFiles[i])
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}
			else
			{
				node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			}

			ImGui::TreeNodeEx(vSelectedDirFiles[i].c_str(), node_flags);
			FilesMouseEvents(vSelectedDirFiles[i], selectedDir + "/");
		}
		ImGui::EndChild();

		ImGui::Columns(1);

	} ImGui::End();
}

void ProjectFiles::ShowDir(std::string directory)
{
	std::vector<std::string> vDirs, vFiles;
	std::vector<std::string> vChildrenDirs, vChildrenFiles;

	App->fs->DiscoverFiles(directory.c_str(), vFiles, vDirs);

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None;

	bool open = TreeNode(directory, node_flags, vDirs.empty());
	DirsMouseEvents(directory, vFiles);

	if (open)
	{
		ShowDirectories(directory);

		if (!vDirs.empty())
		{
			ImGui::TreePop();
		}
	}
}

void ProjectFiles::ShowDirectories(std::string directory)
{
	std::vector<std::string> vDirs, vFiles;
	std::vector<std::string> vChildrenDirs, vChildrenFiles;

	App->fs->DiscoverFiles(directory.c_str(), vFiles, vDirs);

	// Update info shown
	for (int i = 0; i < vFiles.size(); i++)
	{
		// Get full relative path
		if (vFiles[i] == selectedFile)
		{
			selectedFileFullPath = selectedDir;
		}
		if (directory == selectedDir)
		{
			vSelectedDirFiles = vFiles;
		}
	}


	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None;

	for (int i = 0; i < vDirs.size(); i++)
	{
		App->fs->DiscoverFiles((directory + "/" + vDirs[i]).c_str(), vChildrenFiles, vChildrenDirs);

		// Update info shown
		if (vDirs[i] == selectedDir)
		{
			vSelectedDirFiles = vChildrenFiles;
		}

		// Get full relative path
		for (int i = 0; i < vChildrenFiles.size(); i++)
		{
			if (vChildrenFiles[i] == selectedFile)
			{
				selectedFileFullPath = directory + "/" + selectedDir;
			}
		}

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
		ClearVec(vSelectedDirFiles);
		vSelectedDirFiles = files;
	}
	// ------

	// ---RMB Click event---
	if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
	{
		ImGui::MenuItem(current.c_str(), NULL, false, false);
		ImGui::Separator();
		//if (ImGui::MenuItem("Create Folder"))
		//{

		//}
		//if (ImGui::MenuItem("Delete Folder"))
		//{
		//	//App->fs->Remove((selectedFileFullPath + "/" + current).c_str());
		//}

		selectedDir = current;
		ClearVec(vSelectedDirFiles);
		vSelectedDirFiles = files;
		ImGui::EndPopup();
	}
}

void ProjectFiles::FilesMouseEvents(std::string currentFile, std::string currentDir)
{
	// ---Click event---
	if (ImGui::IsItemClicked())
	{
		selectedFile = currentFile;
	}
	// ------
	// 
	// ---RMB Click event---
	if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
	{
		ImGui::MenuItem(currentFile.c_str(), NULL, false, false);
		ImGui::Separator();

		//App->resource->CheckExtensionType(currentFile.c_str());
		if (ImGui::MenuItem("Import to Scene"))
		{
			//App->resource->pendingToLoadScene = true;

			if (currentFile == "MainScreen.pnk")
			{
				App->scene->ImportDefaultMainScreen();
			}
			else if (currentFile == "Street.pnk")
			{
				App->scene->ImportDefaultScene();
			}
			else
			{
				App->resource->sceneFileName = currentFile;
				App->resource->ImportToSceneV(currentFile, selectedFileFullPath + "/");
			}
		}
		//if (ImGui::MenuItem("Create File (WIP)", NULL, false, false))	// TODO:
		//{

		//}
		if (ImGui::MenuItem("Delete File"))
		{
			DeleteFileAndRefs((selectedFileFullPath + "/" + currentFile).c_str());
		}

		selectedFile = currentFile;
		ImGui::EndPopup();
	}
}

void ProjectFiles::DeleteFileAndRefs(const char* filePath)
{
	std::string path = filePath;

	switch (App->resource->CheckExtensionType(filePath))
	{
	case R_TYPE::MESH:
		if (App->fs->Exists((path + ".meta").c_str()))
		{
			App->parson->DeleteLibDirs((path + ".meta").c_str());
			App->fs->Remove((path + ".meta").c_str());
		}
		break;
	case R_TYPE::TEXTURE:
		if (App->fs->Exists((path + ".meta").c_str()))
		{
			App->parson->DeleteLibDirs((path + ".meta").c_str());
			App->fs->Remove((path + ".meta").c_str());
		}
		break;
	case R_TYPE::O_META:
	{
		App->parson->DeleteLibDirs(path.c_str());
	}
	break;
	case R_TYPE::NONE:
		break;
	default:
		break;
	}

	App->fs->Remove(filePath);
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
	//ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	//ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Appearing);
	if (ImGui::Begin("Resources", &show))
	{
		if (ImGui::BeginTable("Resources", 5, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter
			| ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_NoHostExtendX))
		{
			ImGui::TableSetupColumn("UID", ImGuiTableFlags_SizingFixedFit);
			ImGui::TableSetupColumn("Name", ImGuiTableFlags_SizingFixedFit);
			ImGui::TableSetupColumn("Type", ImGuiTableFlags_SizingFixedFit);
			ImGui::TableSetupColumn("Count", ImGuiTableFlags_SizingFixedFit);
			ImGui::TableSetupColumn(" ", ImGuiTableFlags_SizingFixedFit);
			ImGui::TableHeadersRow();

			std::array<std::string, 2> resTypes = { "Mesh", "Texture" };
			std::map<u32, Resource*>::iterator it = App->resource->mResources.begin();
			int i = 0;
			while (it != App->resource->mResources.end())
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%d", it->first);

				ImGui::TableSetColumnIndex(1);
				ImGui::Text(it->second->name.c_str());

				ImGui::TableSetColumnIndex(2);
				ImGui::Text(std::to_string(it->second->count).c_str());

				ImGui::TableSetColumnIndex(3);
				ImGui::Text((resTypes[(int)it->second->GetType()]).c_str());

				ImGui::TableSetColumnIndex(4);
				std::string deleteButton = "Delete##" + std::to_string(i);
				if (ImGui::Button(deleteButton.c_str()))
				{
					App->resource->vPendingToDelete.push_back(it->second);
				}
				++it; ++i;
			}
			ImGui::EndTable();
		}

	} ImGui::End();
}