#pragma once
#include "External Libraries/Assimp/Assimp.h"

#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"

#include "External Libraries/MathGeoLib/include/Math/float3x3.h"
#include "External Libraries/MathGeoLib/include/Math/float4x4.h"
#include "Primitive.h"

#include "External Libraries/mmgr/mmgr.h"

#include "External Libraries/Glew/include/glew.h"
#include "C_Camera.h"

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
	void SetCamActive(C_Camera* cam);
	//wip, dunno how it works
	void CreateFrameBuffer(int width, int height);

public:

	C_Camera* activeCam = nullptr;

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	CPlane Grid;

	bool wireframe;
	bool VertexNormals;
	bool FaceNormals;
	bool Vsync;

	std::vector<ai::mesh*> meshes;

	//init textures
	GLuint texture_checker;
};