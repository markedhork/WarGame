#pragma once
#include "../Mouse/MouseClass.h"
#include "../Keyboard/KeyboardClass.h"
#include "../Timer.h"
#include "../Graphics/Graphics.h"
#include "../Network/Network.h"
#include "../Bullet.h"

#define PLAYER_ROTATE_SPD		(D3DXToRadian(30.0f))


class SceneMgr
{
public:
	bool Initialize(Graphics* pGfx, KeyboardClass* pKeyboard,
		MouseClass* pMouse, Timer *pTimer, Network *pNetwork)
	{
		this->gfx = pGfx;
		this->keyboard = pKeyboard;
		this->mouse = pMouse;
		this->timer = pTimer;
		this->network = pNetwork;
		return true;
	}
	virtual bool Set() = 0;
	virtual int Update() = 0;
	virtual bool Draw() = 0;

	struct PLAYER_INFO
	{
		int ID;
		int team;
		D3DXVECTOR3 pos;
		D3DXVECTOR3 rot;
		int hp;
		float moveSpd = 0.1f;
		D3DXVECTOR3			vec_back;
		D3DXVECTOR3			vec_front;
		D3DXVECTOR3			vec_left;
		D3DXVECTOR3			vec_right;
	};
	PLAYER_INFO player;
	PLAYER_INFO enemy;

protected:
	Graphics* gfx;
	KeyboardClass* keyboard;
	MouseClass* mouse;
	Timer *timer;
	Network *network;
};


class Title :
	public SceneMgr
{
public:
	bool Set();
	int Update();
	bool Draw();
private:
	LPD3DXFONT			pDXfont;
	int ck = 0;
};

class Lobby :
	public SceneMgr
{
public:
	bool Set();
	int Update();
	bool Draw();
private:
	LPD3DXFONT			pDXfont;
	bool				RdyToGo = false;
	int					CountDown = 10;
	int					NetType;
	std::string			tmpIP = "";
};

class Game :
	public SceneMgr
{
public:
	bool Set();
	int Update();
	bool Draw();

private:
	LPD3DXFONT			pDXfont;    // the pointer to the font object

	Bullet bullet;

	MY_MSG msg;
	MY_MSG msg2;

	void AdjustCamera();
	bool CheckWall(int dir);

	const D3DXVECTOR3 DEFAULT_FORWARD_VECTOR = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	const D3DXVECTOR3 DEFAULT_BACKWARD_VECTOR = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	const D3DXVECTOR3 DEFAULT_LEFT_VECTOR = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	const D3DXVECTOR3 DEFAULT_RIGHT_VECTOR = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
};


class Ending :
	public SceneMgr
{
public:
	bool Set();
	int Update();
	bool Draw();
private:
	LPD3DXFONT			pDXfont;
};

