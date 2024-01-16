#ifndef __UI_CHECKBOX_H__
#define __UI_CHECKBOX_H__
#pragma once

#include "Globals.h"
#include "C_UI.h"

#include "UI_Image.h"
#include "UI_Text.h"

class UI_Checkbox : public C_UI
{
public:
	// x = 0, y = 0, w = 170, y = 20
	UI_Checkbox(GameObject* g, int x = 0, int y = 0, int w = 170, int h = 20);
	~UI_Checkbox();

	void ShowInInspector();
	void Draw(bool game) override {};

	void OnNormal() override;
	void OnFocused() override;
	void OnPressed() override;
	void OnSelected() override;
	void OnRelease() override;

	void CheckVSync();

public:
	bool isInteractable;
	bool isChecked;

	UI_Image* bgImg;
	UI_Image* cmImg;

	std::string text;
	UI_Text* displayText;

	// TODO: fix this
	bool defaultFunction1;
	bool defaultFunction2;
private:
	//color
	Color focusedColor;
	Color pressedColor;
	Color selectedColor;
	Color disabledColor;
};
#endif // __UI_CHECKBOX_H__