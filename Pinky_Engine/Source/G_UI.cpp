#include "G_UI.h"
#include "UI_Canvas.h"
#include "UI_Image.h"
//#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_InputBox.h"

#include "ModuleScene.h"

G_UI::G_UI(UI_TYPE t) : GameObject()
{
	//RemoveComponent(transform);//TODO: fer amb altre transform

	AddUIComponent(t);
}

G_UI::~G_UI()
{
}

update_status G_UI::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	if (isActive)
	{
		if (!vChildren.empty())
		{
			for (auto i = 0; i < vChildren.size(); i++)
			{
				vChildren[i]->Update(dt);
			}
		}

		if (transform != nullptr)
		{
			if (transform->dirty_)
			{
				transform->UpdateTransformsChilds();
			}
		}

		if (vComponents.size() > 1)
		{
			
		}
	}

	return ret;
}

//std::vector<UI_TYPE*> G_UI::GetComponents(UI_TYPE type)
//{
//	//std::vector<type> vec = {};
//	//for (auto i = 0; i < vComponents.size(); i++)
//	//{
//	//	if (vComponents[i]->type == C_TYPE::MATERIAL)
//	//	{
//	//		vec.push_back((C_Material*)vComponents[i]);
//	//	}
//	//}
//	return vec;
//}

bool G_UI::AddUIComponent(UI_TYPE type)
{
	switch (type)
	{
	case UI_TYPE::CANVAS:
	{
		UI_Canvas* comp = new UI_Canvas(this);
		vComponents.push_back(comp);

		name = "Canvas";

		App->scene->SetCanvas(*this);
	}
	break;
	case UI_TYPE::IMAGE:
	{
		UI_Image* comp = new UI_Image(this);
		vComponents.push_back(comp);

		name = "Image";

		if (App->scene->GetCanvas() == nullptr)
		{
			App->scene->SetCanvas(*(new G_UI(UI_TYPE::CANVAS)));
		}
		ReParent(App->scene->GetCanvas());
	}
	break;
	case UI_TYPE::TEXT:
	{


		name = "Text";

		if (App->scene->GetCanvas() == nullptr)
		{
			App->scene->SetCanvas(*(new G_UI(UI_TYPE::CANVAS)));
		}
		ReParent(App->scene->GetCanvas());
	}
		break;
	case UI_TYPE::BUTTON:
	{
		UI_Button* comp = new UI_Button(this);
		vComponents.push_back(comp);

		name = "Button";

		if (App->scene->GetCanvas() == nullptr)
		{
			App->scene->SetCanvas(*(new G_UI(UI_TYPE::CANVAS)));
		}
		ReParent(App->scene->GetCanvas());
	}
		break;
	case UI_TYPE::INPUTBOX:
	{
		UI_Canvas* comp = new UI_Canvas(this);
		vComponents.push_back(comp);

		name = "Input Box";

		if (App->scene->GetCanvas() == nullptr)
		{
			App->scene->SetCanvas(*(new G_UI(UI_TYPE::CANVAS)));
		}
		ReParent(App->scene->GetCanvas());
	}
		break;
	case UI_TYPE::CHECKBOX:
	{
		UI_Canvas* comp = new UI_Canvas(this);
		vComponents.push_back(comp);

		name = "Checkbox";

		if (App->scene->GetCanvas() == nullptr)
		{
			App->scene->SetCanvas(*(new G_UI(UI_TYPE::CANVAS)));
		}
		ReParent(App->scene->GetCanvas());
	}
		break;
	case UI_TYPE::NONE:
		break;
	default:
		break;
	}

	return true;
}
