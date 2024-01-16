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

struct Character 
{
	GLuint textureID;
	float2  size;       
	float2  bearing;    
	unsigned int advance;
};

struct Font
{
public:
	Font(std::string name, std::string fontPath);
	~Font();

	bool InitFont(std::string name, std::string fontPath);
	GLuint GetCharacterTexID(GLchar character);

public:
	//buffers
	float3 vertex[4];
	float2 uvs[4];

	uint* index;

	GLuint VBO; //vertex buffer object
	GLuint EBO; //element buffer object
	GLuint id_tex_uvs; //texture UVs

	GLuint textureID;

	//
	FT_Library ft;
	FT_Face face;

	std::map<char, Character*> mCharacters;
};

class UI_Text : public C_UI
{
public:
	// x = 0, y = 0, w = 200, y = 50
	UI_Text(GameObject* g, int x = 0, int y = 0, int w = 200, int h = 50);
	~UI_Text();

	void ShowInInspector();

	void Draw(bool game) override;

public:
	std::string text;
	Font* font;

	float fontSize;

	UIBounds* boundsEditor;
	UIBounds* boundsGame;
};
#endif // __UI_TEXT_H__