#include "Timer.h"

namespace TimeManager
{
	enum class PlayState {
		NONE,
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
	bool IsOnPlay();//Know if engine is in play mode
	void CleanUp();

	static PlayState state = PlayState::NONE;
	static float realTime = 0.0f;
	static float gameTime = 0.0f;
	static float timeScale = 0.0f;
	static Timer* gameTimer;
	static Timer* realTimer;
}