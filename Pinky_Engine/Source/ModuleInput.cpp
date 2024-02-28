#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"

#include "ModuleResource.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
	keyboard = nullptr;
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("[ERROR] SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	maxChars = 10;
	getInput_B = false;
	strBU = "";
	strToChange = nullptr;

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for (int i = 0; i < 5; ++i)
	{
		if (buttons & SDL_BUTTON(i))
		{
			if (mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		//GUI input event
		ImGui_ImplSDL2_ProcessEvent(&e);

		switch (e.type)
		{
		case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

		case SDL_MOUSEMOTION:
		{
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
		}
		break;

		case SDL_QUIT:
			quit = true;
			break;

		case SDL_WINDOWEVENT:
		{
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				App->renderer3D->editorCam->OnResize(e.window.data1, e.window.data2);
				if (App->renderer3D->gameCam != nullptr)
				{
					App->renderer3D->gameCam->OnResize(e.window.data1, e.window.data2);
				}
			}
			break;
		}
		case SDL_KEYDOWN:
		{
			if (getInput_B)
			{
				HandleInput(e);
			}
		}
		break;

		case (SDL_DROPFILE):
		{      // In case if dropped file
			dropped_filedir = e.drop.file;
			// Shows directory of dropped file
			/*SDL_ShowSimpleMessageBox(
				SDL_MESSAGEBOX_INFORMATION,
				"File dropped on window", dropped_filedir, App->window->window);*/

			LOG("File dropped from: %s", dropped_filedir);

			App->resource->ImportFileToEngine(dropped_filedir);

			SDL_free(dropped_filedir);    // Free dropped_filedir memory
			break;
		}
		}

		// Close window if viewports enabled
		if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(App->window->window))
			quit = true;
	}

	if (quit == true)
	{
		LOG("Quit");
		return UPDATE_STOP;
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);

	strBU = "";
	strToChange = nullptr;
	return true;
}

// Handle user input
void ModuleInput::HandleInput(SDL_Event event)
{
	// If backspace was pressed and the string isn't blank
	if ((event.key.keysym.sym == SDLK_BACKSPACE))
	{
		if (!strToChange->empty())	// For logical purposes it can't be in the previous if
		{
			// Remove a character from the end
			strToChange->erase(strToChange->length() - 1);
		}
	}

	// Cancel and reset the string to the original
	else if((event.key.keysym.sym == SDLK_ESCAPE))
	{
		// Change back to previous string;
		*strToChange = strBU;
		getInput_B = false;
	}

	// Submit and change the corresponding string
	else if((event.key.keysym.sym == SDLK_RETURN && event.key.keysym.sym == SDLK_TAB) && !strToChange->empty())
	{
		//strToChange->erase(strToChange->length() - 1);
		getInput_B = false;
	}

	// Ignore shift
	else if(event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
	{	}
	
	// else ifthe string less than maximum size
	else if(strToChange->length() <= maxChars)
	{
		//Append the character
		*strToChange += (char)event.key.keysym.sym;
	}
}


bool ModuleInput::GetInputActive()
{
	return getInput_B;
}

void ModuleInput::SetInputActive(std::string& strToStore, bool getInput)
{
	// Keep a copy of the current version of the string
	strBU = strToStore;

	// Activate getting input
	getInput_B = getInput;

	strToChange = &strToStore;
}

void ModuleInput::SetMaxChars(int limit)
{
	maxChars = limit;
}
