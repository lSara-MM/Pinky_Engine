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

#include "R_Mesh.h"
#include "C_UI.h"

#include <string>

#include "UI_Text.h"

#include "External Libraries/FreeType/include/ft2build.h"
#include "External Libraries/FreeType/include/freetype/freetype.h"
#include "External Libraries/FreeType/include/freetype/ftglyph.h"
#pragma comment(lib, "Source/External Libraries/FreeType/libx86/freetype.lib")

#define MAX_LIGHTS 8
#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void DrawBox();
	void SetVsync(bool enable);
	void SetGameCamera(C_Camera* cam);
	void GetUIGOs(GameObject* go, std::vector<C_UI*>& listgo);
	void GetSceneGOs(GameObject* go, std::vector<GameObject*>& listgo);

public:

	//Scene viewer camera
	C_Camera* editorCam;

	//Game viewer camera
	C_Camera* gameCam;

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	CPlane grid;

	bool wireframe;
	bool vertexNormals;
	bool faceNormals;
	bool vsync;

	//init textures
	GLuint texture_checker;

	//font
	Font* defaultFont;
};