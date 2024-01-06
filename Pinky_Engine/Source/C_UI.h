#ifndef __UI_H__
#define __UI_H__

#pragma once

#include "Globals.h"
#include "Component.h"

enum class UI_TYPE
{
	CANVAS,
	IMAGE,
	TEXT,
	BUTTON,
	INPUTBOX,

	CHECKBOX,

	NONE
};

class C_UI : public Component
{
public:
	C_UI(C_TYPE t, GameObject* g, std::string n = "UI");
	~C_UI();
	
public:
	UI_TYPE type;
};
#endif // __UI_H__