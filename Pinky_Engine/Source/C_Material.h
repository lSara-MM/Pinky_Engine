#pragma once
#include "Globals.h"
#include "Light.h"

#include "External Libraries/Assimp/Assimp.h"

#include "Component.h"
#include "Color.h"

class C_Material : public Component
{
public:
	C_Material(GameObject* g = nullptr, ai::texture* t = nullptr, uint i = 0, bool start_enabled = true);	
	C_Material(GameObject* g, C_Material* toCopy, bool start_enabled, uint i = 0);
	~C_Material();

	void ShowInInspector();

public: 
	ai::texture* tex;
	bool checkered;

	Color color{};
};