#pragma once

#include "pch.h"
#include "ECSManager.h"

class System
{
public:
	System();
	virtual ~System();

	virtual void OnUpdate(std::vector<Component*>);
	void SetWantedComponent(std::vector<int> _mNeededComponent) { mNeededComponent = _mNeededComponent; }
	const std::vector<int>& GetWantedComponent() const { return mNeededComponent; }
	std::vector<Component*> HasWantedComponent(std::vector<Component*> allEntityComponent);

	int GetId() { return mId; }

protected:

	int mId;
	std::vector<int> mNeededComponent;

private:

	void Update(std::vector<Component*>);
	friend class SystemManager;
};


