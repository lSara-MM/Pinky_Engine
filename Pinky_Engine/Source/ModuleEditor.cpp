#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "../SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_sdl2.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"

#ifdef _DEBUG
#pragma comment (lib, "MathGeoLib/libx86/libDebug/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#else
#pragma comment (lib, "MathGeoLib/libx86/libRelease/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#endif // _DEBUG


ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
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
	ImGui::StyleColorsDark(); // dark mode default :D

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init("#version 130");

	// 2nd window state
	show_demo_window = true;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	aboutColor = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
	aboutWin = false;
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

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	//ImGui::ShowDemoWindow();

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Exit"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			return UPDATE_STOP;
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		//TODO: FPS HISTOGRAM IN
		FpsWindow(io);

		ImGui::End();
	}

	//LOG window, not working (error log.cpp)
	OutputWindow();

	//Configwindow, change to fullscreen and such
	ConfigWindow();

	App->HardwareDetection();

	ret = Toolbar();

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
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

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(context);
	return true;
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
			if (ImGui::MenuItem("Save", "Ctrl+S")) {}
			if (ImGui::MenuItem("Save As..")) {}

			ImGui::Separator();
			if (ImGui::BeginMenu("Options"))
			{
				static bool enabled = true;
				ImGui::MenuItem("Enabled", "", &enabled);
				
				ImGui::BeginChild("child", ImVec2(0, 60), true);
				for (int i = 0; i < 10; i++)
					ImGui::Text("Scrolling Text %d", i);
				ImGui::EndChild();
				
				static float f = 0.5f;
				static int n = 0;
				ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
				ImGui::InputFloat("Input", &f, 0.1f);
				ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");

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
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			float sz = ImGui::GetTextLineHeight();
			ImVec2 p = ImGui::GetCursorScreenPos();
			
			ImGui::ColorPicker3("a", col);

			if (ImGui::MenuItem("Exit"))
			{
				ImGui::EndMenu();
				ImGui::EndMainMenuBar();
				return UPDATE_STOP;
			}

			ImGui::EndMenu();
		}

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

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Module Settings")) {}
			if (ImGui::MenuItem("Information Output")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("About")) { aboutWin = true; }
			ImGui::EndMenu();
		}

		About();

		ImGui::EndMainMenuBar();
	}

	return UPDATE_CONTINUE;
}

void ModuleEditor::ConfigWindow()
{
	{
		ImGui::Begin("Configuration");
		if (ImGui::CollapsingHeader("Window"))
		{
			if (ImGui::Checkbox("Fullscreen", &App->window->fullScreen))
			{
				(!App->window->fullScreen) ? App->window->flags = SDL_WINDOW_SHOWN : App->window->flags = SDL_WINDOW_FULLSCREEN;

				SDL_SetWindowFullscreen(App->window->window, App->window->flags);
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Change fullscreen window");//TODO: noms millorables?
			}
			ImGui::SameLine();

			if (ImGui::Checkbox("Resizable", &App->window->resizable))
			{
				SDL_SetWindowResizable(App->window->window, (SDL_bool)App->window->resizable);
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Change resizable window");
			}

			if (ImGui::Checkbox("Borderless", &App->window->borderless))
			{
				SDL_SetWindowResizable(App->window->window, (SDL_bool)App->window->borderless);
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Change borderless window");
			}
			ImGui::SameLine();

			if (ImGui::Checkbox("Full desktop", &App->window->fullScreenDesktop))
			{
				(!App->window->fullScreenDesktop) ? App->window->flags = SDL_WINDOW_SHOWN : App->window->flags = SDL_WINDOW_FULLSCREEN_DESKTOP;

				SDL_SetWindowFullscreen(App->window->window, App->window->flags);
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Change fullscreen desktop window");
			}
			ImGui::Text("Window size: %d x %d", SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);

			ImGui::Text("Brightness: %f", SDL_GetWindowBrightness(App->window->window));

			//Todo: Vsync no està implementat, esperar per si profe explica
			/*if (ImGui::Checkbox("Full desktop", &App->window->fullScreenDesktop))
			{
				(vSync_B) ? flags = SDL_RENDERER_ACCELERATED : flags |= SDL_RENDERER_PRESENTVSYNC;
			}*/
			ImGui::Text("Mouse position: %d x, %d y", App->input->GetMouseX(), App->input->GetMouseY());
			//TODO: Input keys¿? Audio current volumes¿?
		}
		ImGui::End();
	}
}

void ModuleEditor::OutputWindow()
{
	//Log window, falta que app funcione en log
	{
		ImGui::Begin("Console", NULL, ImGuiWindowFlags_AlwaysVerticalScrollbar);
		ImGui::BeginChild("##output");
		for (int n = 0; n < logVec.size(); n++)
		{
			ImGui::Text(logVec[n].c_str(), n);
		}
		ImGui::EndChild();
		ImGui::End();
	}
}

void ModuleEditor::FpsWindow(ImGuiIO& io)
{
	char title[25];
	AddFPS(mFPSLog, io.Framerate);
	AddFPS(mSLog, 1000.0f / io.Framerate);
	sprintf_s(title, 25, "Framerate %.1f", mFPSLog[mFPSLog.size() - 1]);
	ImGui::PlotHistogram("##framerate ", &mFPSLog[0], mFPSLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100.0f));
	sprintf_s(title, 25, "Milliseconds %0.1f", mSLog[mSLog.size() - 1]);
	ImGui::PlotHistogram("##milliseconds ", &mSLog[0], mSLog.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100.0f));
}

void ModuleEditor::About()
{
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP)
	{
		LOG("hola");
	}

	if (aboutWin)
	{
		// Open a modal pop up to show info about the project
		ImGui::OpenPopup("About");
		aboutWin = false;
	}

	if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::BeginChild("left pane", ImVec2(150, 0), true);
		ImGui::TextWrapped("MIT LicenseCopyright\n(c) 2023\n");
		ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copyof this software and associated documentation files (the 'Software'), to dealin the Software without restriction, including without limitation the rightsto use, copy, modify, merge, publish, distribute, sublicense, and/or sellcopies of the Software, and to permit persons to whom the Software isfurnished to do so, subject to the following conditions:The above copyright notice and this permission notice shall be included in allcopies or substantial portions of the Software.THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS ORIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THEAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHERLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THESOFTWARE.");
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::BeginChild("right pane", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us

		ImGui::Text("Pinky Engine");
		ImGui::Separator();
		ImGui::TextColored(aboutColor, "lSara-MM & AndyCubico");
		ImGui::ColorEdit3("", (float*)&aboutColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::EndChild();


		if (ImGui::Button("GitHub")) { OsOpenInShell("https://github.com/lSara-MM/Pinky_Engine"); }
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
		ImGui::EndGroup();
		ImGui::EndPopup();
	}
}

void ModuleEditor::AddFPS(std::vector<float>& vect, const float aFPS)
{
	if (vect.size() < 60)
	{
		vect.push_back(aFPS);
	}
	else
	{
		std::rotate(vect.begin(), vect.begin() + 1, vect.end());//TODO: ta bien?
		vect.pop_back();
		vect.push_back(aFPS);
	}
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