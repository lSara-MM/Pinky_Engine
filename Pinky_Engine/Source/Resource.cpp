#include "Resource.h"
#include "Application.h"

Resource::Resource()
{
	uid = App->randomLCG->Int();
}

Resource::~Resource()
{
}

int Resource::GetUID() { return uid; }

R_TYPE Resource::GetType() { return type; }
