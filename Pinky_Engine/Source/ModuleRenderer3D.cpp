// Importers
#include "../Assimp/Assimp.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "../SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>


#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"
#include "../DevIL/include/ilut.h"
#include "../mmgr/mmgr.h"

#ifdef _DEBUG
#pragma comment (lib, "MathGeoLib/libx86/libDebug/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#else
#pragma comment (lib, "MathGeoLib/libx86/libRelease/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#endif // _DEBUG


ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Get RAM values
	statsVRAM = m_getMemoryStatistics();

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if (ret == true)
	{
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
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
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
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
		glewInit();
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	Grid.axis = true;
	wireframe = false;
	LoadCheckers();

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;

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


	glBindTexture(GL_TEXTURE_2D, 0);
	Grid.Render();

	(wireframe) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for each (ai::mesh* i in meshes)
	{
		DrawMesh(i);
	}

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN || 
		App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
	{
		ai::DeleteLastMesh();
	}

	return ret;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	// Cleanup
	for (auto i = 0; i < meshes.size(); i++)
	{
		ai::DeleteLastMesh();
	}
	
	SDL_GL_DeleteContext(context);
	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//todo: USE MATHGEOLIB here BEFORE 1st delivery! (TIP: Use MathGeoLib/Geometry/Frustum.h, view and projection matrices are managed internally.)
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(ProjectionMatrix.M);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::HardwareDetection(bool& infoOutputWin)
{
	if (infoOutputWin)
	{
		ImGui::Begin("Hardware", &infoOutputWin);
		//Diapo sale cajita con active, no entiendo
		ImGui::Text("SDL Version: %d.%d.%d", versionSDL.major, versionSDL.minor, versionSDL.patch);

		//CPU info
		ImGui::Separator();
		ImGui::Text("CPUs: %d (Cache: %d kb) ", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
		ImGui::Text("System RAM: %.2f GB", SDL_GetSystemRAM()* 0.001048576);
		ImGui::Text("Caps: ", SDL_GetSystemRAM());
		ImGui::SameLine();
		if (SDL_Has3DNow())ImGui::Text("3Dnow ");
		ImGui::SameLine();
		if (SDL_HasAltiVec)ImGui::Text("AltiVec ");
		ImGui::SameLine();
		if (SDL_HasAVX())ImGui::Text("AVX ");
		ImGui::SameLine();
		if (SDL_HasAVX2())ImGui::Text("AVX2 ");
		ImGui::SameLine();
		if (SDL_HasMMX())ImGui::Text("MMX ");
		ImGui::SameLine();
		if (SDL_HasRDTSC)ImGui::Text("RDTSC ");
		ImGui::SameLine();
		if (SDL_HasSSE())ImGui::Text("SSE ");
		ImGui::SameLine();
		if (SDL_HasSSE2())ImGui::Text("SSE2 ");
		ImGui::SameLine();
		if (SDL_HasSSE3())ImGui::Text("SSE3 ");
		ImGui::SameLine();
		if (SDL_HasSSE41())ImGui::Text("SSE41 ");
		ImGui::SameLine();
		if (SDL_HasSSE42())ImGui::Text("SSE42");

		//GPU info
		ImGui::Separator();
		ImGui::Text("GPU: ");
		ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_RENDERER));
		ImGui::Text("Brand: ");
		ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_VENDOR));
		ImGui::Text("Version: ");
		ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_VERSION));
		ImGui::Text("VRAM Budget: %d", statsVRAM.accumulatedActualMemory);//TODO: no sé que representa cada cosa de estas
		ImGui::Text("VRAM Usage: %d", statsVRAM.accumulatedReportedMemory);
		ImGui::Text("VRAM Available: %d", statsVRAM.totalActualMemory);
		ImGui::Text("VRAM Reserved: %d", statsVRAM.totalReportedMemory);
		ImGui::Text("VRAM nose: %d", statsVRAM.peakActualMemory);
		ImGui::Text("VRAM nose2: %d", statsVRAM.peakActualMemory);

		ImGui::End();
	}
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

void ModuleRenderer3D::DrawMesh(ai::mesh* mesh)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	//normals
	glEnableClientState(GL_NORMAL_ARRAY);

	//TODO NO VA ahora si¿?
	//texture
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// Mesh buffers
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);

    
	// Normales
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
	glNormalPointer(GL_FLOAT, 0, NULL);

	// Textures
	//TODO NO VA ahora si¿?
	glBindBuffer(GL_ARRAY_BUFFER, mesh->tex.id_tex);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glBindTexture(GL_TEXTURE_2D, mesh->tex.id_tex);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 1); // Cleanning bind buffer;

	// Draw mesh
	glDrawElements(GL_TRIANGLES, mesh->num_index, GL_UNSIGNED_INT, NULL);
	// Clean textures
	//glDrawArrays(GL_TRIANGLES, 0, mesh->num_vertex);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void ModuleRenderer3D::LoadCheckers()
{
	GLuint buffer;
	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &buffer);
	glBindTexture(GL_TEXTURE_2D, buffer);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	/*glTexImage2D(GL_TEXTURE_2D, 0, 3, CHECKERS_HEIGHT, CHECKERS_HEIGHT, 0, GL_RGB,
		GL_UNSIGNED_BYTE, checkerImage);  // Create texture from image data*/
}