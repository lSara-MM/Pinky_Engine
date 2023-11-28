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

	int GameObjectJSON(GameObject* go, std::string name, int counter = 0);
	int GameObjectInfo(GameObject* go, std::string name, int counter = 0);
	void ComponentsJSON(Component* comp, std::string name);

private:
	void std_json_object_dotset_string(JSON_Object* root_object, std::string s, std::string string);
	void std_json_object_dotset_number(JSON_Object* root_object, std::string s, double number);
	void std_json_object_dotset_boolean(JSON_Object* root_object, std::string s, bool boolean);

	void json_object_dotset_array(float* num, int size, string name);

private:
	JSON_Value* root_value;
	JSON_Object* root_object;
};