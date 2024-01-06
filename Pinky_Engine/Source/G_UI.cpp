#include "G_UI.h"
#include "UI_Canvas.h"
#include "UI_Image.h"
//#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_InputBox.h"


G_UI::G_UI(UI_TYPE t) : GameObject()
{
	RemoveComponent(transform);

	AddUIComponent(t);
}

G_UI::~G_UI()
{
}

update_status G_UI::Update()
{
	return UPDATE_CONTINUE;
}

bool G_UI::AddUIComponent(UI_TYPE type)
{
	switch (type)
	{
	case UI_TYPE::CANVAS:
	{
		UI_Canvas* comp = new UI_Canvas(this);
		vComponents.push_back(comp);

		name = "Canvas";
	}
	break;
	case UI_TYPE::IMAGE:
	{
		UI_Image* comp = new UI_Image(this);
		vComponents.push_back(comp);

		name = "Image";
	}
	break;
	case UI_TYPE::TEXT:
	{


		name = "Text";
	}
		break;
	case UI_TYPE::BUTTON:
	{
		UI_Canvas* comp = new UI_Canvas(this);
		vComponents.push_back(comp);

		name = "Button";
	}
		break;
	case UI_TYPE::INPUTBOX:
	{
		UI_Canvas* comp = new UI_Canvas(this);
		vComponents.push_back(comp);

		name = "Input Box";
	}
		break;
	case UI_TYPE::CHECKBOX:
	{
		UI_Canvas* comp = new UI_Canvas(this);
		vComponents.push_back(comp);

		name = "Checkbox";
	}
		break;
	case UI_TYPE::NONE:
		break;
	default:
		break;
	}

	return true;
}
