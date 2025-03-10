#ifndef UTILS_H
#define UTILS_H

#include "pch.h"


class Utils
{
public:
	Utils() = default;
	~Utils() = default;

	static Utils* Get();

	float Lerp(float start, float end, float time);
};

#endif // !UTILS_H