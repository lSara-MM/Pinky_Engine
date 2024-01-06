#ifndef __UI_IMAGE_H__
#define __UI_IMAGE_H__
#pragma once

#include "Globals.h"
#include "C_UI.h"
#include "C_Material.h"

class UI_Image : public C_UI
{
public:

	UI_Image(GameObject* g);
	~UI_Image();

public:
	// TODO: que te una img? no es com una textura tal cual?
	C_Material* mat;
};
#endif // __UI_IMAGE_H__