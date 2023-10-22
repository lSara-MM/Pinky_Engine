#pragma once
#include "Globals.h"
#include "Light.h"

#include "../Assimp/Assimp.h"
#include <vector>

#include "Component.h"

class C_Mesh : public Component
{
public:
	C_Mesh(GameObject* g = nullptr, ai::mesh* m = nullptr, unsigned int i = 0, bool start_enabled = true);
	~C_Mesh();

	void ShowInInspector();
	void Draw(bool checkered = false);
	void DrawVertexNormals();
public:
	ai::mesh* mesh = nullptr;

	//GLuint texture_checker;
};