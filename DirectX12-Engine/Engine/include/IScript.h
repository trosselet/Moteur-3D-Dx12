#ifndef ISCRIPT__H
#define ISCRIPT__H

#include "GameObject.h"

namespace Engine
{
	class IScript
	{
	public:
		virtual void OnStart();
		virtual void OnUpdate();
		virtual void OnFixedUpdate();
		virtual void OnDisactivation();

	protected:
		GameObject* m_pOwner;

	private:
		void SetOwner(GameObject* pOwner) { m_pOwner = pOwner; };

		friend class GameManager;
		friend GameObject;
	};
}


#endif // !ISCRIPT__H
