#include "Globals.h"
#include "Application.h"
#include "FileSystemManager.h"

#include "External Libraries\SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "External Libraries/PhysFS/include/physfs.h"
//#include <fstream>
#include <filesystem>
#pragma comment( lib, "Source/External Libraries/PhysFS/libx86/physfs.lib" )


// Node Path
NodePath::NodePath() : path("")
{
	isLeaf = true;
	isFile = true;
}

bool NodePath::IsLastFolder() const
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

bool NodePath::operator == (const NodePath node) const
{
	return path == node.path;
}

//
FileSystemManager::FileSystemManager()
{
	// needs to be created before Init so other modules can use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(nullptr);
	SDL_free(base_path);

	// Setting the working directory as the writing directory
	if (PHYSFS_setWriteDir(".") == 0)
	{
		LOG("[ERROR] File System: Could not write dir: %s\n", PHYSFS_getLastError());
	}

	AddPath("."); // Adding ProjectFolder (working directory)
	AddPath("Assets");
	AddPath("Library");
	AddPath("PinkyAssets");

	CreateLibraryDirs();
}

// Destructor
FileSystemManager::~FileSystemManager()
{}


// Create
void FileSystemManager::CreateLibraryDirs()
{
	CreateDir(LIBRARY_PATH);
	CreateDir(MESHES_PATH);
	CreateDir(TEXTURES_PATH);
}

bool FileSystemManager::AddPath(std::string path)
{
	bool ret = false;

	if (PHYSFS_mount(path.c_str(), nullptr, 1) == 0)
	{
		LOG("[ERROR] File System: Could not add a path: %s\n", PHYSFS_getLastError());
	}
	else
	{
		ret = true;
	}

	return ret;
}

bool FileSystemManager::CreateDir(const char* dir)
{
	if (IsDirectory(dir) == false)
	{
		PHYSFS_mkdir(dir);
		return true;
	}

	return false;
}


// Checks
//// Check if a file exists
bool FileSystemManager::Exists(const char* file) const
{
	return PHYSFS_exists(file) != 0;
}

//// Check if a file is a directory
bool FileSystemManager::IsDirectory(const char* file) const
{
	return PHYSFS_isDirectory(file) != 0;
}


// Getters
const char* FileSystemManager::GetWriteDir() const
{
	//TODO: erase first annoying dot (".")
	return PHYSFS_getWriteDir();
}

void FileSystemManager::DiscoverFiles(const char* directory, std::vector<std::string>& vFiles, std::vector<std::string>& vDirs) const
{
	char** rc = PHYSFS_enumerateFiles(directory);

	for (char** i = rc; *i != nullptr; i++)
	{
		std::string str = std::string(directory) + std::string("/") + std::string(*i);

		(IsDirectory(str.c_str())) ? vDirs.push_back(*i) : vFiles.push_back(*i);
	}

	PHYSFS_freeList(rc);
}

std::vector<std::string>& FileSystemManager::GetByExt(const char* directory, const char* extension, std::vector<std::string>& vFiles) const
{
	std::vector<std::string> files, dirs;

	DiscoverFiles(directory, files, dirs);

	for (uint i = 0; i < files.size(); i++)
	{
		std::string ext;
		SplitFilePath(files[i].c_str(), nullptr, nullptr, &ext);

		if (ext == extension)
		{
			vFiles.push_back(files[i]);
		}
	}

	return vFiles;
}

NodePath FileSystemManager::GetAllFiles(const char* directory, std::vector<std::string>* filter_ext, std::vector<std::string>* ignore_ext) const
{
	NodePath root;

	if (Exists(directory))
	{
		root.path = directory;
		SplitFilePath(directory, nullptr, &root.localPath);

		if (root.localPath == "")
		{
			root.localPath = directory;
		}

		std::vector <std::string> vFiles, vDirs;
		DiscoverFiles(directory, vFiles, vDirs);

		//Adding all child directories
		for (uint i = 0; i < vDirs.size(); i++)
		{
			std::string str = directory;
			str.append("/").append(vDirs[i]);
			root.vChildren.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
		}

		//Adding all child files
		for (uint i = 0; i < vFiles.size(); i++)
		{
			//Filtering extensions
			bool filter = true, discard = false;
			if (filter_ext != nullptr)
			{
				filter = HasExtension(vFiles[i].c_str(), *filter_ext);
			}
			if (ignore_ext != nullptr)
			{
				discard = HasExtension(vFiles[i].c_str(), *ignore_ext);
			}
			if (filter == true && discard == false)
			{
				std::string str = directory;
				str.append("/").append(vFiles[i]);
				root.vChildren.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
			}
		}
		root.isFile = HasExtension(root.path.c_str());
		root.isLeaf = root.vChildren.empty() == true;
	}
	return root;
}

