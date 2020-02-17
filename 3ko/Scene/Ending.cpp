#include "SceneMgr.h"

bool Ending::Set()
{
	D3DXCreateFont(this->gfx->GetDevice(), 200, 100, FW_BOLD, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		"Arial", &pDXfont);

	return true;
}

int Ending::Update()
{
	if (this->keyboard->KeyIsTrigger('R'))
	{
		return TITLE_NUM;
	}
	return END_NUM;
}

bool Ending::Draw()
{
	this->gfx->RenderFrame();

	this->gfx->DrawGround();
	this->gfx->DrawSky();

	RECT textbox;
	SetRect(&textbox, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (this->gfx->playerHP <= 0)
	{
		if (this->gfx->enemyHP <= 0)
		{
			//Draw
			pDXfont->DrawTextA(NULL,
				"DRAW",
				5,
				&textbox,
				DT_CENTER | DT_VCENTER,
				D3DCOLOR_ARGB(255, 255, 255, 255));
		}
		else
		{
			//Lose
			pDXfont->DrawTextA(NULL,
				"YOU LOSE",
				9,
				&textbox,
				DT_CENTER | DT_VCENTER,
				D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
	else
	{
		//Win
		pDXfont->DrawTextA(NULL,
			"YOU WIN",
			8,
			&textbox,
			DT_CENTER | DT_VCENTER,
			D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	this->gfx->RenderFrame_end();
	return true;
}
