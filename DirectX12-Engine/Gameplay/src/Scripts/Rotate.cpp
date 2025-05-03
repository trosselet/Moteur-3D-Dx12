#include "Scripts/Rotate.h"

#include <iostream>

Rotate::Rotate()
{
}

Rotate::~Rotate()
{
}

void Rotate::OnStart()
{
	std::cout << "OnStart gg !! \n";
}

void Rotate::OnUpdate()
{
	m_pOwner->m_pTransform->RotateYPR({ 0.1f, 0.1f, 0.1f });
}

void Rotate::OnFixedUpdate()
{
	std::cout << "OnFixedUpdate gg !! \n";
}
