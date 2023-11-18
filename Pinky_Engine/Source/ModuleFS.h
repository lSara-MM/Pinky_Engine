#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"

struct NodePath
{
	NodePath() : path("") {};
	//Path in reference to the first PathNode
	std::string path;
	//Folder / file itself
	std::string localPath;

	std::vector<NodePath> vChildren;

	bool isLeaf = true;
	bool isFile = true;

	bool IsLastFolder() const
	{
		for (uint i = 0; i < vChildren.size(); i++)
		{
			if (!vChildren[i].isFile)
			{
				return false;
			}
		}

		return true;
	}

	bool operator == (const NodePath node) const
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

	// Create
	void CreateLibraryDirs();
	bool AddPath(std::string path);
	bool CreateDir(const char* dir);

	// Checks
	bool Exists(const char* file) const;
	bool IsDirectory(const char* file) const;

	// Getters
	const char* GetWriteDir() const;
	void DiscoverFiles(const char* directory, std::vector<std::string>& vFiles, std::vector<std::string>& vDirs) const;
	std::vector<std::string>& GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& vFiles) const;
	NodePath GetAllFiles(const char* directory, std::vector<std::string>* filter_ext = nullptr, std::vector<std::string>* ignore_ext = nullptr) const;
	std::string GetRealDir(const char* path) const;
	std::string GetPathRelativeToAssets(const char* originalPath) const;

	// Check extension
	bool HasExtension(const char* path) const;
	bool HasExtension(const char* path, std::string extension) const;
	bool HasExtension(const char* path, std::vector<std::string> extensions) const;

	//
	std::string NormalizePath(const char* path) const;
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) const;

	// Loads
	//// Open for Read/Write
	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;

	// Duplicate
	bool DuplicateFile(const char* file, const char* dstFolder, std::string& relativePath);
	bool DuplicateFile(const char* srcFile, const char* dstFile);

	// Others
	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;
	bool Remove(const char* file);

	uint64 GetLastModTime(const char* fileName);
	std::string GetUniqueName(const char* path, const char* name) const;

public:

private:

};