#pragma once
#include "Globals.h"
#include "Light.h"

#include "../Assimp/Assimp.h"
#include <vector>

#include "Component.h"

class C_Material : public Component
{
public:
	C_Material(GameObject* g = nullptr, unsigned int i = 0, bool start_enabled = true);
	~C_Material();

	void ShowInInspector();

private:
};