#pragma once

#include "../../lyo-t2-amiga-p7-07/src/Application/Scene.h"
#include "GraphicEngine.h"
#include "ECSManager.h"

#define FIXED_DT 0.01667f

namespace Engine
{
	class AppWindow
	{
	public:
		AppWindow() = default;
		virtual ~AppWindow();

		bool Initialize(int width, int height);
		void OnCreate(HWND hwnd);
		void OnDestroy();
		void Update();
		void FixedUpdate();
		bool IsRunning();
		bool UpdateTime();
		float GetDeltaTime() const { return mDeltaTime; }
		void SetDeltaTime(float deltaTime) { mDeltaTime = deltaTime; }


	private:
		GraphicEngine* mpGraphicEngine;
		ECSManager* mECSManager;
		Scene scene;
		

	private:
		HWND mHwnd;
		bool mIsRunning;

		int mWidth, mHeight;

		float m_elapsedTime, m_time;
		float mAccumulatedDt;
		float mDeltaTime;

		int test = 0;
	};

}
