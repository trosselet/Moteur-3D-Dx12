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
		};

		Type type = Type::None;
	};
}

#endif // !TEVENT__H
