#ifndef TCOLOR__H
#define TCOLOR__H

class Color
{
public:

	Color();
	Color(float red, float green, float blue, float alpha = 1);

	Color& operator=(const Color& other);


public:

	static const Color Black;
	static const Color White;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Yellow;
	static const Color Magenta;
	static const Color Cyan;
	static const Color Transparent;

	float r;
	float g;
	float b;
	float a;
};

#endif // !TCOLOR__H
