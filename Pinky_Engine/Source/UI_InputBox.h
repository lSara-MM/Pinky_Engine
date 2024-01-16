#ifndef __UI_INPUTBOX_H__
#define __UI_INPUTBOX_H__
#pragma once

#include "Globals.h"
#include "C_UI.h"
#include "UI_Text.h"

class UI_InputBox : public C_UI
{
public:
	// x = 0, y = 0, w = 170, y = 30
	UI_InputBox(GameObject* g, int x = 0, int y = 0, int w = 170, int h = 30);
	~UI_InputBox();

	update_status Update(float dt) override;

	void ShowInInspector();

	void OnSelected();

public:
	std::string text;
	float fontSize;

	int maxChars;
	UI_Text* displayText;

private:
	//color
	Color disabledColor;
	bool isWriting;
};
#endif // __UI_INPUTBOX_H__