#ifndef TRANSFORMABLE__H
#define TRANSFORMABLE__H

#include "Define.h"
#include "TVector.hpp"
#include "TTransform.h"

#include <DirectXMath.h>

namespace Core
{
	class Transformable
	{
	public:
		static Transformable const Identity;

		Transformable();
		~Transformable();

		void SetPosition(Vector3f pos);
		void SetRotationX(float32 angle);
		void SetRotationY(float32 angle);
		void SetRotationZ(float32 angle);
		void SetRotationYPR(Vector3f rot);
		void SetScale(Vector3f scale);

		void Translate(Vector3f offset);
		void RotateX(float32 angle);
		void RotateY(float32 angle);
		void RotateZ(float32 angle);
		void RotateYPR(Vector3f rot);
		void Scale(Vector3f offset);

		Vector3f GetPosition();
		Vector3f GetScale();

		Vector3f Forward();
		Vector3f Right();
		Vector3f Up();

	protected:
		TRANSFORM m_transform;
	};
}

#endif // !TRANSFORMABLE__H
