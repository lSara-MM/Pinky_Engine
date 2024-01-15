#include "UI_Canvas.h"
#include "ModuleScene.h"

UI_Canvas::UI_Canvas(GameObject* g, int w, int h) : C_UI(UI_TYPE::CANVAS, C_TYPE::UI, g, "Canvas")
{

}

UI_Canvas::~UI_Canvas()
{
	if (App->scene->GetCanvas() == gameObject)
	{
		App->scene->SetCanvas();
	}
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

void UI_Canvas::Draw(bool game)
{
	if (!game)
	{
		glBegin(GL_LINE_LOOP);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		float3 v1 = float3(posX, posY, 0);
		float3 v2 = float3(posX + width, posY, 0);
		float3 v3 = float3(posX, posY + height, 0);
		float3 v4 = float3(posX + width, posY + height, 0);

		glVertex3f(v1.x, v1.y, v1.z);
		glVertex3f(v2.x, v2.y, v2.z);
		glVertex3f(v4.x, v4.y, v4.z);
		glVertex3f(v3.x, v3.y, v3.z);

		glEnd();
	}
}

void UI_Canvas::UpdateUITransform()
{
	gameObject->transform->SetTransform(float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1));
	
	width = App->editor->gameViewSize.x;
	height = App->editor->gameViewSize.y;

	boundsEditor->vertex[0] = float3(posX, posY, 0);
	boundsEditor->vertex[1] = float3(posX + width, posY, 0);
	boundsEditor->vertex[2] = float3(posX, posY + height, 0);
	boundsEditor->vertex[3] = float3(posX + width, posY + height, 0);

	boundsEditor->uvs[2] = float2(0, 1);
	boundsEditor->uvs[3] = float2(1, 1);
	boundsEditor->uvs[1] = float2(1, 0);
	boundsEditor->uvs[0] = float2(0, 0);

	boundsEditor->InitBuffers();
}

