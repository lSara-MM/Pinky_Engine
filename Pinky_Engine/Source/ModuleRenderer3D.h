#pragma once
#include "../Assimp/Assimp.h"

#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"

#include "../MathGeoLib/include/Math/float3x3.h"
#include "../MathGeoLib/include/Math/float4x4.h"
#include "Primitive.h"

#include "../mmgr/mmgr.h"

#include "../Glew/include/glew.h"


//todo: REMOVE this before 2nd delivery!!
#include "glmath.h"

#define MAX_LIGHTS 8
#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);
	void DrawBox();
	void SetVsync(bool enable);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	CPlane Grid;
	
	//You won't need this after using Frustum
	mat4x4 ProjectionMatrix;

	bool wireframe;
	bool VertexNormals;
	bool FaceNormals;
	bool Vsync;

	std::vector<ai::mesh*> meshes;

	//init textures
	GLuint texture_checker;
};