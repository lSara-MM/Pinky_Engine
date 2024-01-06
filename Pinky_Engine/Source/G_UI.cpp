#include "G_UI.h"

G_UI::G_UI(std::string name) : GameObject(name)
{
	RemoveComponent(transform);
}

G_UI::~G_UI()
{
	int a = 2;
}