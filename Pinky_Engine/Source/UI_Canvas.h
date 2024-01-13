#ifndef __UI_CANVAS_H__
#define __UI_CANVAS_H__

#include "Globals.h"
#include "G_UI.h"

class UI_Canvas : public C_UI
{
public:
	
	UI_Canvas(GameObject* g, int w = 100, int h = 100);
	~UI_Canvas();

	void ShowInInspector();
	void Draw(bool game) override;
	void UpdateUITransform() override;

public:

	int canvasWidth, canvasHeight;
	//SDL_Rect bounds;        // Position and size
	//SDL_Texture* texture;   // Texture atlas reference
	//SDL_Rect section;       // Texture atlas base section
};
#endif // __UI_CANVAS_H__