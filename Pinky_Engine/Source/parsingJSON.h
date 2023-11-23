#pragma once
#include "Module.h"
#include "Globals.h"

#include "GameObject.h"

#include "External libraries/parson/parson.h"

class ParsingJSON
{
public:
	ParsingJSON();
	~ParsingJSON();

	void CreateGOMetaFile(GameObject* go);

	void GameObjectJSON(GameObject* go, JSON_Object* root_object);
	void ComponentsJSON(Component* comp, JSON_Object* root_object);
};