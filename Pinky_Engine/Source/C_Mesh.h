#pragma once
#include "Globals.h"
#include "Light.h"

#include "External Libraries/Assimp/Assimp.h"

#include "Component.h"
#include "Color.h"

class C_Mesh : public Component
{
public:
	C_Mesh(GameObject* g = nullptr, ai::mesh* m = nullptr, uint i = 0, bool start_enabled = true);
	C_Mesh(GameObject* g, C_Mesh* toCopy, bool start_enabled = true, uint i = 0);
	~C_Mesh();

	void ShowInInspector();
	void Draw(bool checkered = false, Color color = { 255, 255, 255, 255});
	void DrawVertexNormals();
	void DrawFaceNormals();
	void DrawAABB();
	void DrawOBB();

public:
	ai::mesh* mesh = nullptr;
	bool showVertexNormals;
	bool showFacesNormals;
	bool showAABB;
	bool showOBB;

	AABB global_aabb;//global OB
	OBB obb;//global AABB

	//GLuint texture_checker;
};