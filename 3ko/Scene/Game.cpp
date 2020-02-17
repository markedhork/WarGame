#include "SceneMgr.h"

// position.xyz , rotation.xyz , size.xyz , TextureIndex number
static Sprite Game_sprite[] = {
	{D3DXVECTOR3(0,0,1.0f),D3DXVECTOR3(0,0,0),D3DXVECTOR2(1,1),TEXTURE_INDEX_DANGER},
};
static Mesh Game_mesh[] = {
	{D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),D3DXVECTOR3(1,1,1),MESH_SOLDIER01},
	{D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),D3DXVECTOR3(1,1,1),MESH_SOLDIER02},
};

// 読み込みテクスチャ数
static const int SPRITE_COUNT_TT = sizeof(Game_sprite) / sizeof(Game_sprite[0]);
static const int MESH_COUNT_TT = sizeof(Game_mesh) / sizeof(Game_mesh[0]);

bool Game::Set()
{
	int situation = this->network->GetType();
	switch (situation)
	{
	case HOSTROOM_NUM:
		player.pos = D3DXVECTOR3(-7 * SIZE, -0.5*SIZE, 0);
		player.rot = D3DXVECTOR3(90, 0, 0);
		player.ID = 0;

		enemy.pos = D3DXVECTOR3(7 * SIZE, -0.5*SIZE, 0);
		enemy.rot = D3DXVECTOR3(-90, 0, 0);
		enemy.ID = 1;

		break;
	case JOINTOTHER_NUM:
		player.pos = D3DXVECTOR3(7 * SIZE, -0.5*SIZE, 0);
		player.rot = D3DXVECTOR3(-90, 0, 0);
		player.ID = 1;

		enemy.pos = D3DXVECTOR3(-7 * SIZE, -0.5*SIZE, 0);
		enemy.rot = D3DXVECTOR3(90, 0, 0);
		enemy.ID = 0;

		break;
	default:
		break;
	}
	enemy.hp = 100;
	player.hp = 100;
	this->player.moveSpd = 0.002f;
	Game_mesh[0].pos = D3DXVECTOR3(-7 * SIZE, -0.5*SIZE, 0);
	Game_mesh[0].rot = D3DXVECTOR3(90, 0, 0);

	Game_mesh[1].pos = D3DXVECTOR3(7 * SIZE, -0.5*SIZE, 0);
	Game_mesh[1].rot = D3DXVECTOR3(-90, 0, 0);
	this->gfx->Set(Game_sprite, 0, Game_mesh, MESH_COUNT_TT);

	this->AdjustCamera();

	D3DXCreateFont(this->gfx->GetDevice(), 100, 30, FW_BOLD, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		"Arial", &pDXfont);

	this->bullet.SetDevice(this->gfx->GetDevice());
	this->bullet.CreateMeshBuffer();

	player.anim = IDLE;
	enemy.anim = IDLE;
	return true;
}

