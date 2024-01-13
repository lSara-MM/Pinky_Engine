#ifndef __UI_TEXT_H__
#define __UI_TEXT_H__
#pragma once

#include "Globals.h"
#include "C_UI.h"
#include "C_Material.h"

class UI_Text : public C_UI
{
public:

	UI_Text(GameObject* g);
	~UI_Text();

	void ShowInInspector();

	void Draw(bool game) override;

public:
	std::string text;
	float fontSize;
};
#endif // __UI_TEXT_H__