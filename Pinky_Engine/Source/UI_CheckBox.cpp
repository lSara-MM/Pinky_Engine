#include "UI_Checkbox.h"
#include "GameObject.h"

UI_Checkbox::UI_Checkbox(GameObject* g) : C_UI(UI_TYPE::CHECKBOX, C_TYPE::UI, g, "Checkbox")
{
	isInteractable = true;
	isChecked = true;

	focusedColor = { 0, 1, 0, 1 };//TODO: posa checkbox black?
	pressedColor = { 1, 0, 1, 1 };
	selectedColor = { 0, 0, 1, 1 };
	disabledColor = { 1, 1, 1, 1 };

	bgImg = nullptr;
	cmImg = nullptr;
}

UI_Checkbox::~UI_Checkbox()
{
	// Image is already another GameObject
	bgImg = nullptr;
	cmImg = nullptr;
}

void UI_Checkbox::ShowInInspector()
{
	// --- Set ImGui ids ---
	std::string checkbox = name.c_str();
	std::string header = name.c_str();
	bool exists = true;

	checkbox.insert(checkbox.begin(), 2, '#');
	checkbox.append(std::to_string(GetUID()));

	header.append("##").append(std::to_string(GetUID()));
	// ---------------------------------------------

	ImGui::Checkbox(checkbox.c_str(), &isActive);	ImGui::SameLine();

	if (ImGui::CollapsingHeader(header.c_str(), &exists, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (!isActive) { ImGui::BeginDisabled(); }

		ImGui::Dummy(ImVec2(0, 10));

		ImGui::Checkbox("Interactable", &isInteractable);
		if(ImGui::Checkbox("Checked", &isChecked))
		{
			cmImg->isActive = isChecked;
		}

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

void UI_Checkbox::OnNormal()
{
	bgImg->color = color;
}

void UI_Checkbox::OnFocused()
{
	bgImg->color = focusedColor;
}

void UI_Checkbox::OnPressed()
{
	bgImg->color = pressedColor;
}

void UI_Checkbox::OnSelected()
{
	bgImg->color = selectedColor;
}

void UI_Checkbox::OnRelease()
{
	if (isInteractable)
	{
		isChecked = !isChecked;
		bgImg->isActive = isChecked;
	}
}