int Game::Update()
{
	this->bullet.Update();
	if (this->network->IfNewMsg())
	{
		msg = this->network->GetMsg();
		enemy.pos = msg.pos;
		enemy.rot = msg.rot;
		enemy.vec_front = msg.dir;
		enemy.anim = msg.anim;
		enemy.hp = msg.hp;
	}


	if (this->player.ID == 0)
	{
		Game_mesh[1].pos = enemy.pos;
		Game_mesh[1].rot = enemy.rot;
		this->gfx->SwitchTrack(1, enemy.anim);
	}
	else
	{
		Game_mesh[0].pos = enemy.pos;
		Game_mesh[0].rot = enemy.rot;
		this->gfx->SwitchTrack(0, enemy.anim);
	}
	if (enemy.anim == AIM)
	{
		this->bullet.GenerateEnemy(enemy.pos, enemy.vec_front);
	}
	while (!this->mouse->EventBufferIsEmpty())
	{
		MouseEvent me = this->mouse->ReadEvent();

		//adjust player rotation
		if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
		{
			Game_mesh[player.ID].rot.x += (float)me.GetPosX()*0.1f;
			//this->gfx->camera.AdjustRotation((float)me.GetPosX()*0.01f, (float)me.GetPosY()*0.01f, 0.0f);
		}
	}
	if (this->mouse->IsLeftDown())
	{
		this->bullet.Generate(this->player.pos, this->player.vec_front);
		this->gfx->SwitchTrack(this->player.ID, AIM);
		player.anim = AIM;
	}
	else
	{
		this->gfx->SwitchTrack(this->player.ID, RELOAD);
		player.anim = RELOAD;
	}
	static DWORD ctime = GetTickCount();
	if (!this->keyboard->KeyBufferIsEmpty())
	{
		if (player.anim != AIM)
		{
			player.anim = IDLE;
		}
		if (this->keyboard->KeyIsPressed('W') && this->CheckWall(1))
		{
			Game_mesh[this->player.ID].pos += this->player.vec_front*this->player.moveSpd*(GetTickCount() - ctime)* 0.01f;

			player.anim = MOVE;

		}
		if (this->keyboard->KeyIsPressed('A') && this->CheckWall(3))
		{
			Game_mesh[this->player.ID].pos += this->player.vec_left*this->player.moveSpd*(GetTickCount() - ctime)* 0.01f;

			player.anim = MOVE;
		}
		if (this->keyboard->KeyIsPressed('S') && this->CheckWall(2))
		{
			Game_mesh[this->player.ID].pos += this->player.vec_back*this->player.moveSpd*(GetTickCount() - ctime)* 0.01f;

			player.anim = MOVE;
		}
		if (this->keyboard->KeyIsPressed('D') && this->CheckWall(4))
		{
			Game_mesh[this->player.ID].pos += this->player.vec_right*this->player.moveSpd*(GetTickCount() - ctime)* 0.01f;

			player.anim = MOVE;
		}
		if (player.anim != AIM)
		{
			this->gfx->SwitchTrack(this->player.ID, player.anim);
		}
		this->AdjustCamera();
	}
	ctime = GetTickCount();
	player.pos = Game_mesh[this->player.ID].pos;
	player.rot = Game_mesh[this->player.ID].rot;

	this->player.hp -= this->bullet.UpdateEnemy(this->player.pos);

	msg.pos = player.pos;
	msg.rot = player.rot;
	msg.dir = player.vec_front;
	msg.anim = player.anim;
	msg.hp = player.hp;
	this->network->Send(&msg);



	if (enemy.hp > 0 && player.hp > 0)
	{
		return GAME_NUM;
	}
	else
	{
		this->gfx->playerHP = player.hp;
		this->gfx->enemyHP = enemy.hp;
		return END_NUM;
	}
}

