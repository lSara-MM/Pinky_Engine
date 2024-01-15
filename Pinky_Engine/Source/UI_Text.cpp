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

UI_Text::UI_Text(GameObject* g, int w, int h, int x, int y) : C_UI(UI_TYPE::TEXT, C_TYPE::UI, g, "Text", w, h, x, y)
{
	text = "Hello World";
	font = App->renderer3D->defaultFont;
	
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

		ImGui::Checkbox("Draggeable", &draggable);

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
	UIBounds* boundsDrawn = nullptr;

	if (game)
	{
		boundsDrawn = boundsGame;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, App->editor->gameViewSize.x, App->editor->gameViewSize.y, 0.0, 1.0, -1.0);//TODO: orginal con 0,0 bien en pantalla pero mueve al revés
		//glOrtho(App->editor->GameViewSize.x, 0.0, 0.0, App->editor->GameViewSize.y, 1.0, -1.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	else
	{
		boundsDrawn = boundsEditor;

		glPushMatrix();
		glMultMatrixf(gameObject->transform->GetGLTransform());
	}

	glEnableClientState(GL_VERTEX_ARRAY);

	//normals
	glEnableClientState(GL_NORMAL_ARRAY);

	//texture
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// Mesh buffers
	glBindBuffer(GL_ARRAY_BUFFER, boundsDrawn->VBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boundsDrawn->EBO);

	//alpha material
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	glColor4f(color.r, color.g, color.b, color.a);

	// Textures
	glBindBuffer(GL_ARRAY_BUFFER, boundsDrawn->id_tex_uvs);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glActiveTexture(GL_TEXTURE0);

	GLuint a = font->GetCharacterTexID('p');
	glBindTexture(GL_TEXTURE_2D, a);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	// Clean textures
	glBindTexture(GL_TEXTURE_2D, 0); // Cleanning bind buffer;
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if (!game)
	{
		glPopMatrix();
	}

	boundsDrawn = nullptr;
}

Font::Font(std::string name, std::string fontPath, int size)
{
	InitFont(name, fontPath);

	fontSize = size;
	FT_Set_Pixel_Sizes(face, 0, fontSize);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
			LOG("[ERROR] Failed to load glyph");
            continue;
        }
        // generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character* character = new Character{
            texture,
            float2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            float2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<GLchar, Character*>(c, character));
    }

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

}

bool Font::InitFont(std::string name, std::string fontPath)
{
    if (FT_Init_FreeType(&ft))
    {
        LOG("[ERROR] Could not init FreeType Library");
        return false;
    }

	if (FT_New_Face(ft, (fontPath + "/" + name).c_str(), 0, &face))
	{
		LOG("[ERROR] Failed to load font");
		return false;
	}
	return true;
}

GLuint Font::GetCharacterTexID(GLchar character)
{
	for (std::map<GLchar, Character*>::const_iterator it = Characters.begin(); it != Characters.end(); it++)
	{
		if ((*it).first == character)
		{
			GLuint id = (*it).second->TextureID;
			return id;
		}
	}

	return GLuint();
}
