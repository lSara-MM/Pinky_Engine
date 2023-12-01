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
	void ShowDirectories(std::string directory);

	bool TreeNode(std::string currentDir, ImGuiTreeNodeFlags node_flags, bool finalDir);
	void MouseEvents(std::string current, std::vector<std::string> files);

public:
	std::string selectedDir;
	std::vector<std::string> vSelectedDirFiles;
};