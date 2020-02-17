#include "SceneMgr.h"

static Sprite sprite[] = {
	{D3DXVECTOR3(0,0,0.0f),D3DXVECTOR3(0,0,0),D3DXVECTOR2(1,1),TEXTURE_INDEX_HOST},
};
static Mesh mesh[] = {
	{D3DXVECTOR3(-7 * SIZE,-0.5*SIZE,0),D3DXVECTOR3(0,0,0),D3DXVECTOR3(1,1,1),MESH_SOLDIER01},
	//{D3DXVECTOR3(-7 * SIZE,-0.5*SIZE,0),D3DXVECTOR3(-90,0,0),D3DXVECTOR3(1,1,1),MESH_INDEX_TANK},
};

// 読み込みテクスチャ数
static const int SPRITE_COUNT = sizeof(sprite) / sizeof(sprite[0]);
static const int MESH_COUNT = sizeof(mesh) / sizeof(mesh[0]);


bool Lobby::Set()
{

	this->gfx->Set(sprite, 1, mesh, 0);
	this->gfx->camera.SetPosition(0, 0, -5.0f);
	this->gfx->camera.SetRotation(0, D3DXToRadian(-10.0f), 0);

	D3DXCreateFont(this->gfx->GetDevice(), 100, 0, FW_BOLD, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		"Arial", &pDXfont);

	this->NetType = this->network->GetType();
	if (this->NetType == HOSTROOM_NUM)
	{
		sprite[0].index = TEXTURE_INDEX_HOST;
	}
	else
	{
		sprite[0].index = TEXTURE_INDEX_JOINER;
	}
	return true;
}

int Lobby::Update()
{
	float dt = this->timer->GetMilisecondsElapsed();
	if (dt > 1000.0f)
	{
		this->timer->Restart();
		if (RdyToGo)
		{
			CountDown--;
		}
	}

	this->RdyToGo = this->network->IfConected();

	if (this->keyboard->KeyIsTrigger('R'))
	{
		return TITLE_NUM;

	}
	if (this->NetType == HOSTROOM_NUM)
	{
		if (CountDown < 0)
		{
			return GAME_NUM;
		}
		else
		{
			return HOSTROOM_NUM;
		}
	}
	else
	{
		//Input IP addr of Host

		while (!this->keyboard->CharBufferIsEmpty())
		{
			unsigned char ch = this->keyboard->ReadChar();
			if (isdigit(ch) || ch == '.')
			{
				this->tmpIP += ch;
			}
		}

		if (this->keyboard->KeyIsTrigger(VK_BACK))
		{
			this->tmpIP.pop_back();

		}
		else if (this->keyboard->KeyIsTrigger(VK_RETURN))
		{
			if (this->tmpIP.length() > 6)
			{
				this->network->LocalIP = this->tmpIP.c_str();
				this->network->Set();
			}
		}

		if (CountDown < 0)
		{
			return GAME_NUM;
		}
		else
		{
			return JOINTOTHER_NUM;
		}
	}
}

bool Lobby::Draw()
{
	this->gfx->RenderFrame();
	this->gfx->Draw();
	//this->gfx->DrawSprite();
	this->gfx->DrawGround();
	this->gfx->DrawSky();

	RECT textbox;
	SetRect(&textbox, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (this->RdyToGo)
	{
		if (CountDown > 0)
		{
			static char timeText[5];
			_itoa_s(CountDown, timeText, 10);
			pDXfont->DrawTextA(NULL,
				(LPCSTR)&timeText,
				strlen((LPCSTR)&timeText),
				&textbox,
				DT_CENTER | DT_VCENTER,
				D3DCOLOR_ARGB(255, 255, 255, 255));
		}
		else
		{
			pDXfont->DrawTextA(NULL,
				"GO!",
				4,
				&textbox,
				DT_CENTER | DT_VCENTER,
				D3DCOLOR_ARGB(255, 120, 120, 255));
		}

	}
	else
	{
		pDXfont->DrawTextA(NULL,
			"WAITING...",
			11,
			&textbox,
			DT_CENTER,
			D3DCOLOR_ARGB(177, 255, 255, 255));

		if (this->NetType == HOSTROOM_NUM)
		{
			pDXfont->DrawTextA(NULL,
				this->network->LocalIP,
				strlen(this->network->LocalIP),
				&textbox,
				DT_CENTER | DT_BOTTOM,
				D3DCOLOR_ARGB(255, 120, 120, 255));
		}
		else
		{
			pDXfont->DrawTextA(NULL,
				this->tmpIP.c_str(),
				this->tmpIP.length(),
				&textbox,
				DT_CENTER | DT_BOTTOM,
				D3DCOLOR_ARGB(255, 120, 120, 255));
		}

	}
	this->gfx->RenderFrame_end();
	return true;
}
