#ifndef ROTATE__H
#define ROTATE__H

#include "IScript.h"

class Rotate : public Engine::IScript
{
public:
	Rotate();
	~Rotate();

	void OnStart() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;
};


#endif // !ROTATE__H