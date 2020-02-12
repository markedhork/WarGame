#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	//this->DisableCursor();
	timer.Start();
	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
	{
		return false;
	}
	if (!gfx.Initialize(this->render_window.GetHWND(), width, height))
	{
		return false;
	}

	this->Set(TITLE_NUM);
	return true;
}

void Engine::Set(int index)
{
	switch (index)
	{
	case TITLE_NUM:			//Title
		pScene = new Title;
		break;
	case HOSTROOM_NUM:			//Lobby
		pScene = new Lobby;
		this->network = new Server;
		break;
	case JOINTOTHER_NUM:
		pScene = new Lobby;
		this->network = new Client;
		break;
	case GAME_NUM:
		pScene = new Game;
		break;
	case END_NUM:			//Lobby
		pScene = new Ending;
		break;
	default:
		break;
	}

	pScene->Initialize(&this->gfx, &this->keyboard, &this->mouse, &this->timer, this->network);
	pScene->Set();
	this->recScene = index;
}

bool Engine::ProcessMessages()
{
	return this->render_window.ProcessMessages();
}

void Engine::Update()
{
	this->nexScene = pScene->Update();
	if (this->nexScene != this->recScene)
	{
		this->Set(this->nexScene);
	}
}

void Engine::RenderFrame()
{
	this->pScene->Draw();
}