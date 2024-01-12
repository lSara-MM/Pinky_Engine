#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "External Libraries/SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Source/External Libraries/Glew/libx86/glew32.lib")

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"
#include "External Libraries/ImGui/misc/cpp/imgui_stdlib.h"
//

#include "External Libraries/ImGuizmo/ImGuizmo.h"
//
#include "External Libraries/ImGui/imgui_custom.h"
#include "External Libraries/Deviceld/DeviceId.h"

#ifdef _DEBUG
#pragma comment (lib, "Source/External Libraries/MathGeoLib/libx86/libDebug/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#else
#pragma comment (lib, "Source/External Libraries/MathGeoLib/libx86/libRelease/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#endif // _DEBUG


#include "ModuleScene.h"
#include "GameObject.h"
#include "ImGuiResourcesWindows.h"
#include "G_UI.h"

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	VRAM_budget = 0;
	VRAM_usage = 0;
	VRAM_available = 0;
	VRAM_reserved = 0;
}

// Destructor
ModuleEditor::~ModuleEditor()
{}

// Called before render is available
bool ModuleEditor::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	ImGui::StyleColorsClassic();
	//ImGuiCustom::Theme_Cinder();
	//ImGuiCustom::Theme_EnemyMouse();
	ImGuiCustom::Theme_PinkCherry();

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Calibri.ttf", 15.0f);
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Consola.ttf", 14.0f);
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Comic.ttf", 18.0f);
	io.Fonts->AddFontDefault();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init("#version 130");

	// Module Settings
	docking = true;
	moduleSettingsWin = false;
	styleEditor = false;
	frcap = true;

	saveAs = false;

	//// OpenGL configuration
	depthTest = true;
	cullFace = true;
	lightning = true;
	colorMaterial = true;
	texture2D = true;
	blend = false;
	lineSmooth = true;
	normalize = true;
	raycast = false;

	//
	infoOutputWin = false;
	//
	consoleWin = false;

	//VRAM data (leaks memory in update)
	getGraphicsDeviceInfo(NULL, NULL, NULL, &VRAM_budget, &VRAM_usage, &VRAM_available, &VRAM_reserved);

	// About
	aboutColor = ImVec4(0.953f, 0.533f, 0.969f, 1.0f);
	aboutWin = false;

	changeTimeState = false;
	currentTimeState = TimeManager::PlayState::EDITOR;

	//Guizmos
	transformOperation = ImGuizmo::OPERATION::TRANSLATE;
	return ret;
}

// PreUpdate: clear buffer
update_status ModuleEditor::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleEditor::PostUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	memoryStats = m_getMemoryStatistics();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	ImGuiIO& io = ImGui::GetIO();

	UseDockSpace(io);

	ret = Toolbar();

	//Time
	if (TimeManager::IsOnStep())
	{
		TimeManager::Pause();
	}

	if (changeTimeState) {
		switch (currentTimeState)
		{
		case TimeManager::PlayState::EDITOR:
			TimeManager::Stop();
			break;
		case TimeManager::PlayState::PLAY:
			TimeManager::Play();
			break;
		case TimeManager::PlayState::PAUSE:
			TimeManager::Pause();
			break;
		case TimeManager::PlayState::STEP:
			TimeManager::Step();
			break;
		default:
			break;
		}
		changeTimeState = false;
	}

	TimeButtons();

	//Scene window
	EditorWindow();

	//Game window
	GameWindow();

	//Configwindow, change to fullscreen and such
	ConfigWindow(io);
	HardwareDetection(infoOutputWin);

	// Hierarchy - Inspector
	for (int i = 0; i < vImGuiWindows.size(); i++)
	{
		if (vImGuiWindows[i]->show)
		{
			vImGuiWindows[i]->ShowWindow();
		}
	}

	// Rendering
	ImGui::Render();
	//glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	//glClearColor(1.0, 1.0, 1.0, 0.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	SDL_GL_SwapWindow(App->window->window);

	return ret;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	LOG("Destroying 3D Renderer");

	ClearVec(mFPSLog);
	ClearVec(mSLog);
	ClearVec(MemLog);

	ClearVecPtr(vImGuiWindows);

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(context);
	return true;
}

