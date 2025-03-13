#include "pch.h"
#include "main.h"


int main()
{
    ECS ecs;

    Entity player = ecs.CreateEntity();
    ecs.AddComponent(player, TransformComponent{ 0.f, 0.f });

    std::cout << "Hello, World!\n";
    return 0;
}