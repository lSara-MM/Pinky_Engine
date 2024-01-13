#include "UI_Text.h"
#include "ModuleResource.h"

#include "GameObject.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/backends/imgui_impl_sdl2.h"
#include "External Libraries/ImGui/backends/imgui_impl_opengl3.h"
#include "External Libraries/ImGui/misc/cpp/imgui_stdlib.h"
//
#include "External Libraries/ImGui/imgui_custom.h"

//#include "STB_Text.h"

UI_Text::UI_Text(GameObject* g) : C_UI(UI_TYPE::TEXT, C_TYPE::UI, g, "Text")
{
	text = "Hello World";
	fontSize = 36;
}

UI_Text::~UI_Text()
{
}

void UI_Text::ShowInInspector()
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

		ImGui::InputTextMultiline(name.c_str(), &text, ImVec2(0, 0), ImGuiInputTextFlags_AllowTabInput);
		//ImGui::InputText(name.c_str(), &text, ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::Dummy(ImVec2(0, 10));

		ImGui::ColorEdit4("Color", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

		if (!isActive) { ImGui::EndDisabled(); }
	}
	ImGui::SameLine();

	if (!exists) { gameObject->RemoveComponent(this); }
}

void UI_Text::Draw(bool game)
{
	//App->renderer3D->stb_text->Draw(text, 150, 150);
}
