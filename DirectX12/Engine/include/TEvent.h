#ifndef TEVENT__H
#define TEVENT__H

namespace Engine
{
	struct Event
	{
		enum class Type
		{
			None = 0,
			Closed,
			Resized,
		};

		Type type = Type::None;
	};
}

#endif // !TEVENT__H
