#ifndef __UI_BUTTON_H__
#define __UI_BUTTON_H__
#pragma once

#include "Globals.h"
#include "C_UI.h"

#include "UI_Image.h"
#include "UI_Text.h"

class UI_Button : public C_UI
{
public:
	// x = 0, y = 0, w = 300, y = 50
	UI_Button(GameObject* g, int x = 0, int y = 0, int w = 300, int h = 50);
	~UI_Button();

	void ShowInInspector();

	void OnNormal();
	void OnFocused();
	void OnPressed();
	void OnSelected();
	void OnRelease();

public:
	bool isInteractable;
	UI_Image* image;

	UI_Text* displayText;

	// TODO: fix this
	bool defaultFunction;

private:
	//color
	Color focusedColor;
	Color pressedColor;
	Color selectedColor;
	Color disabledColor;
};
#endif // __UI_BUTTON_H__