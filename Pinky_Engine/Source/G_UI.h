#ifndef __G_UI_H__
#define __G_UI_H__

#include "Globals.h"
#include "GameObject.h"
#include "C_UI.h"
#include "C_TransformUI.h"

#include "ModuleScene.h"

class UI_Canvas;

class G_UI : public GameObject
{
public:
	G_UI(UI_TYPE t, GameObject* pParent = App->scene->rootNode, int x = 0, int y = 0);
	~G_UI();

	update_status Update(float dt);
	C_UI* GetComponentUI(UI_TYPE type);
	std::vector<C_UI*> GetComponentsUI_ByType(UI_TYPE type);
	//std::vector<C_UI*> GetComponentsUI_ByType(UI_TYPE type);

	bool AddUIComponent(UI_TYPE type, int x, int y, GameObject* parent = App->scene->rootNode);

public:
	UI_Canvas* canvas;
	C_TransformUI rectTransform;
};
#endif // __G_UI_H__