void ModuleEditor::PrimitivesMenu(GameObject* go, bool component)
{
	std::array<std::string, 4> components = { "Cube", "Sphere", "Cylinder", "Plane" };

	for (int i = 0; i < components.size(); i++)
	{
		if (ImGui::MenuItem(components[i].c_str()))
		{
			ai::CreatePolyPrimitive(ai::POLY_PRIMITIVE_TYPE(i), go, component);
			break;
		}
	}
}

void ModuleEditor::UseDockSpace(ImGuiIO& io)
{
	if (docking)
	{
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else { dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode; }

		//// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		//// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) { window_flags |= ImGuiWindowFlags_NoBackground; }

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding) { ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); }
		ImGui::Begin("DockSpace", NULL, window_flags);

		if (!opt_padding) { ImGui::PopStyleVar(); }

		if (opt_fullscreen) { ImGui::PopStyleVar(2); }

		// Submit the DockSpace
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		ImGui::End();
	}
}

update_status ModuleEditor::Toolbar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New")) {}
			if (ImGui::MenuItem("Open", "Ctrl+O")) {}
			if (ImGui::BeginMenu("Open Recent"))
			{
				ImGui::MenuItem("fish_hat.c");
				ImGui::MenuItem("fish_hat.inl");
				ImGui::MenuItem("fish_hat.h");
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Save", "Ctrl+S")) { App->parson->SaveScene(App->scene->rootNode->name); }
			if (ImGui::MenuItem("Save As...")) { saveAs = true; }

			ImGui::Separator();
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::BeginMenu("Theme"))
				{
					if (ImGui::MenuItem("Classic"))
					{
						ImGui::StyleColorsClassic();
					}
					if (ImGui::MenuItem("Light (please don't)"))
					{
						ImGui::StyleColorsLight();
					}
					if (ImGui::MenuItem("Dark"))
					{
						ImGui::StyleColorsDark();
					}
					ImGui::Separator();

					if (ImGui::MenuItem("Advanced"))
					{
						moduleSettingsWin = true;
						styleEditor = true;
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Exit"))
			{
				ImGui::EndMenu();
				ImGui::EndMainMenuBar();
				return UPDATE_STOP;
			}

			ImGui::EndMenu();
		}
		SaveAs();

		//------
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}

		//------
		if (ImGui::BeginMenu("Game Object"))
		{
			if (ImGui::MenuItem("Create Empty")) { new GameObject("Empty"); }

			if (ImGui::BeginMenu("Create Primitive"))
			{
				PrimitivesMenu();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Create Custom Mesh"))
			{
				std::array<std::string, 3> components = { "Law Hat", "Kuro", "King Shark" };

				for (int i = 0; i < components.size(); i++)
				{
					if (ImGui::MenuItem(components[i].c_str()))
					{
						ai::CreateCustomMehses(ai::CUSTOM_MESH(i));
						break;
					}
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("UI"))
			{
				std::array<std::string, 6> ui = { "Canvas", "Image", "Text", "Button", "Input Box", "Checkbox"};

				for (int i = 0; i < ui.size(); i++)
				{
					if (ImGui::MenuItem(ui[i].c_str()))
					{
						new G_UI((UI_TYPE)i);
						break;
					}
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		//------
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Hierarchy"))
			{
				if (App->scene->hierarchy != nullptr)
				{
					App->scene->hierarchy->show = !App->scene->hierarchy->show;
				}
				//vImGuiWindows.push_back(new Hierarchy(vImGuiWindows.size()));
			}
			if (ImGui::MenuItem("Inspector"))
			{
				if (App->scene->inspector != nullptr)
				{
					App->scene->inspector->show = !App->scene->inspector->show;
				}
				//vImGuiWindows.push_back(new Inspector(vImGuiWindows.size()));
			}
			if (ImGui::MenuItem("Console"))
			{
				if (App->scene->console != nullptr)
				{
					App->scene->console->show = !App->scene->console->show;
				}
				//consoleWin != consoleWin;
			}
			if (ImGui::MenuItem("Project"))
			{
				if (App->scene->project != nullptr)
				{
					App->scene->project->show = !App->scene->project->show;
				}
			}
			if (ImGui::MenuItem("Resources"))
			{
				if (App->scene->rm != nullptr)
				{
					App->scene->rm->show = !App->scene->rm->show;
				}
			}
			ImGui::EndMenu();
		}

		//------
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Module Settings")) { moduleSettingsWin = true; }
			if (ImGui::MenuItem("Information Output")) { infoOutputWin = true; }
			ImGui::Separator();
			if (ImGui::MenuItem("About")) { aboutWin = true; }
			ImGui::EndMenu();
		}
		AboutWindow();

		//------
		ImGui::Checkbox("Wireframe", &App->renderer3D->wireframe);
		ImGui::Checkbox("Raycast", &raycast);

		ImGui::EndMainMenuBar();
	}

	return UPDATE_CONTINUE;
}

void ModuleEditor::ConfigWindow(ImGuiIO& io)
{
	if (moduleSettingsWin)
	{
		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(500, 500));
		ImGui::Begin("Configuration", &moduleSettingsWin, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

		static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton;
		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Window"))
			{
				ImGuiCustom::ToggleButton("Docking", &docking);
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Enable docking");
				}

				//------
				if (ImGuiCustom::ToggleButton("Fullscreen", &App->window->fullScreen))
				{
					(!App->window->fullScreen) ? App->window->flags = SDL_WINDOW_SHOWN : App->window->flags = SDL_WINDOW_FULLSCREEN;

					SDL_SetWindowFullscreen(App->window->window, App->window->flags);
					App->renderer3D->editorCam->OnResize(SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
					if (App->renderer3D->gameCam != nullptr)
					{
						App->renderer3D->gameCam->OnResize(SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
					}
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Change fullscreen window");
				}
				ImGui::SameLine(150);

				if (ImGuiCustom::ToggleButton("Resizable", &App->window->resizable))
				{
					SDL_SetWindowResizable(App->window->window, (SDL_bool)App->window->resizable);
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Change resizable window");
				}

				if (ImGuiCustom::ToggleButton("Borderless", &App->window->borderless))
				{
					SDL_SetWindowBordered(App->window->window, (SDL_bool)!App->window->borderless);
					App->renderer3D->editorCam->OnResize(SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
					if (App->renderer3D->gameCam != nullptr)
					{
						App->renderer3D->gameCam->OnResize(SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
					}
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Change borderless window");
				}
				ImGui::SameLine(150);

				if (ImGuiCustom::ToggleButton("Fullscreen desktop", &App->window->fullScreenDesktop))
				{
					(!App->window->fullScreenDesktop) ? App->window->flags = SDL_WINDOW_SHOWN : App->window->flags = SDL_WINDOW_FULLSCREEN_DESKTOP;

					SDL_SetWindowFullscreen(App->window->window, App->window->flags);
					App->renderer3D->editorCam->OnResize(SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
					if (App->renderer3D->gameCam != nullptr)
					{
						App->renderer3D->gameCam->OnResize(SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
					}
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Change fullscreen desktop window");
				}
				ImGui::Separator();

				ImGui::Text("Window size: %d x %d", SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
				if (ImGui::SliderInt("Width", &SDL_GetWindowSurface(App->window->window)->w, 0, 1920, "%d"))
				{
					SDL_SetWindowSize(App->window->window, SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
					App->renderer3D->editorCam->OnResize(SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
					if (App->renderer3D->gameCam != nullptr)
					{
						App->renderer3D->gameCam->OnResize(SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
					}
				}

				if (ImGui::SliderInt("Height", &SDL_GetWindowSurface(App->window->window)->h, 0, 1920, "%d"))
				{
					SDL_SetWindowSize(App->window->window, SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
					App->renderer3D->editorCam->OnResize(SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
					if (App->renderer3D->gameCam != nullptr)
					{
						App->renderer3D->gameCam->OnResize(SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
					}
				}
				ImGui::Separator();

				ImGui::Text("Brightness: %.2f", SDL_GetWindowBrightness(App->window->window));
				if (ImGui::SliderFloat("Brightness", &App->window->brightness, 0.2350f, 1.0f, "%.2f"))
				{
					SDL_SetWindowBrightness(App->window->window, App->window->brightness);
				}
				ImGui::Separator();

				ImGui::Text("Mouse position: %d x, %d y", App->input->GetMouseX(), App->input->GetMouseY());
				ImGui::Separator();

				FpsWindow(io);

				ImGui::Checkbox("FPS Cap", &App->frcap);
				ImGui::SliderInt("FPS cap", &App->fps, 1, 120, "%d");
				ImGui::Separator();

				if (ImGui::Checkbox("VSync", &App->renderer3D->Vsync))
				{
					App->renderer3D->SetVsync(App->renderer3D->Vsync);
				};

				MemWindow();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("OpenGl"))
			{
				// Depth test
				if (ImGui::Checkbox("Depth testing", &depthTest))
				{
					(depthTest) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("If enabled, do depth comparisons and update the depth buffer. ");
				}
				ImGui::SameLine();

				// Cull Face
				if (ImGui::Checkbox("Cull face", &cullFace))
				{
					(cullFace) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("If enabled, cull polygons based on their winding in window coordinates.");
				}
				ImGui::SameLine();

				// Lightning
				if (ImGui::Checkbox("Lightning", &lightning))
				{
					(lightning) ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("If enabled and no vertex shader is active, use the current lighting parameters to compute the vertex\n color or index. Otherwise, simply associate the current color or index with each vertex.");
				}
				ImGui::SameLine();

				// Color material
				if (ImGui::Checkbox("Color material", &colorMaterial))
				{
					(colorMaterial) ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("If enabled, have one or more material parameters track the current color.");
				}

				//

				// Texture 2D
				if (ImGui::Checkbox("Texture 2D", &texture2D))
				{
					(texture2D) ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("If enabled and no fragment shader is active, two-dimensional texturing is performed.");
				}
				ImGui::SameLine();

				// Blend
				if (ImGui::Checkbox("Blend", &blend))
				{
					(blend) ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("If enabled, blend the computed fragment color values with the values in the color buffers.");
				}
				ImGui::SameLine();

				// Line Smooth
				if (ImGui::Checkbox("Line Smooth", &lineSmooth))
				{
					(lineSmooth) ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("If enabled, draw lines with correct filtering. Otherwise, draw aliased lines.");
				}
				ImGui::SameLine();

				// Line Smooth
				if (ImGui::Checkbox("Normalize", &normalize))
				{
					(normalize) ? glEnable(GL_NORMALIZE) : glDisable(GL_NORMALIZE);
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("If enabled and no vertex shader is active, normal vectors are normalized \nto unit length after transformation and before lighting.");
				}
				ImGui::EndTabItem();
			}

			ImGuiTabItemFlags flags = ImGuiTabItemFlags_None;
			if (styleEditor) flags = ImGuiTabItemFlags_SetSelected;
			if (ImGui::BeginTabItem("Style Editor", NULL, flags))
			{
				ImGui::ShowStyleEditor();
				ImGui::EndTabItem();
			}
			else { styleEditor = false; }

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}

void ModuleEditor::FpsWindow(ImGuiIO& io)
{
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	char title[25];
	AddFPS(mFPSLog, io.Framerate);
	AddFPS(mSLog, 1000.0f / io.Framerate);
	sprintf_s(title, 25, "Framerate %.1f", mFPSLog[mFPSLog.size() - 1]);
	ImGui::PlotHistogram("##framerate ", &mFPSLog[0], mFPSLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100.0f));
	sprintf_s(title, 25, "Milliseconds %0.1f", mSLog[mSLog.size() - 1]);
	ImGui::PlotHistogram("##milliseconds ", &mSLog[0], mSLog.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100.0f));
}

void ModuleEditor::MemWindow()
{
	char title[25];
	AddMem(MemLog, memoryStats.totalReportedMemory);
	ImGui::Text("Reported Memory % 0.1f", MemLog[MemLog.size() - 1]);
	ImGui::PlotHistogram("", &MemLog[0], MemLog.size(), 0, "", 0.0f, memoryStats.peakReportedMemory * 2, ImVec2(310, 100.0f));
}

void ModuleEditor::AddFPS(std::vector<float>& vect, const float aFPS)
{
	if (vect.size() < 60)
	{
		vect.push_back(aFPS);
	}
	else
	{
		std::rotate(vect.begin(), vect.begin() + 1, vect.end());
		vect.pop_back();
		vect.push_back(aFPS);
	}
}

void ModuleEditor::AddMem(std::vector<float>& vect, const float repMem)
{
	if (vect.size() < 100)
	{
		vect.push_back(repMem);
	}
	else
	{
		std::rotate(vect.begin(), vect.begin() + 1, vect.end());
		vect.pop_back();
		vect.push_back(repMem);
	}
}

void ModuleEditor::AddWindow(ImGuiWindows* window)
{
	vImGuiWindows.push_back(window);
}

void ModuleEditor::HardwareDetection(bool& infoOutputWin)
{
	if (infoOutputWin)
	{
		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::Begin("Hardware", &infoOutputWin);
		SDL_GetVersion(&versionSDL);
		ImGui::Text("SDL Version: %d.%d.%d", versionSDL.major, versionSDL.minor, versionSDL.patch);

		//CPU info
		ImGui::Separator();
		ImGui::Text("CPUs: %d (Cache: %d kb) ", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
		ImGui::Text("System RAM: %.2f GB", SDL_GetSystemRAM() * 0.001048576);
		ImGui::Text("Caps:");
		ImGui::SameLine();
		if (SDL_Has3DNow())ImGui::Text("3Dnow");
		ImGui::SameLine();
		if (SDL_HasAltiVec)ImGui::Text("AltiVec");
		ImGui::SameLine();
		if (SDL_HasAVX())ImGui::Text("AVX");
		ImGui::SameLine();
		if (SDL_HasAVX2())ImGui::Text("AVX2");
		ImGui::SameLine();
		if (SDL_HasMMX())ImGui::Text("MMX");
		ImGui::SameLine();
		if (SDL_HasRDTSC)ImGui::Text("RDTSC");
		ImGui::SameLine();
		if (SDL_HasSSE())ImGui::Text("SSE");
		ImGui::SameLine();
		if (SDL_HasSSE2())ImGui::Text("SSE2");
		ImGui::SameLine();
		if (SDL_HasSSE3())ImGui::Text("SSE3");
		ImGui::SameLine();
		if (SDL_HasSSE41())ImGui::Text("SSE41");
		ImGui::SameLine();
		if (SDL_HasSSE42())ImGui::Text("SSE42");

		//GPU info
		ImGui::Separator();
		ImGui::Text("GPU:");
		ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_RENDERER));
		ImGui::Text("Brand:");
		ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_VENDOR));
		ImGui::Text("Version:");
		ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_VERSION));

		ImGui::Text("VRAM Budget: %.2f Mb", (float)VRAM_budget / (1024 * 1024));
		ImGui::Text("VRAM Usage: %.2f Mb", (float)VRAM_usage / (1024 * 1024));
		ImGui::Text("VRAM Available: %.2f Mb", (float)VRAM_available / (1024 * 1024));
		ImGui::Text("VRAM Reserved: %.2f Mb", (float)VRAM_reserved / (1024 * 1024));

		ImGui::Text("Total Reported Memory: %d", memoryStats.totalReportedMemory);
		ImGui::Text("Total Actual Memory: %d", memoryStats.totalActualMemory);
		ImGui::Text("Max Reported Memory: %d", memoryStats.peakReportedMemory);
		ImGui::Text("Max Actual Mem: %d", memoryStats.peakActualMemory);
		ImGui::Text("Accumulated Reported Mem: %d", memoryStats.accumulatedReportedMemory);
		ImGui::Text("Accumulated Actual Mem: %d", memoryStats.accumulatedActualMemory);
		ImGui::Text("Accumulated Alloc Unit Count: %d", memoryStats.accumulatedAllocUnitCount);
		ImGui::Text("Total Alloc Unit Count: %d", memoryStats.totalAllocUnitCount);
		ImGui::Text("Peak Alloc Unit Count: %d", memoryStats.peakAllocUnitCount);
		ImGui::End();
	}
}

void ModuleEditor::SaveAs()
{
	if (saveAs)
	{
		ImGui::OpenPopup("Save As");
	}

	//PopUpWithTextInput(&saveAs, "Save As");

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Save As", &saveAs, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Scene will be saved in %s as '%s'", ASSETS_AUX, SCENE_EXT);	// TODO: Change maybe in another folder?
		ImGui::Separator();

		static std::string sceneName_saveAS = App->scene->rootNode->name.c_str();
		ImGui::InputText("File Name", &sceneName_saveAS);

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			App->parson->SaveScene(sceneName_saveAS);
			saveAs = false;
			sceneName_saveAS = App->scene->rootNode->name.c_str();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			saveAs = false;
			sceneName_saveAS = App->scene->rootNode->name.c_str();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

std::string  ModuleEditor::PopUpWithTextInput(bool* winBool, std::string title, std::string defaultString)
{
	/*if (ImGui::BeginPopupModal(title.c_str(), winBool, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup(); return "a";
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup(); return "";
		}
		ImGui::EndPopup();
	}*/

	return "";
}

void ModuleEditor::AboutWindow()
{
	if (aboutWin)
	{
		// Open a modal pop up to show info about the project
		ImGui::OpenPopup("About");
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Appearing);
	if (ImGui::BeginPopupModal("About", &aboutWin))
	{
		ImGui::BeginChild("left pane", ImVec2(150, 300), true);
		ImGui::TextWrapped("MIT LicenseCopyright\n(c) 2023\n");
		ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the 'Software'), to dealin the Software without restriction, including without limitation the rightsto use, copy, modify, merge, publish, distribute, sublicense, and/or sellcopies of the Software, and to permit persons to whom the Software isfurnished to do so, subject to the following conditions:The above copyright notice and this permission notice shall be included in allcopies or substantial portions of the Software.THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS ORIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THEAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHERLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THESOFTWARE.");
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::BeginChild("right pane", ImVec2(500 - 200, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us

		if (ImGui::BeginTabBar("Pinky Engine"), ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton)
		{
			if (ImGui::BeginTabItem("Pinky Engine"))
			{
				ImGui::TextColored(aboutColor, "lSara-MM & AndyCubico");
				ImGui::TextWrapped("3D Game engine done for the Game Engines subject in the CITM bachelor's degree in Video Game Design and Development");
				ImGui::ColorEdit3("", (float*)&aboutColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Libraries"))
			{
				//---ImGui---
				if (ImGui::Button("ImGui")) { OsOpenInShell("https://github.com/ocornut/imgui"); } ImGui::SameLine();
				ImGui::TextWrapped("Omar Cornut Copyright (c) 2014-2023");

				//---ImGuizmo---
				if (ImGui::Button("ImGuizmo")) { OsOpenInShell("https://github.com/CedricGuillemet/ImGuizmo"); } ImGui::SameLine();
				ImGui::TextWrapped("Copyright (c) 2016 Cedric Guillemet");

				//---PhysFS---
				if (ImGui::Button("PhysFS")) { OsOpenInShell("https://icculus.org/physfs/"); } ImGui::SameLine();
				ImGui::TextWrapped("Copyright (c) 2001-2012 Ryan C. Gordon and others.");

				//---OpenGL---
				if (ImGui::Button("OpenGL")) { OsOpenInShell("https://glew.sourceforge.net/"); } ImGui::SameLine();
				if (ImGui::Button("OpenGL License")) { OsOpenInShell("https://github.com/nigels-com/glew#copyright-and-licensing"); }

				//---MathGeoLib---
				if (ImGui::Button("MathGeoLib")) { OsOpenInShell("https://github.com/juj/MathGeoLib"); } ImGui::SameLine();
				if (ImGui::Button("MathGeoLib License")) { OsOpenInShell("http://www.apache.org/licenses/LICENSE-2.0.html"); }

				//---Parson---
				if (ImGui::Button("Parson")) { OsOpenInShell("https://github.com/kgabis/parson"); } ImGui::SameLine();
				ImGui::TextWrapped("Copyright(c) 2012 - 2022 Krzysztof Gabis");

				//---Assimp---
				if (ImGui::Button("Assimp")) { OsOpenInShell("https://www.assimp.org/"); } ImGui::SameLine();
				ImGui::TextWrapped("Copyright (c) 2006-2021, assimp team");

				//---DevIL---
				if (ImGui::Button("DevIL")) { OsOpenInShell("https://openil.sourceforge.net/"); } ImGui::SameLine();
				ImGui::TextWrapped("Copyright (C) 1991, 1999 Free Software Foundation");

				//---SDL---
				if (ImGui::Button("SDL")) { OsOpenInShell("https://www.libsdl.org/index.php"); } ImGui::SameLine();
				if (ImGui::Button("SDL License")) { OsOpenInShell("https://www.libsdl.org/license.php"); }

				//---mmgr---
				if (ImGui::Button("mmgr")) { OsOpenInShell("https://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml"); }

				//---Deviceld---
				if (ImGui::Button("Deviceld")) { OsOpenInShell("https://github.com/GameTechDev/gpudetect"); } ImGui::SameLine();
				ImGui::TextWrapped("Copyright 2017 - 2018 Intel Corporation");
				ImGui::EndTabItem();

			}
			if (ImGui::BeginTabItem("External Sources"))
			{
				ImGui::TextColored(aboutColor, "WIP");
				ImGui::ColorEdit3("", (float*)&aboutColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				ImGui::EndTabItem();
			}
			//ImGui::Text("Pinky Engine");
			//ImGui::Separator();

			ImGui::EndTabBar();
		}
		ImGui::EndChild();

		ImGui::PushStyleColor(ImGuiCol_Button, aboutColor);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		if (ImGui::Button("GitHub")) { OsOpenInShell("https://github.com/lSara-MM/Pinky_Engine"); }
		ImGui::PopStyleColor(2);
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Close")) { aboutWin = false; ImGui::CloseCurrentPopup(); }
		ImGui::EndGroup();
		ImGui::EndPopup();
	}
}

void ModuleEditor::EditorWindow()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 2, 2 });

	ImGui::Begin("Scene", NULL);

	ImVec2 viewSize = ImGui::GetContentRegionAvail();
	ImVec2 viewPos = ImGui::GetWindowPos();
	ViewportPos = { viewPos.x,viewPos.y };
	ViewportSize = { viewSize.x,viewSize.y };

	App->renderer3D->editorCam->SetAspectRatio(ViewportSize.x, ViewportSize.y);
	ImGui::Image((ImTextureID)App->renderer3D->editorCam->textureColourBuffer, ViewportSize, ImVec2(0, 1), ImVec2(1, 0));

	//Guizmos
	std::vector<GameObject*> selectedList;
	selectedList = App->scene->hierarchy->GetSelectedGOs();

	ImGuizmoControl();

	for (auto i = 0; i < selectedList.size(); i++)
	{
		if (selectedList[i] != nullptr && selectedList[i]->transform != nullptr)
		{
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(viewPos.x, viewPos.y, viewSize.x, viewSize.y);

			//Selected go info
			C_Transform* transform = selectedList[i]->transform;
			float4x4 objectMatrix = transform->globalMatrix.Transposed();

			//Snap
			if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			{
				snapValue = 1.0f; // Snap to 1.0m for translation/scale
				if (transformOperation == ImGuizmo::OPERATION::ROTATE)
				{
					// Snap to 45 degrees for rotation
					snapValue = 45.0f;
				}
			}

			else
			{
				snapValue = 0.0f;
			}

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(App->renderer3D->editorCam->GetViewMatrix(), App->renderer3D->editorCam->GetProjectionMatrix(),
				transformOperation, ImGuizmo::MODE::LOCAL, objectMatrix.ptr(), nullptr, snapValues);

			if (ImGuizmo::IsUsing() && ImGui::IsWindowHovered())
			{
				float4x4 matrix = objectMatrix.Transposed();
				selectedList[i]->transform->UpdateTransformGuizmo(matrix);
			}
		}
	}

	//TODO: igual posar en funció
	if (ImGui::IsWindowHovered())
	{
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && !ImGuizmo::IsUsing() && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT)
		{
			origin.x = (ImGui::GetMousePos().x - viewPos.x) / viewSize.x;
			origin.y = (ImGui::GetMousePos().y - viewPos.y) / viewSize.y;
			origin.x = (origin.x - 0.5f) * 2;
			origin.y = -((origin.y - 0.5f) * 2);

			if (origin.x >= -1 && origin.x <= 1 && origin.y >= -1 && origin.y <= 1)
			{
				pickingRay = App->camera->MainCamera->frustum.UnProjectLineSegment(origin.x, origin.y + 0.05);//TODO: fix this, why the hell do i need an offset for this to work
				App->camera->MousePick(pickingRay);
				App->camera->MousePickUI(pickingRay);
			}
		}
		App->camera->CameraInput();
	}

	ImGui::End();

	ImGui::PopStyleVar();
}

void ModuleEditor::ImGuizmoControl()
{
	if ((App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN))
	{
		transformOperation = ImGuizmo::OPERATION::TRANSLATE;
	}
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
	{
		transformOperation = ImGuizmo::OPERATION::ROTATE;
	}
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		transformOperation = ImGuizmo::OPERATION::SCALE;
	}
}

void ModuleEditor::GameWindow()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

	ImGui::Begin("Game", NULL);

	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	GameViewSize = { viewportSize.x,viewportSize.y };
	ImVec2 viewPos = ImGui::GetWindowPos();
	ViewportPos = { viewPos.x,viewPos.y };

	if (App->renderer3D->gameCam != nullptr)
	{
		App->renderer3D->gameCam->SetAspectRatio(GameViewSize.x, GameViewSize.y);

		if (App->renderer3D->gameCam->isActive)
		{
			ImGui::Image((ImTextureID)App->renderer3D->gameCam->textureColourBuffer, GameViewSize, ImVec2(0, 1), ImVec2(1, 0));
		}
	}

	if (ImGui::IsWindowHovered())
	{
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && !ImGuizmo::IsUsing() && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT)
		{
			origin.x = (ImGui::GetMousePos().x - viewPos.x) / GameViewSize.x;
			origin.y = (ImGui::GetMousePos().y - viewPos.y) / GameViewSize.y;
			origin.x = (origin.x - 0.5f) * 2;
			origin.y = -((origin.y - 0.5f) * 2);

			if (origin.x >= -1 && origin.x <= 1 && origin.y >= -1 && origin.y <= 1)
			{
				if (App->renderer3D->gameCam != nullptr)
				{
					pickingRay = App->renderer3D->gameCam->frustum.UnProjectLineSegment(origin.x, origin.y + 0.05);//TODO: no acaba d'anar fino
					App->camera->MousePickUI(pickingRay);
				}
			}
		}

	}

	ImGui::End();

	ImGui::PopStyleVar();
}

void ModuleEditor::TimeButtons()
{
	// Play Buttons
	ImGui::Begin("Play Time");

	if (TimeManager::state == TimeManager::PlayState::PLAY)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.5, 0.5, 1, 1 });
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.25, 0.25, 0.25, 1 });
	}

	string playButton;
	if (TimeManager::IsOnPlay())
	{
		playButton = "STOP";
	}
	else
	{
		playButton = "PLAY";
	}

	if (ImGui::Button(playButton.c_str()))//posar imatge play igual
	{
		changeTimeState = true;
		currentTimeState = TimeManager::PlayState::PLAY;
	}
	ImGui::PopStyleColor();

	ImGui::SameLine();

	if (TimeManager::state == TimeManager::PlayState::PAUSE)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.5, 0.5, 1, 1 });
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.25, 0.25, 0.25, 1 });
	}

	if (ImGui::Button("PAUSE") && TimeManager::IsOnGame())//posar imatge pause igual
	{
		changeTimeState = true;
		currentTimeState = TimeManager::PlayState::PAUSE;
	}
	ImGui::PopStyleColor();

	ImGui::SameLine();

	if (TimeManager::state == TimeManager::PlayState::STEP)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.5, 0.5, 1, 1 });
	}

	else
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.25, 0.25, 0.25, 1 });
	}

	if (ImGui::Button("STEP") && TimeManager::IsOnGame())//posar imatge step igual
	{
		changeTimeState = true;
		currentTimeState = TimeManager::PlayState::STEP;
	}
	ImGui::PopStyleColor();

	ImGui::SameLine();

	static float scale = 1.0f;
	ImGui::Text("Play Speed");
	ImGui::SameLine();
	ImGui::PushItemWidth(75);
	if (ImGui::SliderFloat("##", &scale, 0.1f, 10.0f, "%.2f")) {
		TimeManager::SetScale(scale);
	}

	ImGui::SameLine();
	ImGui::Text("Real time: %.2f", TimeManager::GetRealTime());
	ImGui::SameLine();
	ImGui::Text("Play Time: %.2f", TimeManager::GetGameTime());
	ImGui::End();
}

void ModuleEditor::OsOpenInShell(const char* path)
{
#ifdef _WIN32
	// Note: executable path must use backslashes!
	::ShellExecuteA(NULL, "open", path, NULL, NULL, SW_SHOWDEFAULT);
#else
#if __APPLE__
	const char* open_executable = "open";
#else
	const char* open_executable = "xdg-open";
#endif
	char command[256];
	snprintf(command, 256, "%s \"%s\"", open_executable, path);
	system(command);
#endif
}