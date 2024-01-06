#ifndef __G_UI_H__
#define __G_UI_H__

#include "Globals.h"
#include "GameObject.h"
#include "C_TransformUI.h"

class G_UI : public GameObject
{
public:
	G_UI(std::string name = "UI");
	virtual ~G_UI();
	
public:

	C_TransformUI rectTransform;
};
#endif // __G_UI_H__
