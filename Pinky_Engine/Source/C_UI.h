#ifndef __UI_H__
#define __UI_H__

#pragma once

#include "Globals.h"
#include "Component.h"

class C_UI : public Component
{
public:
	C_UI(C_TYPE t, GameObject* g, std::string n = "UI");
	~C_UI();
	

public:
	//SDL_Rect bounds;        // Position and size
	//SDL_Texture* texture;   // Texture atlas reference
	//SDL_Rect section;       // Texture atlas base section
};
#endif // __UI_H__