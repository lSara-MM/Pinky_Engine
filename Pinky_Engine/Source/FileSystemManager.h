#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"

class NodePath
{
public:
	NodePath();

	bool IsLastFolder() const;
	bool operator == (const NodePath node) const;

public:
	// Path in reference to the first NodePath
	std::string path;

	// Folder / file itself
	std::string localPath;

	std::vector<NodePath> vChildren;

	bool isLeaf;
	bool isFile;
};

class FileSystemManager : public Module
{
public:
	FileSystemManager(Application* app, bool start_enabled = true);
	~FileSystemManager();

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
	std::vector<std::string>& GetByExt(const char* directory, const char* extension, std::vector<std::string>& vFiles) const;
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
	uint Load(const char* path, const char* file, char** buffer) const;
	uint Load(const char* file, char** buffer) const;

	// Duplicate
	bool DuplicateFile(const char* file, const char* dstFolder, std::string& relativePath);
	bool DuplicateFile(const char* srcFile, const char* dstFile);

	// Others
	uint Save(const char* file, const void* buffer, uint size, bool append = false) const;
	bool Remove(const char* file);

	uint64 GetLastModTime(const char* fileName);
	std::string GetUniqueName(const char* path, const char* name) const;

public:

private:

};