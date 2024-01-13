#ifndef __UI_BUTTON_H__
#define __UI_BUTTON_H__
#pragma once

#include "Globals.h"
#include "C_UI.h"
#include "C_Material.h"

class UI_Button : public C_UI
{
public:

	UI_Button(GameObject* g);
	~UI_Button();

	void ShowInInspector();

public:
	bool isInteractable;

private:
	//color
	Color normalColor;
	Color focusedColor;
	Color pressedColor;
	Color selectedColor;
	Color disabledColor;
};
#endif // __UI_BUTTON_H__