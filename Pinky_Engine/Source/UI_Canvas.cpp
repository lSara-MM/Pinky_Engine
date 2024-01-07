#include "UI_Canvas.h"

UI_Canvas::UI_Canvas(GameObject* g, int w, int h) : C_UI(C_TYPE::UI, g, "Canvas")
{
	canvasWidth = w;
	canvasHeight = h;
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

void UI_Canvas::Draw()
{
	glBegin(GL_LINE_LOOP);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	float3 position = gameObject->transform->position;

	float3 v1 = float3(position.x, position.y, position.z);
	float3 v2 = float3(position.x + canvasWidth, position.y, position.z);
	float3 v3 = float3(position.x, position.y + canvasHeight, position.z);
	float3 v4 = float3(position.x + canvasWidth, position.y + canvasHeight, position.z);

	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v4.x, v4.y, v4.z);
	glVertex3f(v3.x, v3.y, v3.z);

	glEnd();
}
