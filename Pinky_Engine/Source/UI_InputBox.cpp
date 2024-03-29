#include "UI_InputBox.h"
#include "ModuleResource.h"

#include "GameObject.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"
#include "External Libraries/ImGui/misc/cpp/imgui_stdlib.h"
//
#include "External Libraries/ImGui/imgui_custom.h"

UI_InputBox::UI_InputBox(GameObject* g, int x, int y, int w, int h) : C_UI(UI_TYPE::INPUTBOX, C_TYPE::UI, g, "Input Box", x, y, w, h)
{
	displayText = nullptr;
	fontSize = 21;
	image = nullptr;

	focusedColor = { 0, 1, 0, 1 };
	pressedColor = { 1, 0, 1, 1 };
	selectedColor = { 1.0f, 0.081f, 1.0f, 1.0f };
	disabledColor = { 1, 1, 1, 1 };

	text = "uwu";
	maxChars = 10;

	isWriting = false;
}

UI_InputBox::~UI_InputBox()
{
	displayText = nullptr;
	image = nullptr;
}

update_status UI_InputBox::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	if (state == UI_STATE::NORMAL)
	{
		isWriting = false;
		App->input->SetInputActive(text, false);
	}
	
	if (text != displayText->text) { displayText->text = text; }
	return ret;
}

void UI_InputBox::ShowInInspector()
{
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

		ImGui::Checkbox("Draggeable", &isDraggable);

		ImGuiCustom::ToggleButton("Writing##", &isWriting);

		ImGui::Text("Text");
		ImGui::InputTextMultiline("##input", &text, ImVec2(0, 0), ImGuiInputTextFlags_AllowTabInput);

		ImGui::Dummy(ImVec2(0, 10));

		ImGui::Text("Characters limit");
		if (ImGui::DragInt("##Characters limit", (maxChars < 0) ? &(maxChars = 0) : &maxChars))
		{
			App->input->SetMaxChars(maxChars);
		}

		ImGui::ColorEdit4("Normal color", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Selected color", (float*)&selectedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Disabled color", (float*)&disabledColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

		if (!isActive) { ImGui::EndDisabled(); }
	}
	ImGui::SameLine();

	if (!exists) { gameObject->RemoveComponent(this); }
}

void UI_InputBox::OnNormal()
{
	image->color = color;
}

void UI_InputBox::OnFocused()
{
	image->color = focusedColor;
}

void UI_InputBox::OnPressed()
{
	image->color = pressedColor;
}

void UI_InputBox::OnSelected()
{
	if (!isWriting)
	{
		isWriting = true;
		App->input->SetInputActive(text);

		image->color = selectedColor;
	}
	else if (!App->input->GetInputActive())
	{
		isWriting = false;
		state = UI_STATE::NORMAL;
	}
}

void UI_InputBox::OnRelease()
{
}
