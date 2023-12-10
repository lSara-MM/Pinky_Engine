#pragma once
#include "Globals.h"

#include "Component.h"

enum class R_TYPE
{
	MESH,
	TEXTURE,
	PREFAB,
	SCENE,

	NONE
};

class GameObject;

class Resource
{
public:
	Resource(R_TYPE t);
	~Resource();

	void CleanUp();

	int GetUID();
	void SetUID(u32 id);

	R_TYPE GetType();

public:
	std::string name;
	std::string assetsFile;
	std::string libraryFile;

	uint count;
	R_TYPE type;

	std::vector<Component*> vComponents;
protected:
	u32 uid;
};