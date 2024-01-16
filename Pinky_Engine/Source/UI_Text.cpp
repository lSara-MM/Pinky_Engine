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

UI_Text::UI_Text(GameObject* g, int x, int y, int w, int h) : C_UI(UI_TYPE::TEXT, C_TYPE::UI, g, "Text", x, y, w, h)
{
	text = "Hello World";
	font = App->renderer3D->defaultFont;

	boundsEditor = new UIBounds;
	boundsGame = new UIBounds;

	boundsEditor->index = new uint[6];
	boundsEditor->index[0] = 0;
	boundsEditor->index[1] = 1;
	boundsEditor->index[2] = 2;
	boundsEditor->index[3] = 2;
	boundsEditor->index[4] = 1;
	boundsEditor->index[5] = 3;

	boundsGame->index = new uint[6];
	boundsGame->index[0] = 0;
	boundsGame->index[1] = 1;
	boundsGame->index[2] = 2;
	boundsGame->index[3] = 2;
	boundsGame->index[4] = 1;
	boundsGame->index[5] = 3;

	boundsEditor->uvs[3] = float2(1, 1);
	boundsEditor->uvs[2] = float2(0, 1);
	boundsEditor->uvs[1] = float2(1, 0);
	boundsEditor->uvs[0] = float2(0, 0);

	boundsGame->uvs[3] = float2(1, 1);
	boundsGame->uvs[2] = float2(0, 1);
	boundsGame->uvs[1] = float2(1, 0);
	boundsGame->uvs[0] = float2(0, 0);

	boundsEditor->InitBuffers();
	boundsGame->InitBuffers();

	fontSize = 21;
	space = 0;
}

UI_Text::~UI_Text()
{
	font = nullptr;
	boundsEditor->DeleteBuffers();
	boundsGame->DeleteBuffers();
	RELEASE_ARRAY(boundsEditor->index);
	RELEASE_ARRAY(boundsGame->index);
	RELEASE(boundsEditor);
	RELEASE(boundsGame);
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

		ImGui::Checkbox("Draggeable", &isDraggable);

		ImGui::InputTextMultiline(name.c_str(), &text, ImVec2(0, 0), ImGuiInputTextFlags_AllowTabInput);
		//ImGui::InputText(name.c_str(), &text, ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::Dummy(ImVec2(0, 10));

		ImGui::Text("Font Size");
		if (ImGui::DragFloat("##FontSize", (fontSize < 0) ? &(fontSize = 0) : &fontSize, 0.1f, 0, 0, "%.1f"))
		{
		}
		ImGui::ColorEdit4("Color", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

		if (!isActive) { ImGui::EndDisabled(); }
	}
	ImGui::SameLine();

	if (!exists) { gameObject->RemoveComponent(this); }
}

