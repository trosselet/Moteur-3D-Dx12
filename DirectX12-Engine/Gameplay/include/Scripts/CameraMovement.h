#ifndef CAMERAMOVEMENT__H
#define CAMERAMOVEMENT__H

#include "IScript.h"

class CameraMovement : public Engine::IScript
{
public:
	CameraMovement();
	~CameraMovement();

	void OnStart() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;

private:
	int cameraSpeed;
};


#endif // !CAMERAMOVEMENT__H