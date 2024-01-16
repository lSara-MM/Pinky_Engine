#include "UI_Button.h"
#include "GameObject.h"
#include "G_UI.h"

#include "UI_Canvas.h"

UI_Button::UI_Button(GameObject* g, int x, int y, int w, int h) : C_UI(UI_TYPE::BUTTON, C_TYPE::UI, g, "Button", x, y, w, h)
{
	isInteractable = true;

	focusedColor = { 0, 1, 0, 1 };
	pressedColor = { 1, 0, 1, 1 };
	selectedColor = { 0, 0, 1, 1 };
	disabledColor = { 1, 1, 1, 1 };

	image = nullptr;
	defaultFunction = false;
	displayText = nullptr;
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
		ImGui::Checkbox("Draggeable", &isDraggable);

		ImGui::Dummy(ImVec2(0, 10));
		ImGui::Text(text.c_str());
		ImGui::Dummy(ImVec2(0, 10));

		ImGui::ColorEdit4("Normal color", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Focused color", (float*)&focusedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Pressed color", (float*)&pressedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Selected color", (float*)&selectedColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4("Disabled color", (float*)&disabledColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

		{
			ImGui::Dummy(ImVec2(0, 10));
			ImGui::Text("Funtion: "); ImGui::SameLine();
			if (defaultFunction)
			{
				ImGui::Text("void FadeUI(float dt)");
			}
			else
			{
				ImGui::Text("<null>");
			}
		}

		if (!isActive) { ImGui::EndDisabled(); }
	}
	ImGui::SameLine();

	if (!exists) { gameObject->RemoveComponent(this); }
}

void UI_Button::OnNormal()
{
	image->color = color;
}

void UI_Button::OnFocused()
{

	image->color = focusedColor;
}

void UI_Button::OnPressed()
{
	image->color = pressedColor;
}

void UI_Button::OnSelected()
{
	image->color = selectedColor;
}

void UI_Button::OnRelease()
{
	if (defaultFunction)
	{
		static_cast<G_UI*>(gameObject)->canvas->fade = true;
	}
}
