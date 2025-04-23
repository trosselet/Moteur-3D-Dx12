#ifndef IDRAWABLE__H
#define IDRAWABLE__H

namespace Core
{
	class IDrawable
	{
	public:
		virtual void Draw(class ID3D12Device* pDevice, class ID3D12GraphicsCommandList* commandList) = 0;
		virtual ~IDrawable() = default;
	};
}


#endif // !IDRAWABLE__H