std::string FileSystemManager::GetRealDir(const char* path) const
{
	std::string dir;
	dir = PHYSFS_getBaseDir();

	std::string baseDir = PHYSFS_getBaseDir();
	std::string searchPath = *PHYSFS_getSearchPath();
	std::string realDir = PHYSFS_getRealDir(path);

	dir.append(*PHYSFS_getSearchPath()).append("/");
	dir.append(PHYSFS_getRealDir(path)).append("/").append(path);

	return dir;
}

std::string FileSystemManager::GetPathRelativeToAssets(const char* originalPath) const
{
	return GetRealDir(originalPath);
}


uint64 FileSystemManager::GetLastModTime(const char* fileName)
{
	return PHYSFS_getLastModTime(fileName);
}

std::string FileSystemManager::GetUniqueName(const char* path, const char* name) const
{
	//TODO: modify to distinguix files and dirs?
	std::vector<std::string> vFiles, vDirectories;
	DiscoverFiles(path, vFiles, vDirectories);

	std::string finalName(name);
	bool unique = false;

	for (uint i = 0; i < 50 && unique == false; ++i)
	{
		unique = true;

		// Build the compare name (name_i)
		if (i > 0)
		{
			finalName = std::string(name).append("_");

			if (i < 10)
			{
				finalName.append("0");
			}
			finalName.append(std::to_string(i));
		}

		// Iterate through all the files to find a matching name
		for (uint f = 0; f < vFiles.size(); ++f)
		{
			if (finalName == vFiles[f])
			{
				unique = false;
				break;
			}
		}
	}

	return finalName;
}


// Check extension
bool FileSystemManager::HasExtension(const char* path) const
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	return ext != "";
}

bool FileSystemManager::HasExtension(const char* path, std::string extension) const
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);

	return ext == extension;
}

bool FileSystemManager::HasExtension(const char* path, std::vector<std::string> extensions) const
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	if (ext == "")
	{
		return true;
	}

	for (uint i = 0; i < extensions.size(); i++)
	{
		if (extensions[i] == ext)
		{
			return true;
		}
	}

	return false;
}


//
std::string FileSystemManager::NormalizePath(const char* full_path) const
{
	std::string newPath(full_path);

	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '\\')
		{
			newPath[i] = '/';
		}
	}

	return newPath;
}

void FileSystemManager::SplitFilePath(const char* full_path, std::string* path, std::string* file, std::string* extension) const
{
	if (full_path != nullptr)
	{
		std::string full(full_path);
		size_t pos_separator = full.find_last_of("\\/");
		size_t pos_dot = full.find_last_of(".");

		if (path != nullptr)
		{
			if (pos_separator < full.length())
			{
				*path = full.substr(0, pos_separator + 1);
			}
			else
			{
				path->clear();
			}
		}

		if (file != nullptr)
		{
			if (pos_separator < full.length())
			{
				*file = full.substr(pos_separator + 1, pos_dot - pos_separator - 1);
			}
			else
			{
				*file = full.substr(0, pos_dot);
			}
		}

		if (extension != nullptr)
		{
			if (pos_dot < full.length())
			{
				*extension = full.substr(pos_dot + 1);
			}
			else
			{
				extension->clear();
			}
		}
	}
}

std::string FileSystemManager::GetFileName(const char* full_path) const
{
	std::string fileName;

	if (full_path != nullptr)
	{
		std::string full(full_path);
		size_t pos_separator = full.find_last_of("\\/");
		size_t pos_dot = full.find_last_of(".");

		if (pos_separator < full.length())
		{
			fileName = full.substr(pos_separator + 1, pos_dot - pos_separator - 1);
		}
		else
		{
			fileName = full.substr(0, pos_dot);
		}
	}
	return fileName;
}


// Loads
uint FileSystemManager::Load(const char* path, const char* file, char** buffer) const
{
	std::string full_path(path);
	full_path += file;

	return Load(full_path.c_str(), buffer);
}

