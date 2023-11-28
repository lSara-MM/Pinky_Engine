#include "parsingJSON.h"

#include "Globals.h"
#include "GameObject.h"

ParsingJSON::ParsingJSON()
{
}

ParsingJSON::~ParsingJSON()
{
}

void ParsingJSON::CreateGOMetaFile(GameObject* go)
{
    int resource_count = 0;

    std::string file_name = "Assets\\" + go->name + ".meta.json";
    root_value = json_value_init_object();
    root_object = json_value_get_object(root_value);
    char* serialized_string = NULL;

    std::string node_name = "GameObject.Info";
    std_json_object_dotset_number(root_object, node_name + ".Children number", resource_count);
    std_json_object_dotset_string(root_object, node_name + ".Local Directory", file_name);

    resource_count = GameObjectInfo(go, node_name + ".Resources") - 1;

    std_json_object_dotset_number(root_object, node_name + ".Children number", resource_count);

    GameObjectJSON(go, "GameObject.Parent");
   
    serialized_string = json_serialize_to_string_pretty(root_value);
    puts(serialized_string);

    json_serialize_to_file(root_value, file_name.c_str());
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

int ParsingJSON::GameObjectJSON(GameObject* go, std::string name, int counter)
{
    counter++;
    std_json_object_dotset_string(root_object, name + ".Name", go->name.c_str());
    std_json_object_dotset_number(root_object, name + ".UID", go->GetUid());
    std_json_object_dotset_number(root_object, name + ".Parent UID", go->pParent->GetUid());
    std_json_object_dotset_boolean(root_object, name + ".Active", go->isActive);
    std_json_object_dotset_boolean(root_object, name + ".Static", go->isStatic);
    std_json_object_dotset_number(root_object, name + ".Parent UID", go->pParent->GetUid());
    std_json_object_dotset_number(root_object, name + ".Components num", go->vComponents.size());

    for (int i = 0; i < go->vComponents.size(); i++)
    {
        ComponentsJSON(go->vComponents[i], name);
    }

    for (int i = 0; i < go->vChildren.size(); i++)
    {
        counter = GameObjectJSON(go->vChildren[i], "GameObject.Child " + std::to_string(counter), counter);
    }

    return counter;
}

int ParsingJSON::GameObjectInfo(GameObject* go, std::string name, int counter)
{
    counter++;

    std_json_object_dotset_string(root_object, name + ".Name", go->name.c_str());
    std_json_object_dotset_number(root_object, name + ".UUID", go->GetUid());

    for (int i = 0; i < go->vChildren.size(); i++)
    {
        counter = GameObjectInfo(go->vChildren[i], name, counter);
    }

    return counter;
}

void ParsingJSON::ComponentsJSON(Component* comp, std::string name)
{
    std::string comp_name = name + ".Components." + comp->name;
    std_json_object_dotset_number(root_object, comp_name + ".Type", (double)comp->type);
    std_json_object_dotset_number(root_object, comp_name + ".UUID", comp->GetUID());

    switch (comp->type)
    {
    case C_TYPE::TRANSFORM:
        json_object_dotset_array(&static_cast<C_Transform*> (comp)->position[0], 3, comp_name + ".Position");
        json_object_dotset_array(&static_cast<C_Transform*> (comp)->eulerRot[0], 3, comp_name + ".Rotation");
        json_object_dotset_array(&static_cast<C_Transform*> (comp)->scale[0], 3, comp_name + ".Scale");
        break;
    case C_TYPE::MESH:
        std_json_object_dotset_number(root_object, comp_name + ".UUID", comp->GetUID());
        std_json_object_dotset_number(root_object, comp_name + ".Mesh UUID", comp->GetUID());
        std_json_object_dotset_string(root_object, comp_name + ".Library dir", "Libray\/Meshes\/" + std::to_string(comp->GetUID()));
        break;
    case C_TYPE::MATERIAL:
        std_json_object_dotset_number(root_object, comp_name + ".UUID", comp->GetUID());
        std_json_object_dotset_number(root_object, comp_name + ".Material UUID", comp->GetUID());
        std_json_object_dotset_string(root_object, comp_name + ".Library dir", "Libray\/Textures\/" + std::to_string(comp->GetUID()));
        break;
    case C_TYPE::CAM:
        break;
    case C_TYPE::NONE:
        break;
    default:
        break;
    }
}

// Private
// The same as the json but accepts string as name instead of const char*
void ParsingJSON::std_json_object_dotset_string(JSON_Object* root_object, std::string s, std::string string)
{
    json_object_dotset_string(root_object, s.c_str(), string.c_str());
}

void ParsingJSON::std_json_object_dotset_number(JSON_Object* root_object, std::string s, double number)
{
    json_object_dotset_number(root_object, s.c_str(), number);
}

void ParsingJSON::std_json_object_dotset_boolean(JSON_Object* root_object, std::string s, bool boolean)
{
    json_object_dotset_boolean(root_object, s.c_str(), boolean);
}

void ParsingJSON::json_object_dotset_array(float* num, int size, std::string name)
{
    JSON_Value* value = json_value_init_array();
    JSON_Array* jArray = json_value_get_array(value);

    json_object_dotset_value(root_object, name.c_str(), value);

    for (int i = 0; i < size; i++)
    {
        json_array_append_number(jArray, num[i]);
    }
}
