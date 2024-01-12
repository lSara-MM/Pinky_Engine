#ifndef __UI_CHECKBOX_H__
#define __UI_CHECKBOX_H__
#pragma once

#include "Globals.h"
#include "C_UI.h"

class UI_Checkbox : public C_UI
{
public:

	UI_Checkbox(GameObject* g);
	~UI_Checkbox();

	void ShowInInspector();

public:
};
#endif // __UI_CHECKBOX_H__