void UI_Text::Draw(bool game)
{
	UIBounds* boundsDrawn = nullptr;
	space = 0;

	for (size_t i = 0; i < text.length(); i++)
	{
		float3 position = gameObject->transform->position;

		auto itr = font->mCharacters.find(text[i]);

		if (itr != font->mCharacters.end())
		{
			if (i != 0)
			{
				auto itr2 = font->mCharacters.find(text[i - 1]);
				space += itr2->second->size.x;
			}

			if (itr->first == ' ')
			{
				space += fontSize;
			}

			boundsEditor->vertex[0] = float3(position.x + space, position.y + (fontSize * scaleBounds.y), 0);
			boundsEditor->vertex[1] = float3(position.x + space + (itr->second->size.x * scaleBounds.x), position.y + (fontSize * scaleBounds.y), 0);
			boundsEditor->vertex[2] = float3(position.x + space, position.y, 0);
			boundsEditor->vertex[3] = float3(position.x + space + (itr->second->size.x * scaleBounds.x), position.y, 0);

			boundsGame->vertex[0] = float3(posX + space, posY + (fontSize * scaleBounds.y), 0);
			boundsGame->vertex[1] = float3(posX + space + (itr->second->size.x * scaleBounds.x), posY + (fontSize * scaleBounds.y), 0);
			boundsGame->vertex[2] = float3(posX + space, posY, 0);
			boundsGame->vertex[3] = float3(posX + space + (itr->second->size.x * scaleBounds.x), posY, 0);

			boundsEditor->RegenerateVBO();
			boundsGame->RegenerateVBO();

			if (game)
			{
				boundsDrawn = boundsGame;

				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(0.0, App->editor->gameViewSize.x, 0.0, App->editor->gameViewSize.y, 1.0, -1.0);

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

			glBindTexture(GL_TEXTURE_2D, itr->second->textureID);

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
		}
	}

	boundsDrawn = nullptr;
}

void UI_Text::ChangeFontSize()
{
	UIBounds* boundsDrawn = nullptr;
	space = 0;

	for (size_t i = 0; i < text.length(); i++)
	{
		float3 position = gameObject->transform->position;

		auto itr = font->mCharacters.find(text[i]);

		if (itr != font->mCharacters.end())
		{
			if (i != 0)
			{
				auto itr2 = font->mCharacters.find(text[i - 1]);
				space += itr2->second->size.x;
			}

			if (itr->first == ' ')
			{
				space += fontSize;
			}

			boundsEditor->vertex[0] = float3(position.x + space, position.y + (fontSize * scaleBounds.y), 0);
			boundsEditor->vertex[1] = float3(position.x + space + (itr->second->size.x * scaleBounds.x), position.y + (fontSize * scaleBounds.y), 0);
			boundsEditor->vertex[2] = float3(position.x + space, position.y, 0);
			boundsEditor->vertex[3] = float3(position.x + space + (itr->second->size.x * scaleBounds.x), position.y, 0);

			boundsGame->vertex[0] = float3(posX + space, posY + (fontSize * scaleBounds.y), 0);
			boundsGame->vertex[1] = float3(posX + space + (itr->second->size.x * scaleBounds.x), posY + (fontSize * scaleBounds.y), 0);
			boundsGame->vertex[2] = float3(posX + space, posY, 0);
			boundsGame->vertex[3] = float3(posX + space + (itr->second->size.x * scaleBounds.x), posY, 0);

			boundsEditor->RegenerateVBO();
			boundsGame->RegenerateVBO();
		}
	}

	boundsDrawn = nullptr;
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

		// Allocate a new buffer for RGBA data
		GLubyte* rgbaBuffer = new GLubyte[4 * face->glyph->bitmap.width * face->glyph->bitmap.rows];

		// Copy the glyph data into the RGBA buffer
		for (int y = 0; y < face->glyph->bitmap.rows; ++y)
		{
			for (int x = 0; x < face->glyph->bitmap.width; ++x)
			{
				GLubyte value = face->glyph->bitmap.buffer[y * face->glyph->bitmap.width + x];
				rgbaBuffer[4 * (y * face->glyph->bitmap.width + x)] = value;  // Red
				rgbaBuffer[4 * (y * face->glyph->bitmap.width + x) + 1] = value;  // Green
				rgbaBuffer[4 * (y * face->glyph->bitmap.width + x) + 2] = value;  // Blue
				rgbaBuffer[4 * (y * face->glyph->bitmap.width + x) + 3] = value;  // Alpha
			}
		}

		// Use the RGBA buffer for texture data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, rgbaBuffer);

		// Clean up the temporary buffer
		delete[] rgbaBuffer;

		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// now store character for later use
		Character* character = new Character
		{
			texture,
			float2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			float2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x),
		};

		mCharacters.insert(std::pair<GLchar, Character*>(c, character));
	}

	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

Font::~Font()
{
	for (unsigned char c = 0; c < 128; ++c)
	{
		RELEASE(mCharacters[c]);
	}

	mCharacters.clear();
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
	for (std::map<GLchar, Character*>::const_iterator it = mCharacters.begin(); it != mCharacters.end(); it++)
	{
		if ((*it).first == character)
		{
			GLuint id = (*it).second->textureID;
			return id;
		}
	}

	return 0;
}
