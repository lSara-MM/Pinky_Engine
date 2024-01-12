#include "UI_Button.h"

UI_Button::UI_Button(GameObject* g) : C_UI(UI_TYPE::BUTTON, C_TYPE::UI, g, "Button")
{
}

UI_Button::~UI_Button()
{
	RELEASE(mat);
}

void UI_Button::ShowInInspector()
{
}