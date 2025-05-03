#ifndef ROTATE__H
#define ROTATE__H

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


#endif // !ROTATE__H