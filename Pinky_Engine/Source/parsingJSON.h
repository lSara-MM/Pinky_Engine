#pragma once
#include "Globals.h"
#include <string>

#include "External libraries/parson/parson.h"

using namespace std;

class GameObject;
class Component;

class ParsingJSON
{
public:
	ParsingJSON();
	~ParsingJSON();

	void CreateGOMetaFile(GameObject* go);

	void GameObjectJSON(GameObject* go);
	void ComponentsJSON(Component* comp);

private:
	void std_json_object_dotset_number(JSON_Object* root_object, std::string s, double number);
	void json_object_dotset_array(float* num, int size, string name);

private:
	JSON_Value* root_value;
	JSON_Object* root_object;
};