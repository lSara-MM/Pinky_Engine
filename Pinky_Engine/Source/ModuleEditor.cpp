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
//
#include "../ImGui/imgui_custom.h"

#ifdef _DEBUG
#pragma comment (lib, "MathGeoLib/libx86/libDebug/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#else
#pragma comment (lib, "MathGeoLib/libx86/libRelease/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#endif // _DEBUG


#include "ModuleScene.h"
#include "GameObject.h"

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
	ImGui::StyleColorsClassic(); // dark mode default :D
	//ImGuiCustom::Theme_Cinder();
	//ImGuiCustom::Theme_EnemyMouse();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init("#version 130");

	// Module Settings
	moduleSettingsWin = false;
	frcap = true;

	//// OpenGL configuration
	depthTest = true;
	cullFace = true;
	lightning = true;
	colorMaterial = true;
	texture2D = true;
	blend = true;
	lineSmooth = true;
	normalize = true;

	//
	infoOutputWin = false;

	// About
	aboutColor = ImVec4(0.953f, 0.533f, 0.969f, 1.0f);
	aboutWin = false;

	Hierarchy* h = new Hierarchy(vImGuiWindows.size());
	App->scene->h = h;
	vImGuiWindows.push_back(h);
	h = nullptr;
	Inspector* i = new Inspector(vImGuiWindows.size());
	App->scene->i = i;
	vImGuiWindows.push_back(i);
	i = nullptr;

	// Load Baker House at the start by default
	App->scene->BakerHouse();
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
	App->renderer3D->statsVRAM = m_getMemoryStatistics();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	//ImGui::ShowDemoWindow();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ret = Toolbar();
	
	//LOG window
	LogWindow();

	//Configwindow, change to fullscreen and such
	ConfigWindow(io);
	App->renderer3D->HardwareDetection(infoOutputWin);
	
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

	ClearVec(mFPSLog);
	ClearVec(mSLog);
	ClearVec(vLog);
	ClearVec(MemLog);

	ClearVecPtr(vImGuiWindows);

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

		if (ImGui::BeginMenu("Game Object"))
		{
			if (ImGui::MenuItem("Create Empty")) { new GameObject("Empty"); }

			if (ImGui::BeginMenu("Create Primitive"))
			{
				std::array<std::string, 4> components = { "Cube", "Sphere", "Cylinder", "Plane" };

				for (int i = 0; i < components.size(); i++)
				{
					if (ImGui::MenuItem(components[i].c_str()))
					{
						ai::CreatePolyPrimitive(ai::POLY_PRIMITIVE_TYPE(i));
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Hierarchy"))
			{
				if (App->scene->h != nullptr)
				{
					App->scene->h->show = !App->scene->h->show;
				}				
				//vImGuiWindows.push_back(new Hierarchy(vImGuiWindows.size()));
			}
			if (ImGui::MenuItem("Inspector"))
			{
				if (App->scene->i != nullptr)
				{
					App->scene->i->show = !App->scene->i->show;
				}
				//vImGuiWindows.push_back(new Inspector(vImGuiWindows.size()));
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Module Settings")) { moduleSettingsWin = true; }
			if (ImGui::MenuItem("Information Output")) { infoOutputWin = true; }
			ImGui::Separator();
			if (ImGui::MenuItem("About")) { aboutWin = true; }
			ImGui::EndMenu();
		}

		AboutWindow();

		ImGui::Checkbox("Wireframe", &App->renderer3D->wireframe);

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
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.7f));
		ImGui::SetNextWindowSize(ImVec2(500, 200));
		ImGui::Begin("Configuration", &moduleSettingsWin, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		
		static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton;
		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Window"))
			{
				if (ImGuiCustom::ToggleButton("Fullscreen", &App->window->fullScreen))
				{
					(!App->window->fullScreen) ? App->window->flags = SDL_WINDOW_SHOWN : App->window->flags = SDL_WINDOW_FULLSCREEN;

					SDL_SetWindowFullscreen(App->window->window, App->window->flags);
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
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Change borderless window");
				}
				ImGui::SameLine(150);

				if (ImGuiCustom::ToggleButton("Full desktop", &App->window->fullScreenDesktop))
				{
					(!App->window->fullScreenDesktop) ? App->window->flags = SDL_WINDOW_SHOWN : App->window->flags = SDL_WINDOW_FULLSCREEN_DESKTOP;

					SDL_SetWindowFullscreen(App->window->window, App->window->flags);
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Change fullscreen desktop window");
				}
				ImGui::Separator();

				ImGui::Text("Window size: %d x %d", SDL_GetWindowSurface(App->window->window)->w, SDL_GetWindowSurface(App->window->window)->h);
				if (ImGui::SliderInt("Width", &App->window->width, 0, 1920, "%d"))
				{
					SDL_SetWindowSize(App->window->window, App->window->width, App->window->height);
				}

				if (ImGui::SliderInt("Height", &App->window->height, 0, 1920, "%d"))
				{
					SDL_SetWindowSize(App->window->window, App->window->width, App->window->height);
				}
				ImGui::Separator();
				
				ImGui::Text("Brightness: %.2f", SDL_GetWindowBrightness(App->window->window));
				if (ImGui::SliderFloat("Brightness", &App->window->brightness, 0.2350f, 1.0f, "%.2f"))
				{
					SDL_SetWindowBrightness(App->window->window, App->window->brightness);//TODO: falla a vegades
				}
				ImGui::Separator();

				ImGui::Text("Mouse position: %d x, %d y", App->input->GetMouseX(), App->input->GetMouseY());
				ImGui::Separator();

				FpsWindow(io);

				ImGui::Checkbox("FPS Cap", &App->frcap);
				ImGui::SliderInt("FPS cap", &App->fps, 1, 120, "%d");
				ImGui::Separator();

				ImGui::Checkbox("VSync", &App->renderer3D->Vsync);

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

			if(ImGui::BeginTabItem("Style Editor"))
			{
				ImGui::ShowStyleEditor();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::End();
	}
}

void ModuleEditor::LogWindow()
{
	//Log window
	ImGui::SetNextWindowSize(ImVec2(500, 200), ImGuiCond_FirstUseEver);
	ImGui::Begin("Console", NULL, ImGuiWindowFlags_MenuBar);
	ImGui::BeginMenuBar();
	if (ImGui::Button("Clear"))
	{
		ClearVec(vLog);
	}
	if (ImGui::Button("Add Error message"))
	{
		vLog.push_back("[ERROR] debug error message");
	}
	if (ImGui::Button("Add Warning message"))
	{
		vLog.push_back("[WARNING] debug warning message");
	}
	ImGui::Dummy(ImVec2(ImGui::GetFontSize() * 20, 0));
	static ImGuiTextFilter filter;
	filter.Draw("Search", ImGui::GetFontSize() * 15);

	ImGui::EndMenuBar();

	//const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	ImGui::BeginChild("##output", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	
	for (int n = 0; n < vLog.size(); n++)
	{
		if (filter.PassFilter(vLog[n].c_str()))
		{
			if (strstr(vLog[n].c_str(), "[ERROR")) { ImGui::TextColored(ImVec4(0.9f, 0.0f, 0.0f, 1.0f), vLog[n].c_str()); }
			else if (strstr(vLog[n].c_str(), "[WARNING")) { ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), vLog[n].c_str()); }
			else { ImGui::Text(vLog[n].c_str(), n); }
		}
	}

	// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
	// Using a scrollbar or mouse-wheel will take away from the bottom edge.
	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) { ImGui::SetScrollHereY(1.0f); }

	ImGui::EndChild();
	ImGui::End();
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
	AddMem(MemLog, App->renderer3D->statsVRAM.peakReportedMemory);
	ImGui::Text("Reported Memory % 0.1f", MemLog[MemLog.size() - 1]);
	ImGui::PlotHistogram("", &MemLog[0], MemLog.size(), 0, "", 0.0f, 40.0f, ImVec2(310, 100.0f));
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
		ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copyof this software and associated documentation files (the 'Software'), to dealin the Software without restriction, including without limitation the rightsto use, copy, modify, merge, publish, distribute, sublicense, and/or sellcopies of the Software, and to permit persons to whom the Software isfurnished to do so, subject to the following conditions:The above copyright notice and this permission notice shall be included in allcopies or substantial portions of the Software.THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS ORIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THEAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHERLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THESOFTWARE.");
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::BeginChild("right pane", ImVec2(500-150, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us

		if (ImGui::BeginTabBar("Pinky Engine"), ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton)
		{
			if (ImGui::BeginTabItem("Pinky Engine"))
			{
				ImGui::TextColored(aboutColor, "lSara-MM & AndyCubico");
				ImGui::ColorEdit3("", (float*)&aboutColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				ImGui::EndTabItem();
			}
			
			if (ImGui::BeginTabItem("Libraries"))
			{
				//---ImGui---
				if (ImGui::Button("ImGui")) { OsOpenInShell(""); } ImGui::SameLine();
				ImGui::Text("Omar Cornut Copyright (c) 2014-2023"); 

				//---OpenGL---
				if (ImGui::Button("OpenGL")) { OsOpenInShell(""); } ImGui::SameLine();
				ImGui::Text("Omar Cornut Copyright (c) 2014-2023"); 
				ImGui::EndTabItem();
			}
			
			if (ImGui::BeginTabItem("External Sources"))
			{
				ImGui::TextColored(aboutColor, "lSara-MM & AndyCubico");
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