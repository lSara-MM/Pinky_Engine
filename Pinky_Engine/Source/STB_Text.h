#ifndef __STB_TEXT_H__
#define __STB_TEXT_H__
#pragma once

#include "Globals.h"
#include <string>

#include "External Libraries/Glew/include/glew.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "External Libraries/ImGui/imstb_truetype.h"

class STB_Text
{
public:

	STB_Text();
	~STB_Text();

	void Init();
	void Print(float x, float y, const char* text);
	void Draw(float x, float y, std::string& text);

public:
	std::string fontPath;

	GLuint tex;

	unsigned char ttf_buffer[1 << 20];
	unsigned char temp_bitmap[1024 * 1024];
	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
};
#endif // __STB_TEXT_H__