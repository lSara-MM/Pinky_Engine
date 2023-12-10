#pragma once
#include "Globals.h"
#include "Light.h"

#include "External Libraries/Assimp/Assimp.h"

#include "Component.h"
#include "Color.h"

#include "R_Mesh.h"

class C_Mesh : public Component
{
public:
	// TODO: Clean this mess
	C_Mesh(GameObject* g = nullptr, bool a = true);
	C_Mesh(GameObject* g, R_Mesh* m = nullptr, bool start_enabled = true);
	C_Mesh(GameObject* g, C_Mesh* toCopy, bool start_enabled = true);
	~C_Mesh();

	void ShowInInspector();
	void Draw(bool checkered = false, Color color = { 255, 255, 255, 255});
	void DrawVertexNormals();
	void DrawFaceNormals();
	void DrawAABB();
	void DrawOBB();

public:
	R_Mesh* mesh;
	bool showVertexNormals;
	bool showFacesNormals;
	bool showAABB;
	bool showOBB;

	AABB global_aabb;//global OB
	OBB obb;//global AABB

	//GLuint texture_checker;
};