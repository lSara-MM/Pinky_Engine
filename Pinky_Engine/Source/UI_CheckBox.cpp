#include "UI_Checkbox.h"

UI_Checkbox::UI_Checkbox(GameObject* g) : C_UI(UI_TYPE::BUTTON, C_TYPE::UI, g, "Checkbox")
{
	isChecked = true;
}

UI_Checkbox::~UI_Checkbox()
{
}

void UI_Checkbox::ShowInInspector()
{
}
