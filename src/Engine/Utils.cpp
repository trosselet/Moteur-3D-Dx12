#include "pch.h"
#include "Utils.h"

Utils* Utils::Get()
{
	static Utils mInstance;
	return &mInstance;
}

float Utils::Lerp(float start, float end, float time)
{
	return start + time * (end - start);
}