#ifndef __UI_INPUTBOX_H__
#define __UI_INPUTBOX_H__
#pragma once

#include "Globals.h"
#include "C_UI.h"
#include "UI_Text.h"

class UI_InputBox : public C_UI
{
public:

	UI_InputBox(GameObject* g);
	~UI_InputBox();

	void ShowInInspector();

public:
	std::string text;
	float fontSize;

	int maxChars;

	UI_Text* displayText;

private:
	//color
	Color disabledColor;
};
#endif // __UI_INPUTBOX_H__