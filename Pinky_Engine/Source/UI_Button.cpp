#include "UI_Button.h"
#include "GameObject.h"

UI_Button::UI_Button(GameObject* g) : C_UI(UI_TYPE::BUTTON, C_TYPE::UI, g, "Button")
{
	isInteractable = true;
	normalColor = { 1,1,0,1 };
	focusedColor = { 0,1,0,1 };
	pressedColor = { 1,0,1,1 };
	selectedColor = { 0,0,1,1 };
	disabledColor = { 1,1,1,1 };
}

UI_Button::~UI_Button()
{
}

void UI_Button::ShowInInspector()
{	// --- Set ImGui ids ---
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

		ImGui::Dummy(ImVec2(0, 10));

		ImGui::Checkbox("Interactable##1", &isInteractable);

		ImGui::ColorEdit4("Normal color", (float*)&normalColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Focused color", (float*)&focusedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Pressed color", (float*)&pressedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Selected color", (float*)&selectedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Disabled color", (float*)&disabledColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

		if (!isActive) { ImGui::EndDisabled(); }
	}
	ImGui::SameLine();

	if (!exists) { gameObject->RemoveComponent(this); }
}

void UI_Button::OnNormal()
{
	color = normalColor;
}

void UI_Button::OnFocused()
{
	color = focusedColor;
}

void UI_Button::OnPressed()
{
	color = pressedColor;
}

void UI_Button::OnRelease()
{
	LOG("get released");
}

void UI_Button::OnSelected()
{
	color = selectedColor;
}

