#include "UI_InputBox.h"
#include "ModuleResource.h"

#include "GameObject.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"
#include "External Libraries/ImGui/misc/cpp/imgui_stdlib.h"
//
#include "External Libraries/ImGui/imgui_custom.h"

UI_InputBox::UI_InputBox(GameObject* g) : C_UI(UI_TYPE::INPUTBOX, C_TYPE::UI, g, "Input Box")
{
	displayText = nullptr;
	fontSize = 21;

	maxChars = 10;
}

UI_InputBox::~UI_InputBox()
{
	displayText = nullptr;
}

void UI_InputBox::ShowInInspector()
{
	// TODO: delete after, just a test
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN && !ImGui::GetIO().WantTextInput)
		{
			LOG("Input");
			App->input->GetInputActive(text);
		}
	}
	//

	// --- Set ImGui ids ---
	std::string checkbox = name.c_str();
	std::string header = name.c_str();
	bool exists = true;

	checkbox.insert(checkbox.begin(), 2, '#');
	checkbox.append(std::to_string(GetUID()));

	header.append("##").append(std::to_string(GetUID()));
	// ---------------------------------------------

	ImGui::Checkbox(checkbox.c_str(), &isActive);		ImGui::SameLine();

	if (ImGui::CollapsingHeader(header.c_str(), &exists, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!isActive) { ImGui::BeginDisabled(); }

		if (ImGui::InputTextMultiline(name.c_str(), &text, ImVec2(0, 0), ImGuiInputTextFlags_AllowTabInput))
		{
			displayText->text = text;
		}

		ImGui::Dummy(ImVec2(0, 10));

		if (ImGui::DragInt("Characters limit", (maxChars < 0) ? &(maxChars = 0) : &maxChars))
		{
			App->input->SetMaxChars(maxChars);
		}

		ImGui::ColorEdit4("Normal color", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Disabled color", (float*)&disabledColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

		if (!isActive) { ImGui::EndDisabled(); }
	}
	ImGui::SameLine();

	if (!exists) { gameObject->RemoveComponent(this); }
}