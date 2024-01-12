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
	void RgenerateVAO();
	void DeleteBuffers();
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
	C_UI(C_TYPE t, GameObject* g, std::string n = "UI", Color c = {1,1,1,1}, int w = 20, int h = 20, int x = 0, int y = 0);
	~C_UI();

	virtual void ShowInInspector() {};
	virtual void Draw();
	virtual void DebugDraw();
	void DrawABB();
	void DrawOBB();

public:
	UI_TYPE type;

	// Transform info
	int posX, posY, width, height;
	
	//color
	Color color;

	//Mouse pick
	AABB local_aabb;//local AABB
	AABB global_aabb;//global AABB
	OBB obb;//global OB
	float entryDist;
	float exitDist;
	LineSegment rayCam;
	std::map<float, C_UI*> intersects;
	LineSegment localrayCam;

	UIBounds* bounds;

};
#endif // __UI_H__