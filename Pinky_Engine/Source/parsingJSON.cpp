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
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);
    char* serialized_string = NULL;

    json_object_dotset_string(root_object, "GameObject.Info", "1");

   
    serialized_string = json_serialize_to_string_pretty(root_value);
    puts(serialized_string);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

void ParsingJSON::GameObjectJSON(GameObject* go, JSON_Object* root_object)
{
    json_object_set_string(root_object, "Name", go->name.c_str());
    json_object_set_number(root_object, "UID", go->GetUid());
    json_object_set_number(root_object, "Parent UID", go->pParent->GetUid());
    json_object_set_number(root_object, "Components num", go->vComponents.size());

    for (int i = 0; i < go->vComponents.size(); i++)
    {
        ComponentsJSON(go->vComponents[i], root_object);
    }
}

void ParsingJSON::ComponentsJSON(Component* comp, JSON_Object* root_object)
{
    json_object_set_string(root_object, "Components:.Name", comp->name.c_str());
    json_object_set_number(root_object, "Components:.Type", (double)comp->type);
    json_object_set_number(root_object, "Components:.UID", comp->GetUID());

    switch (comp->type)
    {
    case C_TYPE::TRANSFORM:
        json_object_set_number(root_object, "Components:.Position.X:", static_cast<C_Transform*> (comp)->position.x);
        json_object_set_number(root_object, "Components:.Position.Y:", static_cast<C_Transform*> (comp)->position.y);
        json_object_set_number(root_object, "Components:.Position.Z:", static_cast<C_Transform*> (comp)->position.z);


        json_object_set_number(root_object, "Components:.Rotation.", static_cast<C_Transform*> (comp)->rotation.x);
        json_object_set_number(root_object, "Components:.Rotation.", static_cast<C_Transform*> (comp)->rotation.y);
        json_object_set_number(root_object, "Components:.Rotation.", static_cast<C_Transform*> (comp)->rotation.z);
        json_object_set_number(root_object, "Components:.Rotation.", static_cast<C_Transform*> (comp)->rotation.w);

        json_object_set_number(root_object, "Components:.Scale.X:", static_cast<C_Transform*> (comp)->scale.x);
        json_object_set_number(root_object, "Components:.Scale.Y:", static_cast<C_Transform*> (comp)->scale.y);
        json_object_set_number(root_object, "Components:.Scale.Z:", static_cast<C_Transform*> (comp)->scale.z);
        break;
    case C_TYPE::MESH:
        break;
    case C_TYPE::MATERIAL:
        break;
    case C_TYPE::CAM:
        break;
    case C_TYPE::NONE:
        break;
    default:
        break;
    }
}
