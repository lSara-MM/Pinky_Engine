#include "TimeManager.h"
#include "Application.h"

#include "ModuleScene.h"
#include "GameObject.h"

void TimeManager::StartEngineTime()
{
	gameTimer = new Timer();
	realTimer = new Timer();

	realTimer->Start();
}

void TimeManager::Update()
{
	realTime = realTimer->ReadSec();

	if (state == PlayState::PLAY || state == PlayState::STEP) 
	{
		gameTime = gameTimer->ReadSecScale(timeScale);
	}
}

void TimeManager::Play()
{
	switch (state)
	{
	case TimeManager::PlayState::EDITOR:
		App->parson->SaveScene("Scene_Temp", SCENES_TEMP_PATH);

		state = PlayState::PLAY;
		gameTime = 0.0f;
		gameTimer->Start();
		break;
	case TimeManager::PlayState::PLAY:
		App->resource->sceneFileName = SCENES_TEMP_PATH "Scene_Temp" SCENE_EXT;
		state = PlayState::EDITOR;
		gameTime = 0.0F;

		switch (App->scene->mainScreen)
		{
		case 0:
			App->resource->pendingToLoadScene = true;
			break;
		case 1:
			App->scene->ImportDefaultMainScreen();
			break;
		case 2:
			App->scene->ImportDefaultScene();
			break;
		default:
			break;
		}
		break;
	case TimeManager::PlayState::PAUSE:
		state = PlayState::PLAY;
		gameTimer->Step();
		break;
	case TimeManager::PlayState::STEP:
		break;
	default:
		break;
	}
}

void TimeManager::Pause()
{
	switch (state)
	{
	case TimeManager::PlayState::EDITOR:
		break;
	case TimeManager::PlayState::PLAY:
		state = PlayState::PAUSE;
		gameTimer->Stop();
		break;
	case TimeManager::PlayState::PAUSE:
		Play();
		break;
	case TimeManager::PlayState::STEP:
		state = PlayState::PAUSE;
		gameTimer->Stop();
		break;
	default:
		break;
	}
}

void TimeManager::Step()
{
	switch (state)
	{
	case TimeManager::PlayState::EDITOR:
		break;
	case TimeManager::PlayState::PLAY:
		state = PlayState::STEP;
		break;
	case TimeManager::PlayState::PAUSE:
		gameTimer->Step();
		state = PlayState::STEP;
		break;
	case TimeManager::PlayState::STEP:
		break;
	default:
		break;
	}
}

void TimeManager::Stop()
{
	gameTime = 0.0f;
	state = PlayState::EDITOR;
}

void TimeManager::SetScale(float scale)
{
	timeScale = scale;
}

float TimeManager::GetGameTime()
{
	return gameTime;
}

float TimeManager::GetRealTime()
{
	return realTime;
}

bool TimeManager::IsOnGame()
{
	return state != PlayState::EDITOR;
}

bool TimeManager::IsOnStep()
{
	return state == PlayState::STEP;
}

bool TimeManager::IsOnPause()
{
	return state == PlayState::PAUSE;
}

bool TimeManager::IsOnPlay()
{
	return state == PlayState::PLAY;
}


void TimeManager::CleanUp()
{
	RELEASE(realTimer);
	RELEASE(gameTimer);
}
