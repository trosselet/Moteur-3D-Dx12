#ifndef GEOMETRY__H
#define GEOMETRY__H

#include <DirectXMath.h>
#include <vector>
#include "../../Core/include/Define.h"

struct Geometry
{
	std::vector<uint32> indexes = {};

	uint32 indiciesNumber;

	std::vector<DirectX::XMFLOAT3> positions{};
	std::vector<DirectX::XMFLOAT4> colors{};
	std::vector<DirectX::XMFLOAT2> UVs{};
	std::vector<DirectX::XMFLOAT3> normals{};
};

#endif // !GEOMETRY__H
