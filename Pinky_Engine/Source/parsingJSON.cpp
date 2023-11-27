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
    std::string file_name = "Assets\\" + go->name + ".meta.json";
    root_value = json_value_init_object();
    root_object = json_value_get_object(root_value);
    char* serialized_string = NULL;

    json_object_dotset_string(root_object, "GameObject.Info", "1");
    GameObjectJSON(go);
   
    serialized_string = json_serialize_to_string_pretty(root_value);
    puts(serialized_string);

    json_serialize_to_file(root_value, file_name.c_str());
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

void ParsingJSON::GameObjectJSON(GameObject* go)
{
    json_object_set_string(root_object, "Name", go->name.c_str());
    json_object_set_number(root_object, "UID", go->GetUid());
    json_object_set_number(root_object, "Parent UID", go->pParent->GetUid());
    json_object_set_number(root_object, "Components num", go->vComponents.size());   

    for (int i = 0; i < go->vComponents.size(); i++)
    {
        ComponentsJSON(go->vComponents[i]);
    }
}

void ParsingJSON::ComponentsJSON(Component* comp)
{
    std::string comp_name = "Components." + comp->name;
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
        break;
    case C_TYPE::MATERIAL:
        std_json_object_dotset_number(root_object, comp_name + ".UUID", comp->GetUID());
        std_json_object_dotset_number(root_object, comp_name + ".Material UUID", comp->GetUID());
        break;
    case C_TYPE::CAM:
        break;
    case C_TYPE::NONE:
        break;
    default:
        break;
    }
}

void ParsingJSON::std_json_object_dotset_number(JSON_Object* root_object, std::string s, double number)
{
    json_object_dotset_number(root_object, s.c_str(), number);
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