bool Game::Draw()
{
	this->gfx->RenderFrame();
	this->gfx->Draw();
	//this->gfx->DrawSprite();
	this->gfx->DrawGround();
	this->gfx->DrawSky();
	this->bullet.Draw();

	RECT textbox;
	SetRect(&textbox, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	static char hpText[5];
	_itoa_s(player.hp, hpText, 10);
	pDXfont->DrawTextA(NULL,
		(LPCSTR)&hpText,
		strlen((LPCSTR)&hpText),
		&textbox,
		DT_CENTER,
		D3DCOLOR_ARGB(255, 255, 0, 0));


	this->gfx->RenderFrame_end();
	return true;
}

void Game::AdjustCamera()
{
	if (Game_mesh[this->player.ID].rot.x > 360.0f)
	{
		Game_mesh[this->player.ID].rot.x -= 360.0f;
	}
	else if (Game_mesh[this->player.ID].rot.x < -360.0f)
	{
		Game_mesh[this->player.ID].rot.x += 360.0f;
	}
	D3DXMATRIX vecRotationMatrix;
	D3DXMatrixRotationYawPitchRoll(&vecRotationMatrix, D3DXToRadian(Game_mesh[this->player.ID].rot.x),
		D3DXToRadian(Game_mesh[this->player.ID].rot.y), D3DXToRadian(Game_mesh[this->player.ID].rot.z));
	D3DXVec3TransformCoord(&this->player.vec_front, &this->DEFAULT_FORWARD_VECTOR, &vecRotationMatrix);
	D3DXVec3TransformCoord(&this->player.vec_back, &this->DEFAULT_BACKWARD_VECTOR, &vecRotationMatrix);
	D3DXVec3TransformCoord(&this->player.vec_left, &this->DEFAULT_LEFT_VECTOR, &vecRotationMatrix);
	D3DXVec3TransformCoord(&this->player.vec_right, &this->DEFAULT_RIGHT_VECTOR, &vecRotationMatrix);

	D3DXVECTOR3 offset = D3DXVECTOR3(0.0f, 1.5f, 0.0f);
	this->gfx->camera.SetPosition(offset + Game_mesh[this->player.ID].pos);
	this->gfx->camera.AdjustPosition(this->player.vec_back*2.0f);
	this->gfx->camera.SetLookAtPos(Game_mesh[this->player.ID].pos);

	float degree = D3DXToRadian(-15.0f);
	this->gfx->camera.AdjustRotation(0, degree, 0);
}

bool Game::CheckWall(int dir)
{
	D3DXVECTOR3 temp = this->player.pos;
	D3DXVECTOR3 tempL = this->player.pos + this->player.vec_left*(0.2f);
	D3DXVECTOR3 tempR = this->player.pos + this->player.vec_right*(0.2f);

	switch (dir)
	{
	case 1://forward
		temp += this->player.vec_front*this->player.moveSpd;
		tempL += this->player.vec_front*this->player.moveSpd;
		tempR += this->player.vec_front*this->player.moveSpd;
		break;
	case 2://backward
		temp += this->player.vec_back*this->player.moveSpd;
		tempL += this->player.vec_back*this->player.moveSpd;
		tempR += this->player.vec_back*this->player.moveSpd;
		break;
	case 3://left
		temp += this->player.vec_left*this->player.moveSpd;
		tempL += this->player.vec_left*this->player.moveSpd;
		tempR += this->player.vec_left*this->player.moveSpd;
		break;
	case 4://right
		temp += this->player.vec_right*this->player.moveSpd;
		tempL += this->player.vec_right*this->player.moveSpd;
		tempR += this->player.vec_right*this->player.moveSpd;
		break;
	default:
		break;
	}

	D3DXVECTOR2 p0 = D3DXVECTOR2(-7.5f*SIZE, 7.5f*SIZE);
	D3DXVECTOR2 p1 = D3DXVECTOR2(7.5f*SIZE, -7.5f*SIZE);

	//boarder
	if (temp.x<p0.x || temp.x>p1.x || temp.z > p0.y || temp.z < p1.y)
	{
		return false;
	}
	if (tempL.x<p0.x || tempL.x>p1.x || tempL.z > p0.y || tempL.z < p1.y)
	{
		return false;
	}
	if (tempR.x<p0.x || tempR.x>p1.x || tempR.z > p0.y || tempR.z < p1.y)
	{
		return false;
	}
	float checkX, checkY;
	checkX = temp.x - p0.x;
	checkY = p0.y - temp.z;

	checkX = (int)(checkX / SIZE);
	checkY = (int)(checkY / SIZE);

	int bx, by;
	bx = checkX / MAP_X;
	by = checkY / MAP_Y;

	int mx, my;
	mx = (int)checkX % MAP_X;
	my = (int)checkY % MAP_Y;

	if (map[Build[by][bx] - 1][my][mx] == 2)
	{
		return false;
	}


	checkX = tempL.x - p0.x;
	checkY = p0.y - tempL.z;

	checkX = (int)(checkX / SIZE);
	checkY = (int)(checkY / SIZE);

	bx = checkX / MAP_X;
	by = checkY / MAP_Y;

	mx = (int)checkX % MAP_X;
	my = (int)checkY % MAP_Y;

	if (map[Build[by][bx] - 1][my][mx] == 2)
	{
		return false;
	}

	checkX = tempR.x - p0.x;
	checkY = p0.y - tempR.z;

	checkX = (int)(checkX / SIZE);
	checkY = (int)(checkY / SIZE);

	bx = checkX / MAP_X;
	by = checkY / MAP_Y;

	mx = (int)checkX % MAP_X;
	my = (int)checkY % MAP_Y;

	if (map[Build[by][bx] - 1][my][mx] == 2)
	{
		return false;
	}

	this->player.moveSpd = 0.1f;
	return true;
}
