#include "UI_Canvas.h"

UI_Canvas::UI_Canvas(GameObject* g) : C_UI(C_TYPE::UI, g, "Canvas")
{
}

UI_Canvas::~UI_Canvas()
{
}

void UI_Canvas::ShowInInspector()
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

		ImGui::Text(name.c_str());

		if (!isActive) { ImGui::EndDisabled(); }
	}
	ImGui::SameLine();

	if (!exists) { gameObject->RemoveComponent(this); }
}
