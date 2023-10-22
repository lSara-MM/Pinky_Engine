
#pragma once

struct Color
{
	float r, g, b, a;
	
	Color() : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
	{}

	Color(float red, float green, float blue, float alpha = 1.0f)
	{
		if (red > 1.0f)
		{
			r = red / 255;
			g = green / 255;
			b = blue / 255;
			a = alpha / 255;
		}
		else
		{
			r = red;
			g = green;
			b = blue;
			a = alpha;
		}
	}

	void Set(float r, float g, float b, float a = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	float* operator & ()
	{
		return (float*)this;
	}
};

extern Color Red;
extern Color Green;
extern Color Blue;
extern Color Black;
extern Color White;