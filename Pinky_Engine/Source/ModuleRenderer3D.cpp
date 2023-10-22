// Importers
#include "../Assimp/Assimp.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "../SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../MathGeoLib/include/MathGeoLib.h"

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"
#include "../DevIL/include/il.h"
#include "../DevIL/include/ilu.h"
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

		iluInit();
		ilInit();
		ilutInit();
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

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

	/*for each (ai::mesh* i in meshes)
	{
		DrawMesh(i);
		if (VertexNormals)
		{
			DrawVertexNormals(i);
		}
		if (FaceNormals)
		{
			DrawFaceNormals(i);
		}
	}*/

	return ret;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	// Cleanup
	ClearVecPtr(meshes);
	
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
		ImGui::Text("Total Reported Memory: %d", statsVRAM.totalReportedMemory);
		ImGui::Text("Total Actual Memory: %d", statsVRAM.totalActualMemory);
		ImGui::Text("Max Reported Memory: %d", statsVRAM.peakReportedMemory);
		ImGui::Text("Max Actual Mem: %d", statsVRAM.peakActualMemory);
		ImGui::Text("Accumulated Reported Mem: %d", statsVRAM.accumulatedReportedMemory);
		ImGui::Text("Accumulated Actual Mem: %d", statsVRAM.accumulatedActualMemory);
		ImGui::Text("Accumulated Alloc Unit Count: %d", statsVRAM.accumulatedAllocUnitCount);
		ImGui::Text("Total Alloc Unit Count: %d", statsVRAM.totalAllocUnitCount);
		ImGui::Text("Peak Alloc Unit Count: %d", statsVRAM.peakAllocUnitCount);
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
	glBindBuffer(GL_ARRAY_BUFFER, mesh->tex.id_tex);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, mesh->tex.id_tex);
	//(mesh->hasTex) ? glBindTexture(GL_TEXTURE_2D, mesh->tex.id_tex) : glBindTexture(GL_TEXTURE_2D, texture_checker);

	// Draw mesh
	glDrawElements(GL_TRIANGLES, mesh->num_index, GL_UNSIGNED_INT, NULL);

	// Clean textures
	glBindTexture(GL_TEXTURE_2D, 0); // Cleanning bind buffer;
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void ModuleRenderer3D::DrawVertexNormals(ai::mesh* mesh)
{
	//TODO: adjust length value with editor
	float normal_lenght = 1.0f;
	glBegin(GL_LINES);
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	for (uint i = 0; i < mesh->num_vertex * 3; i += 3)
	{
		LineSegment NormalDirection(math::float3(mesh->vertex[i], mesh->vertex[i + 1], mesh->vertex[i + 2]),
			math::float3(mesh->vertex[i] + mesh->normals[i], mesh->vertex[i + 1] + mesh->normals[i + 1], mesh->vertex[i + 2] + mesh->normals[i + 2]));

		glVertex3f(NormalDirection.a.x * normal_lenght, NormalDirection.a.y * normal_lenght, NormalDirection.a.z * normal_lenght);
		glVertex3f(NormalDirection.b.x * normal_lenght, NormalDirection.b.y * normal_lenght, NormalDirection.b.z * normal_lenght);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();
}

void ModuleRenderer3D::DrawFaceNormals(ai::mesh* mesh)
{
	//TODO: adjust length value with editor
	float normal_lenght = 1.0f;
	glBegin(GL_LINES);
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	for (size_t i = 0; i < mesh->num_index ; i += 3)
	{
		uint index1 = mesh->index[i] * 3;
		uint index2 = mesh->index[i + 1] * 3;
		uint index3 = mesh->index[i + 2] * 3;

		math::float3 vec1(mesh->vertex[index1], mesh->vertex[index1 + 1], mesh->vertex[index1 + 2]);
		math::float3 vec2(mesh->vertex[index2], mesh->vertex[index2 + 1], mesh->vertex[index2 + 2]);
		math::float3 vec3(mesh->vertex[index3], mesh->vertex[index3 + 1], mesh->vertex[index3 + 2]);
		
		math::float3 v0 = vec2 - vec1;
		math::float3 v1 = vec3 - vec1;
		math::float3 crossV = math::Cross(v0, v1);

		math::float3 crossNorm = crossV.Normalized();

		GLfloat vx = (vec1.x + vec2.x + vec3.x) / 3;
		GLfloat vy = (vec1.y + vec2.y + vec3.y) / 3;
		GLfloat vz = (vec1.z + vec2.z + vec3.z) / 3;

		GLfloat vx_norm = crossNorm.x;
		GLfloat vy_norm = crossNorm.y;
		GLfloat vz_norm = crossNorm.z;

		glVertex3f(vx, vy, vz);
		glVertex3f(vx + (vx_norm * normal_lenght), vy + (vy_norm * normal_lenght), vz + (vz_norm * normal_lenght));
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();
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
