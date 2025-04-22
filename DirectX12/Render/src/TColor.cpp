#include "pch.h"
#include "TColor.h"

const Render::Color Render::Color::Black(0, 0, 0);
const Render::Color Render::Color::White(1, 1, 1);
const Render::Color Render::Color::Red(1, 0, 0);
const Render::Color Render::Color::Green(0, 1, 0);
const Render::Color Render::Color::Blue(0, 0, 1);
const Render::Color Render::Color::Yellow(1, 1, 0);
const Render::Color Render::Color::Magenta(1, 0, 1);
const Render::Color Render::Color::Cyan(0, 1, 1);
const Render::Color Render::Color::Transparent(0, 0, 0, 0);

Render::Color::Color() : r(0), g(0), b(0), a(1)
{
}

Render::Color::Color(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha)
{
}

Render::Color::Color(DirectX::XMFLOAT4& color)
{
	r = static_cast<float>(color.x);
	g = static_cast<float>(color.y);
	b = static_cast<float>(color.z);
	a = static_cast<float>(color.w);
}

Render::Color& Render::Color::operator=(const Color& other)
{
	if (this != &other)
	{
		r = other.r;
		g = other.g;
		b = other.b;
		a = other.a;
	}
	return *this;
}