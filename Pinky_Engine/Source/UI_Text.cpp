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

	fontSize = 21;
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
	int positionX = posX;

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = font->characters[*c];

		float xpos = positionX + ch.bearing.x * fontSize;
		float ypos = posY - (ch.size.y - ch.bearing.y) * fontSize;

		float w = ch.size.x * fontSize;
		float h = ch.size.y * fontSize;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		//---Render---
		if (game)
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0.0, App->editor->gameViewSize.x, App->editor->gameViewSize.y, 0.0, 1.0, -1.0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}

		else
		{
			glPushMatrix();
			glMultMatrixf(gameObject->transform->GetGLTransform());
		}	
		glEnableClientState(GL_VERTEX_ARRAY);
		//texture
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, font->VBO);
		//glVertexPointer(3, GL_FLOAT, 0, NULL);
		//glBindVertexArray(0);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);  

		glBindBuffer(GL_ARRAY_BUFFER, font->id_tex_uvs);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ch.textureID);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		positionX += (ch.advance >> 6) * fontSize; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
}

Font::Font(std::string name, std::string fontPath)
{
	InitFont(name, fontPath);

	// 128 --> number of characters in a font
	FT_Set_Pixel_Sizes(face, 0, 128);

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
        Character character = Character{
            texture,
            float2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            float2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };

        characters.insert(std::pair<GLchar, Character>(c, character));
    }

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

	//Init buffers
	index = new uint[6];
	index[0] = 0;
	index[1] = 1;
	index[2] = 2;
	index[3] = 2;
	index[4] = 1;
	index[5] = 3;

	uvs[2] = float2(0, 1);
	uvs[3] = float2(1, 1);
	uvs[1] = float2(1, 0);
	uvs[0] = float2(0, 0);

	VBO = 0;
	EBO = 0;
	id_tex_uvs = 0;

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &id_tex_uvs);

	if (VBO == 0 || EBO == 0 || id_tex_uvs == 0)
	{
		LOG("[ERROR] buffer not created");
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * 4 * 6, NULL, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 6, index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, id_tex_uvs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * 4, uvs, GL_STATIC_DRAW);
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
	//for (std::map<GLchar, Character>::const_iterator it = characters.begin(); it != characters.end(); it++)
	//{
	//	if ((*it).first == character)
	//	{
	//		GLuint id = (it).second->textureID;
	//		return id;
	//	}
	//}

	return 0;
}