// Read a file and put it in a new buffer
uint FileSystemManager::Load(const char* file, char** buffer) const
{
	uint ret = 0;
	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if (fs_file != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fs_file);

		if (size > 0)
		{
			*buffer = new char[size + 1];
			uint readed = (uint)PHYSFS_read(fs_file, *buffer, 1, size);

			if (readed != size)
			{
				LOG("[ERROR] File System: Could not read from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE_ARRAY(buffer);
			}
			else
			{
				ret = readed;
				//Adding end of file at the end of the buffer. Loading a shader file does not add this for some reason
				(*buffer)[size] = '\0';
			}
		}

		if (PHYSFS_close(fs_file) == 0)
		{
			LOG("[ERROR] File System: Could not close file %s: %s\n", file, PHYSFS_getLastError());
		}
	}
	else
	{
		LOG("[ERROR] File System: Could not open file %s: %s\n", file, PHYSFS_getLastError());
	}

	return ret;
}


// Duplicate
bool FileSystemManager::DuplicateFile(const char* file, const char* dstFolder, std::string& relativePath)
{
	std::string fileStr, extensionStr;
	SplitFilePath(file, nullptr, &fileStr, &extensionStr);

	relativePath = relativePath.append(dstFolder).append("/") + fileStr.append(".") + extensionStr;
	std::string finalPath = std::string(*PHYSFS_getSearchPath()).append("/") + relativePath;

	return DuplicateFile(file, finalPath.c_str());

}

bool FileSystemManager::DuplicateFile(const char* srcFile, const char* dstFile)
{
	//TODO: Compare performance to calling Load(srcFile) and then Save(dstFile)
	/*std::ifstream src;
	src.open(srcFile, std::ios::binary);
	bool srcOpen = src.is_open();
	std::ofstream  dst(dstFile, std::ios::binary);
	bool dstOpen = dst.is_open();

	dst << src.rdbuf();

	src.close();
	dst.close();

	if (srcOpen && dstOpen)
	{
		LOG("File System: File %s Duplicated Correctly", srcFile);
		return true;
	}
	else
	{
		LOG("[ERROR] File System: Could not be duplicated");
		return false;
	}*/
	return false;
}

int close_sdl_rwops(SDL_RWops* rw)
{
	RELEASE_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

// Others
//// Save a buffer to disk
uint FileSystemManager::Save(const char* file, const void* buffer, uint size, bool append) const
{
	uint ret = 0;

	bool overwrite;

	(PHYSFS_exists(file) == 0) ? overwrite = false : overwrite = true;

	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file);

	if (fs_file != nullptr)
	{
		uint written = (uint)PHYSFS_write(fs_file, (const void*)buffer, 1, size);

		if (written != size)
		{
			LOG("[ERROR] File System: Could not write to file %s: %s", file, PHYSFS_getLastError());
		}
		else
		{
			if (append == true)
			{
				LOG("File System: Added %u data to [%s%s]", size, GetWriteDir(), file);
			}
			else if (overwrite == true)
			{
				LOG("File System: File [%s%s] overwritten with %u bytes", GetWriteDir(), file, size);
			}
			else
			{
				LOG("File System: New file created [%s%s] of %u bytes", GetWriteDir(), file, size);
			}

			ret = written;
		}

		if (PHYSFS_close(fs_file) == 0)
		{
			LOG("[ERROR] File System: Could not close file %s: %s", file, PHYSFS_getLastError());
		}
	}
	else
	{
		LOG("[ERROR] File System: Could not open file %s: %s", file, PHYSFS_getLastError());
	}

	return ret;
}

bool FileSystemManager::Remove(const char* file)
{
	bool ret = false;

	if (file != nullptr)
	{
		// If it is a directory, we need to recursively remove all the files inside
		if (IsDirectory(file))
		{
			std::vector<std::string> containedFiles, containedDirs;
			NodePath rootDirectory = GetAllFiles(file);

			for (uint i = 0; i < rootDirectory.vChildren.size(); ++i)
			{
				Remove(rootDirectory.vChildren[i].path.c_str());
			}
		}

		if (PHYSFS_delete(file) != 0)
		{
			LOG("File System: [%s] deleted", file);
			ret = true;
		}
		else
		{
			LOG("[ERROR] File System: Could not delete [%s]: %s", file, PHYSFS_getLastError());
		}
	}

	return ret;
}