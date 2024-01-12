#include "G_UI.h"
#include "UI_Canvas.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "UI_Button.h"
#include "UI_InputBox.h"

#include "ModuleScene.h"

G_UI::G_UI(UI_TYPE t, GameObject* pParent) : GameObject("", pParent)
{
	//RemoveComponent(transform);//TODO: fer amb altre transform
	canvas = nullptr;
	AddUIComponent(t);

	//AddComponent(C_TYPE::MESH, nullptr, ai::POLY_PRIMITIVE_TYPE::PLANE);
	mesh = nullptr;
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
				canvas->UpdateUITransform();
				canvas->UpdateBoundingBoxes();
			}
		}

		if (vComponents.size() > 1)
		{
			if (canvas != nullptr)
			{
				//canvas->Draw();
				//canvas->DrawABB();
				//canvas->DrawOBB();
				//canvas->DebugDraw();
			}
		}
	}

	return ret;
}

C_UI* G_UI::GetComponentUI(UI_TYPE type)
{
	C_UI* comp = nullptr;

	for (auto i = 0; i < vComponents.size(); i++)
	{
		if (vComponents[i]->type == C_TYPE::UI && static_cast<C_UI*>(vComponents[i])->UI_type == type)
		{
			return comp = static_cast<C_UI*>(vComponents[i]);
		}
	}
}

bool G_UI::AddUIComponent(UI_TYPE type)
{
	bool ret = true;

	switch (type)
	{
	case UI_TYPE::CANVAS:
	{
		if (canvas == nullptr)
		{
			UI_Canvas* comp = new UI_Canvas(this);
			vComponents.push_back(comp);

			canvas = comp;
			App->scene->SetCanvas(this);
		}

		name = "Canvas";
	}
	break;
	case UI_TYPE::IMAGE:
	{
		UI_Image* comp = new UI_Image(this);
		vComponents.push_back(comp);

		name = "Image";

		if (App->scene->GetCanvas() == nullptr)
		{
			App->scene->SetCanvas(new G_UI(UI_TYPE::CANVAS));
		}
		ReParent(App->scene->GetCanvas());
		canvas = static_cast<G_UI*>(pParent)->canvas;
	}
	break;
	case UI_TYPE::TEXT:
	{
		UI_Text* comp = new UI_Text(this);
		vComponents.push_back(comp);
		name = "Text";

		if (App->scene->GetCanvas() == nullptr)
		{
			App->scene->SetCanvas(new G_UI(UI_TYPE::CANVAS));
		}
		ReParent(App->scene->GetCanvas());
		canvas = static_cast<G_UI*>(pParent)->canvas;
	}
		break;
	case UI_TYPE::BUTTON:
	{
		UI_Button* comp = new UI_Button(this);
		vComponents.push_back(comp);

		name = "Button";

		if (App->scene->GetCanvas() == nullptr)
		{
			App->scene->SetCanvas(new G_UI(UI_TYPE::CANVAS));
		}
		ReParent(App->scene->GetCanvas());
		canvas = static_cast<G_UI*>(pParent)->canvas;
	}
		break;
	case UI_TYPE::INPUTBOX:
	{
		// Unity-like
		AddUIComponent(UI_TYPE::IMAGE);
		G_UI* aux = new G_UI(UI_TYPE::TEXT, this);
		aux->ReParent(this);

		//new G_UI(UI_TYPE::IMAGE, this);

		//AddUIComponent(UI_TYPE::IMAGE);
		//AddUIComponent(UI_TYPE::TEXT);

		UI_InputBox* comp = new UI_InputBox(this);
		vComponents.push_back(comp);
		comp->displayText = static_cast<UI_Text*>(aux->GetComponentUI(UI_TYPE::TEXT));

		name = "Input Box";

		if (App->scene->GetCanvas() == nullptr)
		{
			App->scene->SetCanvas(new G_UI(UI_TYPE::CANVAS));
		}
		ReParent(App->scene->GetCanvas());
		canvas = static_cast<G_UI*>(pParent)->canvas;
	}
		break;
	case UI_TYPE::CHECKBOX:
	{
		UI_Canvas* comp = new UI_Canvas(this);
		vComponents.push_back(comp);

		name = "Checkbox";

		if (App->scene->GetCanvas() == nullptr)
		{
			App->scene->SetCanvas(new G_UI(UI_TYPE::CANVAS));
		}
		ReParent(App->scene->GetCanvas());
		canvas = static_cast<G_UI*>(pParent)->canvas;
	}
		break;
	case UI_TYPE::NONE:
		break;
	default:
		break;
	}

	return ret;
}