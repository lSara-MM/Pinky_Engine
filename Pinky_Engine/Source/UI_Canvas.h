#ifndef __UI_CANVAS_H__
#define __UI_CANVAS_H__

#include "Globals.h"
#include "G_UI.h"

class UI_Canvas : public C_UI
{
public:
	
	UI_Canvas(GameObject* g, int w = 500, int h = 500);
	~UI_Canvas();

	void ShowInInspector();
	void canvasDraw();

public:

	int canvasWidth, canvasHeight;
	//SDL_Rect bounds;        // Position and size
	//SDL_Texture* texture;   // Texture atlas reference
	//SDL_Rect section;       // Texture atlas base section
};
#endif // __UI_CANVAS_H__