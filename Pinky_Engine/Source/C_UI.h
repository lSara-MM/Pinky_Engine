#pragma once

#include "Globals.h"
#include "Component.h"

class C_UI : public Component
{
public:
	C_UI();
	C_UI(GameObject* g, float3 pos, Quat rot, float3 sc, bool start_enabled = true);
	C_UI(GameObject* g, C_UI* toCopy);
	~C_UI();
	

public:

	UINT32 id;

	//SDL_Rect bounds;        // Position and size
	//SDL_Texture* texture;   // Texture atlas reference
	//SDL_Rect section;       // Texture atlas base section
};
