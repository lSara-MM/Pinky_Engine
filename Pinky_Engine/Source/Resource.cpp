#include "Resource.h"
#include "Application.h"

Resource::Resource(R_TYPE t) : type (t)
{
	uid = App->randomLCG->Int();
	count = 0;
}

Resource::~Resource()
{
}

int Resource::GetUID() { return uid; }

R_TYPE Resource::GetType() { return type; }
