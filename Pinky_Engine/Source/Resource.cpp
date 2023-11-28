#include "Resource.h"
#include "Application.h"

Resource::Resource(R_TYPE t) : type (t)
{
	uid = App->randomLCG->Int();
}

Resource::~Resource()
{
}

int Resource::GetUID() { return uid; }

R_TYPE Resource::GetType() { return type; }
