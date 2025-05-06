#ifndef IDRAWABLE__H
#define IDRAWABLE__H

namespace Core
{
	class IDrawable
	{
	public:

		virtual ~IDrawable() = default;
	protected:
		virtual void Draw(class ID3D12Device* pDevice, class ID3D12GraphicsCommandList* commandList) = 0;

	};
}


#endif // !IDRAWABLE__H