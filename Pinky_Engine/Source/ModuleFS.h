#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"

struct Path
{
	Path() : path("") {};
	//Path in reference to the first PathNode
	std::string path;
	//Folder / file itself
	std::string localPath;

	std::vector<Path> children;

	bool isLeaf = true;
	bool isFile = true;

	bool IsLastFolder() const
	{
		for (uint i = 0; i < children.size(); i++)
			if (children[i].isFile == false)
				return false;
		return true;
	}

	bool operator == (const Path node) const
	{
		return path == node.path;
	}
};

class ModuleFS : public Module
{
public:
	ModuleFS(Application* app, bool start_enabled = true);
	~ModuleFS();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:
	GameObject* rootNode;

private:

};