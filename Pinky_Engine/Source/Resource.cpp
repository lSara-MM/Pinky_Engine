#include "Resource.h"
#include "Application.h"
#include "ModuleResource.h"

Resource::Resource(R_TYPE t) : type (t)
{
	uid = App->randomLCG->Int();
	count = 0;
	name = std::to_string(GetUID());

	//App->resource->mResources[GetUID()] = *this;
}

Resource::~Resource()
{
}

int Resource::GetUID() { return uid; }

void Resource::SetUID(u32 id) { uid = id; }

R_TYPE Resource::GetType() { return type; }