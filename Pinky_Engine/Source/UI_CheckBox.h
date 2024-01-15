#ifndef __UI_CHECKBOX_H__
#define __UI_CHECKBOX_H__
#pragma once

#include "Globals.h"
#include "C_UI.h"
#include "UI_Image.h"

class UI_Checkbox : public C_UI
{
public:

	UI_Checkbox(GameObject* g);
	~UI_Checkbox();

	void ShowInInspector();
	void Draw(bool game) override {};

	void OnNormal() override;
	void OnFocused() override;
	void OnPressed() override;
	void OnSelected() override;
	void OnRelease() override;

public:
	bool isInteractable;
	bool isChecked;

	UI_Image* bgImg;
	UI_Image* cmImg;

private:
	//color
	Color focusedColor;
	Color pressedColor;
	Color selectedColor;
	Color disabledColor;
};
#endif // __UI_CHECKBOX_H__