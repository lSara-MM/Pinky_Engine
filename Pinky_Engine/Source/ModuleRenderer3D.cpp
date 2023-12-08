// Importers
#include "External Libraries/Assimp/Assimp.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "External Libraries/SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "External Libraries/MathGeoLib/include/MathGeoLib.h"

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Source/External Libraries/Glew/libx86/glew32.lib")

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"
#include "External Libraries/DevIL/include/il.h"
#include "External Libraries/DevIL/include/ilu.h"
#include "External Libraries/DevIL/include/ilut.h"
#include "External Libraries/mmgr/mmgr.h"
#include "C_Camera.h"
#include "GameObject.h"
#include "ModuleScene.h"

#ifdef _DEBUG
#pragma comment (lib, "Source/External Libraries/MathGeoLib/libx86/libDebug/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#else
#pragma comment (lib, "Source/External Libraries/MathGeoLib/libx86/libRelease/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#endif // _DEBUG


ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	gameCam = nullptr;
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);

	if(context == NULL)
	{
		LOG("[ERROR] OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if (ret == true)
	{
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("[WARNING]: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("[ERROR] initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("[ERROR] initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("[ERROR] initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);

		glEnable(GL_TEXTURE_2D);

		error = glewInit();
		if (error != GL_NO_ERROR)
		{
			LOG("[ERROR] initializing glew library! %s", SDL_GetError());
			ret = false;
		}
		else
		{
			LOG("Correct Glew initialization, Version: %d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
		}

		iluInit();
		ilInit();
		ilutInit();

		if (ilutRenderer(ILUT_OPENGL))
		{
			LOG("Correct DevIL initialization");
		}

		if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
			iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION || 
			ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION) 
		{
			printf("DevIL version is different...exiting!\n");
		}

		LOG("Correct OpenGL initialization");
		LOG("Vendor: %s", glGetString(GL_VENDOR));
		LOG("Renderer: %s", glGetString(GL_RENDERER));
		LOG("OpenGL version supported %s", glGetString(GL_VERSION));

		ai::EnableDebug();
	}

	// Projection matrix for
	editorCam->OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
	C_Camera* Cam = (C_Camera*)App->scene->mainCamera->GetComponentByType(C_TYPE::CAM);
	Cam->isGameCam = true;
	Cam->SetAsMain(Cam->isGameCam);

	Grid.axis = true;
	wireframe = false;
	VertexNormals = false;
	FaceNormals = true;
	Vsync = true;

	ai::LoadCheckers(texture_checker);
	meshes = {};
	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{

	editorCam->Draw();
	// light 0 on cam pos
	lights[0].SetPos(editorCam->frustum.pos.x, editorCam->frustum.pos.y, editorCam->frustum.pos.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	if (App->editor->raycast)
	{
		App->camera->DebugRaycast();
	}

	//cube made with GL_TRIANGLES
	/*glLineWidth(2.0f);
	glBegin(GL_TRIANGLES);

	glVertex3d(0, 0, 0);glVertex3d(1, 1, 0);glVertex3d(1, 0, 0);
	glVertex3d(0, 0, 0); glVertex3d(0, 1, 0); glVertex3d(1, 1, 0);
	glVertex3d(0, 0, 0); glVertex3d(0, 1, 1); glVertex3d(0, 1, 0);
	glVertex3d(0, 0, 1); glVertex3d(0, 1, 1); glVertex3d(0, 0, 0);
	glVertex3d(1, 0, 1); glVertex3d(0, 1, 1); glVertex3d(0, 0, 1);
	glVertex3d(1, 0, 1); glVertex3d(1, 1, 1); glVertex3d(0, 1, 1);
	glVertex3d(1, 0, 1); glVertex3d(1, 0, 0); glVertex3d(1, 1, 1);
	glVertex3d(1, 0, 0); glVertex3d(1, 1, 0); glVertex3d(1, 1, 1);
	glVertex3d(1, 1, 1); glVertex3d(0, 1, 0); glVertex3d(0, 1, 1);
	glVertex3d(1, 1, 1); glVertex3d(1, 1, 0); glVertex3d(0, 1, 0);
	glVertex3d(1, 0, 0); glVertex3d(0, 0, 1); glVertex3d(0, 0, 0);
	glVertex3d(1, 0, 0); glVertex3d(1, 0, 1); glVertex3d(0, 0, 1);

	glEnd();
	glLineWidth(1.0f);*/


	//glBindTexture(GL_TEXTURE_2D, 0);
	Grid.Render();
	(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	editorCam->UnbindFrameBuffer();

	if (App->renderer3D->gameCam != nullptr)
	{
		gameCam->Draw();

		lights[0].SetPos(gameCam->frustum.pos.x, gameCam->frustum.pos.y, gameCam->frustum.pos.z);

		for (uint i = 0; i < MAX_LIGHTS; ++i)
			lights[i].Render();

		App->scene->rootNode->Update(dt);//TODO: guarrada important?



		glBindTexture(GL_TEXTURE_2D, 0);
		Grid.Render();

		(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		gameCam->UnbindFrameBuffer();
	}

	return ret;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	ai::DisableDebug();
	ClearVec(meshes);
	
	SDL_GL_DeleteContext(context);
	return true;
}

void ModuleRenderer3D::DrawBox()
{
	glBegin(GL_QUADS);
	// Front Face
	glNormal3f(0.0f, 0.0f, 1.0f);// Normal Pointing Towards Viewer
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);// Point 1 (Front)
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);// Point 2 (Front)
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);// Point 3 (Front)
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);// Point 4 (Front)
	// Back Face
	glNormal3f(0.0f, 0.0f, -1.0f);// Normal Pointing Away From Viewer
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);// Point 1 (Back)
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);// Point 2 (Back)
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);// Point 3 (Back)
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);// Point 4 (Back)
	// Top Face
	glNormal3f(0.0f, 1.0f, 0.0f);// Normal Pointing Up
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);// Point 1 (Top)
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);// Point 2 (Top)
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);// Point 3 (Top)
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);// Point 4 (Top)
	// Bottom Face
	glNormal3f(0.0f, -1.0f, 0.0f);// Normal Pointing Down
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);// Point 1 (Bottom)
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);// Point 2 (Bottom)
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);// Point 3 (Bottom)
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);// Point 4 (Bottom)
	// Right face
	glNormal3f(1.0f, 0.0f, 0.0f);// Normal Pointing Right
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);// Point 1 (Right)
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);// Point 2 (Right)
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);// Point 3 (Right)
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);// Point 4 (Right)
	// Left Face
	glNormal3f(-1.0f, 0.0f, 0.0f);// Normal Pointing Left
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);// Point 1 (Left)
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);// Point 2 (Left)
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);// Point 3 (Left)
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);// Point 4 (Left)
	glEnd();// Done Drawing Quads
}

void ModuleRenderer3D::SetVsync(bool enable)
{
	if (enable)
	{
		if (SDL_GL_SetSwapInterval(1) == -1)
		{
			LOG("VSYNC not supported: %s\n", SDL_GetError());
		}
		else
		{
			LOG("VSYNC enabled");
		}
	}
	else
	{
		if (SDL_GL_SetSwapInterval(0) == -1)
		{
			LOG("VSYNC not supported: %s\n", SDL_GetError());
		}
		else
		{
			LOG("VSYNC disabled");
		}
	}
}

void ModuleRenderer3D::SetGameCamera(C_Camera* cam)
{
	if (cam != nullptr)
	{
		cam->OnResize(SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
		gameCam = cam;
	}
}
