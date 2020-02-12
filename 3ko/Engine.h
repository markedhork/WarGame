#pragma once
#include "Network/Network.h"
#include "WindowContainer.h"
#include "Scene/SceneMgr.h"


class Engine :WindowContainer
{
public:
	bool Initialize(HINSTANCE hInstance, std::string window_title, 
		std::string window_class, int width, int height);
	void Set(int index);
	bool ProcessMessages();
	void Update();
	void RenderFrame();

private:
	Timer			timer;
	SceneMgr*		pScene;
	Network*		network;


	int				recScene;
	int				nexScene;
};

