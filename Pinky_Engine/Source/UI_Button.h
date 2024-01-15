#ifndef __UI_BUTTON_H__
#define __UI_BUTTON_H__
#pragma once

#include "Globals.h"
#include "C_UI.h"
#include "UI_Image.h"

class UI_Button : public C_UI
{
public:

	UI_Button(GameObject* g, int w, int h, int x, int y);
	~UI_Button();

	void ShowInInspector();

	void OnNormal() override;
	void OnFocused() override;
	void OnPressed() override;
	void OnSelected() override;

public:
	bool isInteractable;
	UI_Image* image;

private:
	//color
	Color focusedColor;
	Color pressedColor;
	Color selectedColor;
	Color disabledColor;
};
#endif // __UI_BUTTON_H__