#include "UI_Button.h"
#include "GameObject.h"

UI_Button::UI_Button(GameObject* g, int w, int h, int x, int y) : C_UI(UI_TYPE::BUTTON, C_TYPE::UI, g, "Button", w, h, x, y)
{
	isInteractable = true;

	focusedColor = { 0, 1, 0, 1 };
	pressedColor = { 1, 0, 1, 1 };
	selectedColor = { 0, 0, 1, 1 };
	disabledColor = { 1, 1, 1, 1 };

	image = nullptr;
}

UI_Button::~UI_Button()
{
	// Image is already another GameObject
	image = nullptr;
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

		if (ImGui::Checkbox("Interactable", &isInteractable))
		{
			if (!isInteractable)
			{
				image->color = disabledColor;
			}
		} ImGui::SameLine();
		ImGui::Checkbox("Draggeable", &draggable);

		ImGui::ColorEdit4("Normal color", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
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
	draggable = false;
	image->fade = false;
	if (!image->fade)
	{
		image->color = color;
	}
	ExecuteFunction(normalFunction);
}

void UI_Button::OnFocused()
{
	if (!image->fade)
	{
		image->color = focusedColor;
	}
	ExecuteFunction(focusedFunction);
}

void UI_Button::OnPressed()
{
	draggable = true;
	image->fade = true;

	if (!image->fade)
	{
		image->color = pressedColor;//TODO: prueba fade
	}
	ExecuteFunction(pressedFunction);
}

void UI_Button::OnSelected()
{
	draggable = false;
	image->fade = false;

	if (!image->fade)
	{
		image->color = selectedColor;
	}
	ExecuteFunction(selectedFunction);
}

void UI_Button::OnRelease()
{
	//function.ExecuteFunction();
}

void UI_Button::ExecuteFunction(const FunctionType& func) {
	if (func) {
		func();
	}
	else {
		std::cout << "No function set." << std::endl;
	}
}