#ifndef __G_UI_H__
#define __G_UI_H__

#include "Globals.h"
#include "GameObject.h"
#include "C_UI.h"
#include "C_TransformUI.h"

class UI_Canvas;

class G_UI : public GameObject
{
public:
	G_UI(UI_TYPE t);
	~G_UI();

	update_status Update(float dt);
	C_UI* GetComponentUI(UI_TYPE type);

	bool AddUIComponent(UI_TYPE type);

public:
	C_TransformUI rectTransform;
	UI_Canvas* canvas;
};
#endif // __G_UI_H__