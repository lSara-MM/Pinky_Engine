#pragma once
#include "Globals.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"

#include "ImGuiWindows.h"

class ProjectFiles : public ImGuiWindows
{
public:
	ProjectFiles(int i);
	~ProjectFiles();

	void ShowWindow();
	void ShowDir(std::string directory);
	void ShowDirectories(std::string directory);

	bool TreeNode(std::string currentDir, ImGuiTreeNodeFlags node_flags, bool finalDir);
	void DirsMouseEvents(std::string current, std::vector<std::string> files);

	void FilesMouseEvents(std::string currentFile, std::string currentDir);

public:
	std::string selectedDir;
	std::vector<std::string> vSelectedDirFiles;

	std::string selectedFileFullPath;
	std::string selectedFile;
};

class ResourcesManager : public ImGuiWindows
{
public:
	ResourcesManager(int i);
	~ResourcesManager();

	void ShowWindow();
public:
};