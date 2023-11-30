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
    json_object_dotset_number(root_object, (node_name + ".Children number").c_str(), resource_count);
    json_object_dotset_string(root_object, (node_name + ".Local Directory").c_str(), file_name.c_str());

    resource_count = GameObjectInfo(go, node_name + ".Resources") - 1;

    json_object_dotset_number(root_object, (node_name + ".Children number").c_str(), resource_count);

    GameObjectJSON(go, "GameObject.Parent");
   
    serialized_string = json_serialize_to_string_pretty(root_value);
    puts(serialized_string);

    // TODO: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAaa
    json_serialize_to_file(root_value, file_name.c_str());
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

int ParsingJSON::GameObjectJSON(GameObject* go, std::string name, int counter)
{
    counter++;
    json_object_dotset_string(root_object, (name + ".Name").c_str(), go->name.c_str());
    json_object_dotset_number(root_object, (name + ".UID").c_str(), go->GetUid());
    json_object_dotset_number(root_object, (name + ".Parent UID").c_str(), go->pParent->GetUid());
    json_object_dotset_boolean(root_object, (name + ".Active").c_str(), go->isActive);
    json_object_dotset_boolean(root_object, (name + ".Static").c_str(), go->isStatic);
    json_object_dotset_number(root_object, (name + ".Parent UID").c_str(), go->pParent->GetUid());
    json_object_dotset_number(root_object, (name + ".Components num").c_str(), go->vComponents.size());

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

    json_object_dotset_string(root_object, (name + ".Name").c_str(), go->name.c_str());
    json_object_dotset_number(root_object, (name + ".UUID").c_str(), go->GetUid());

    for (int i = 0; i < go->vChildren.size(); i++)
    {
        counter = GameObjectInfo(go->vChildren[i], name, counter);
    }

    return counter;
}

void ParsingJSON::ComponentsJSON(Component* comp, std::string name)
{
    std::string comp_name = name + ".Components." + comp->name;
    json_object_dotset_number(root_object, (comp_name + ".Type").c_str(), (double)comp->type);
    json_object_dotset_number(root_object, (comp_name + ".UUID").c_str(), comp->GetUID());

    switch (comp->type)
    {
    case C_TYPE::TRANSFORM:
        json_object_dotset_array(&static_cast<C_Transform*> (comp)->position[0], 3, comp_name + ".Position");
        json_object_dotset_array(&static_cast<C_Transform*> (comp)->eulerRot[0], 3, comp_name + ".Rotation");
        json_object_dotset_array(&static_cast<C_Transform*> (comp)->scale[0], 3, comp_name + ".Scale");
        break;
    case C_TYPE::MESH:
        json_object_dotset_number(root_object, (comp_name + ".UUID").c_str(), comp->GetUID());
        json_object_dotset_number(root_object, (comp_name + ".Mesh UUID").c_str(), comp->GetUID());
        json_object_dotset_string(root_object, (comp_name + ".Library dir").c_str(), ("Libray\/Meshes\/" + std::to_string(comp->GetUID())).c_str());
        break;
    case C_TYPE::MATERIAL:
        json_object_dotset_number(root_object, (comp_name + ".UUID").c_str(), comp->GetUID());
        json_object_dotset_number(root_object, (comp_name + ".Material UUID").c_str(), comp->GetUID());
        json_object_dotset_string(root_object, (comp_name + ".Library dir").c_str(), ("Libray\/Textures\/" + std::to_string(comp->GetUID())).c_str());
        break;
    case C_TYPE::CAM:
        break;
    case C_TYPE::NONE:
        break;
    default:
        break;
    }
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
