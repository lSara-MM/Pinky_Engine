#ifndef __UI_H__
#define __UI_H__

#pragma once

#include "Globals.h"
#include "Component.h"

struct UIBounds
{
	float3 vertex[4];
	float2 uvs[4];

	uint* index;

	GLuint VBO; //vertex buffer object
	GLuint EBO; //element buffer object
	GLuint id_tex_uvs; //texture UVs

	GLuint textureID;
	AABB local_aabb;//local AABB

	bool InitBuffers();
	void DeleteBuffers();
};

enum class UI_STATE
{
	DISABLED,
	NORMAL,
	FOCUSED,
	PRESSED,
	RELEASE, 
	SELECTED,

	NONE
};

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
	C_UI(UI_TYPE ui_t, C_TYPE t, GameObject* g, std::string n = "UI", int w = 200, int h = 100, int x = 0, int y = 0, Color c = { 1,1,1,1 });
	~C_UI();

	virtual update_status Update(float dt);

	virtual void ShowInInspector() {};
	virtual void Draw(bool game);
	virtual void DebugDraw();

	//UI logic
	void StateLogic();
	virtual void OnNormal() {};
	virtual void OnFocused() {};
	virtual void OnPressed() {};
	virtual void OnRelease() {};
	virtual void OnSelected() {};
	bool MouseCheck(float2 mouse);

	virtual void UpdateUITransform();
	void DrawABB();
	void DrawOBB();
	void UpdateBoundingBoxes();
	void Drag(float dt);
	void FadeUI(float dt);

public:
	UI_TYPE UI_type;
	UI_STATE state;

	// Transform info
	float posX, posY, width, height;
	
	//color
	Color color;

	//Mouse pick
	AABB local_aabb;//local AABB
	AABB global_aabb;//global AABB
	OBB obb;//global OB

	bool draggable;
	bool fade;

	UIBounds* bounds;
};
#endif // __UI_H__