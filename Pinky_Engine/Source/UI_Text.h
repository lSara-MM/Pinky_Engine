#ifndef __UI_TEXT_H__
#define __UI_TEXT_H__
#pragma once

#include "Globals.h"
#include "C_UI.h"
#include "C_Material.h"
#include "External Libraries/FreeType/include/ft2build.h"
#include "External Libraries/FreeType/include/freetype/freetype.h"
#include "External Libraries/FreeType/include/freetype/ftglyph.h"
#pragma comment(lib, "Source/External Libraries/FreeType/libx86/freetype.lib")

struct Character {
	GLuint TextureID;
	float2  Size;       
	float2  Bearing;    
	unsigned int Advance; 
};

struct Font
{
	Font(std::string name, std::string fontPath, int size = 5);
	FT_Library ft;
	FT_Face face;
	float fontSize;
	std::map<GLchar, Character*> Characters;
	bool InitFont(std::string name, std::string fontPath);
	GLuint GetCharacterTexID(GLchar character);

	//buffers
	float3 vertex[4];
	float2 uvs[4];

	uint* index;

	GLuint VBO; //vertex buffer object
	GLuint EBO; //element buffer object
	GLuint id_tex_uvs; //texture UVs

	GLuint textureID;
};

class UI_Text : public C_UI
{
public:

	UI_Text(GameObject* g, int w, int h, int x, int y);
	~UI_Text();

	void ShowInInspector();

	void Draw(bool game) override;

public:
	std::string text;
	Font* font;

};
#endif // __UI_TEXT_H__