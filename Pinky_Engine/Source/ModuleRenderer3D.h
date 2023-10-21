#pragma once

// Importers
// TODO: cambiar de sitio cuando se cree donde tiene que estar
// TODO preguntar: porque tiene que estar arriba de todo o sino peta el algorithm 
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


//todo: REMOVE this before 1st delivery!!
#include "glmath.h"

#define MAX_LIGHTS 8
#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64

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
	void HardwareDetection(bool &infoOutputWin);

	void DrawBox();
	void DrawMesh(ai::mesh* mesh);
	void DrawVertexNormals(ai::mesh* mesh);
	void DrawFaceNormals(ai::mesh* mesh);

	sMStats statsVRAM;

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	CPlane Grid;
	
	//You won't need this after using Frustum
	mat4x4 ProjectionMatrix;

	bool wireframe;
	bool VertexNormals;
	bool FaceNormals;

	//Struct to get sdl version info
	SDL_version versionSDL;

	std::vector<ai::mesh*> meshes;

	//init textures
	GLuint texture_checker;
};