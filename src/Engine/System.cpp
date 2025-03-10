#include "pch.h"
#include "System.h"
#include "Component.h"

System::System()
{
}

System::~System()
{
}

void System::OnUpdate(std::vector<Component*>)
{
}

std::vector<Component*> System::HasWantedComponent(std::vector<Component*> allEntityComponent)
{
    std::vector<Component*> correctComponent;

    for (int i = 0; i < mNeededComponent.size(); i++)
    {
        bool found = false;
        for (int j = 0; j < allEntityComponent.size(); j++)
        {

            if (allEntityComponent[j] == nullptr)
            {
                continue;
            }


            if (mNeededComponent[i] == allEntityComponent[j]->GetID())
            {
                found = true;
                correctComponent.push_back(allEntityComponent[j]);
                break;
            }
        }

        if (!found)
        {
            return {};
        }
    }

    return correctComponent; 
}



void System::Update(std::vector<Component*> mlistPComponent)
{
	OnUpdate(mlistPComponent);
}
