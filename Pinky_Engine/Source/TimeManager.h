#ifndef __TIMEMANAGER_H__
#define __TIMEMANAGER_H__
#include "Timer.h"

namespace TimeManager
{
	enum class PlayState {
		EDITOR,//out of play mode
		PLAY,
		PAUSE,
		STEP
	};

	void StartEngineTime();
	void Update();
	void Play();
	void Pause();
	void Step();
	void Stop();
	void SetScale(float scale);
	float GetGameTime();
	float GetRealTime();
	bool IsOnGame();//Know if engine is in play mode
	bool IsOnStep();
	bool IsOnPause();
	bool IsOnPlay();
	void CleanUp();

	static PlayState state = PlayState::EDITOR;
	static float realTime = 0.0f;
	static float gameTime = 0.0f;
	static float timeScale = 1.0f;
	static Timer* gameTimer;
	static Timer* realTimer;
}
#endif //!__TIMEMANAGER_H__