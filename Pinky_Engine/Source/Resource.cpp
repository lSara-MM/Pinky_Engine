#include "Resource.h"
#include "Application.h"
#include "ModuleResource.h"

Resource::Resource(R_TYPE t) : type (t)
{
	uid = App->randomLCG->Int();
	count = 0;
	name = std::to_string(GetUID());
}

Resource::~Resource()
{
}

void Resource::CleanUp()
{
}

int Resource::GetUID() { return uid; }

void Resource::SetUID(u32 id) { uid = id; }

R_TYPE Resource::GetType() { return type; }