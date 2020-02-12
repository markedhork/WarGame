#include "SceneMgr.h"
#define PLAYER_STATE Game_mesh[0]
#define ENEMY_STATE Game_mesh[1]
// position.xyz , rotation.xyz , size.xyz , TextureIndex number
static Sprite Game_sprite[] = {
	{D3DXVECTOR3(0,0,1.0f),D3DXVECTOR3(0,0,0),D3DXVECTOR2(1,1),TEXTURE_INDEX_DANGER},
};
static Mesh Game_mesh[] = {
	{D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),D3DXVECTOR3(1,1,1),MESH_SOLDIER01},
	//{D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,0),D3DXVECTOR3(1,1,1),MESH_TANK_BODY},
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
		player.rot = D3DXVECTOR3(-90, 0, 0);


		enemy.pos = D3DXVECTOR3(7 * SIZE, -0.5*SIZE, 0);
		enemy.rot = D3DXVECTOR3(90, 0, 0);

		break;
	case JOINTOTHER_NUM:
		player.pos = D3DXVECTOR3(7 * SIZE, -0.5*SIZE, 0);
		player.rot = D3DXVECTOR3(90, 0, 0);

		enemy.pos = D3DXVECTOR3(-7 * SIZE, -0.5*SIZE, 0);
		enemy.rot = D3DXVECTOR3(-90, 0, 0);
		break;
	default:
		break;
	}
	enemy.hp = 100;
	player.hp = 100;
	PLAYER_STATE.pos = this->player.pos;
	PLAYER_STATE.rot = this->player.rot;

	ENEMY_STATE.pos = this->enemy.pos;
	ENEMY_STATE.rot = this->enemy.rot;
	this->gfx->Set(Game_sprite, 0, Game_mesh, MESH_COUNT_TT);

	this->AdjustCamera();

	D3DXCreateFont(this->gfx->GetDevice(), 100, 30, FW_BOLD, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		"Arial", &pDXfont);

	this->bullet.SetDevice(this->gfx->GetDevice());
	this->bullet.CreateMeshBuffer();



	return true;
}

int Game::Update()
{
	this->bullet.Update();
	msg.pos = player.pos;
	msg.rot = player.rot;
	this->network->Send(&msg);

	if (this->network->IfNewMsg())
	{
		msg = this->network->GetMsg();
		enemy.pos = msg.pos;
		enemy.rot = msg.rot;
	}
	ENEMY_STATE.pos = enemy.pos;
	ENEMY_STATE.rot = enemy.rot;

	while (!this->mouse->EventBufferIsEmpty())
	{
		MouseEvent me = this->mouse->ReadEvent();
		D3DXVECTOR3 offset = D3DXVECTOR3(0.0f, 15.0f, 0.0f);
		this->gfx->camera.SetPosition(PLAYER_STATE.pos + offset);
		if (this->mouse->IsRightDown())
		{
			this->keyboard->ReadKey();
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				this->gfx->camera.AdjustRotation((float)me.GetPosX()*0.01f, (float)me.GetPosY()*0.01f, 0.0f);
			}
		}
	}


	if (!this->keyboard->KeyBufferIsEmpty())
	{

		if (this->keyboard->KeyIsPressed('W') && this->CheckWall(1))
		{
			PLAYER_STATE.pos += this->player.vec_front*this->player.moveSpd;
		}
		if (this->keyboard->KeyIsPressed('A'))
		{
			PLAYER_STATE.rot.x -= PLAYER_ROTATE_SPD;
		}
		if (this->keyboard->KeyIsPressed('S') && this->CheckWall(2))
		{
			PLAYER_STATE.pos += this->player.vec_back*this->player.moveSpd;
		}
		if (this->keyboard->KeyIsPressed('D'))
		{
			PLAYER_STATE.rot.x += PLAYER_ROTATE_SPD;
		}
		if (this->keyboard->KeyIsTrigger(VK_SPACE))
		{
			this->bullet.Generate(this->player.pos, this->player.vec_front);
		}


		this->AdjustCamera();
	}
	player.pos = PLAYER_STATE.pos;
	player.rot = PLAYER_STATE.rot;
	return GAME_NUM;
}

bool Game::Draw()
{
	this->gfx->RenderFrame();
	this->gfx->Draw();
	//this->gfx->DrawSprite();
	this->gfx->DrawGround();
	this->gfx->DrawSky();
	this->bullet.Draw();

	this->gfx->RenderFrame_end();
	return true;
}

void Game::AdjustCamera()
{
	if (PLAYER_STATE.rot.x > 360.0f)
	{
		PLAYER_STATE.rot.x -= 360.0f;
	}
	else if (PLAYER_STATE.rot.x < -360.0f)
	{
		PLAYER_STATE.rot.x += 360.0f;
	}
	D3DXMATRIX vecRotationMatrix;
	D3DXMatrixRotationYawPitchRoll(&vecRotationMatrix, D3DXToRadian(PLAYER_STATE.rot.x),
		D3DXToRadian(PLAYER_STATE.rot.y), D3DXToRadian(PLAYER_STATE.rot.z));
	D3DXVec3TransformCoord(&this->player.vec_front, &this->DEFAULT_FORWARD_VECTOR, &vecRotationMatrix);
	D3DXVec3TransformCoord(&this->player.vec_back, &this->DEFAULT_BACKWARD_VECTOR, &vecRotationMatrix);
	D3DXVec3TransformCoord(&this->player.vec_left, &this->DEFAULT_LEFT_VECTOR, &vecRotationMatrix);
	D3DXVec3TransformCoord(&this->player.vec_right, &this->DEFAULT_RIGHT_VECTOR, &vecRotationMatrix);

	D3DXVECTOR3 offset = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
	this->gfx->camera.SetPosition(offset + PLAYER_STATE.pos);
	this->gfx->camera.AdjustPosition(this->player.vec_back*10.0f);
	this->gfx->camera.SetLookAtPos(PLAYER_STATE.pos);

	float degree = D3DXToRadian(-30.0f);
	this->gfx->camera.AdjustRotation(0, degree, 0);
	this->gfx->camera.AdjustPosition(0, -4.0f, 0);
	this->gfx->camera.AdjustPosition(this->gfx->camera.GetBackwardVector()*2.0f);
}

bool Game::CheckWall(int dir)
{
	D3DXVECTOR3 temp = this->player.pos;
	D3DXVECTOR3 tempL = this->player.pos + this->player.vec_left*(2.0f);
	D3DXVECTOR3 tempR = this->player.pos + this->player.vec_right*(2.0f);

	switch (dir)
	{
	case 1://forward
		temp += this->player.vec_front*(this->player.moveSpd + 4.0f);
		tempL += this->player.vec_front*this->player.moveSpd;
		tempR += this->player.vec_front*this->player.moveSpd;
		break;
	case 2:
		temp += this->player.vec_back*(this->player.moveSpd + 4.0f);
		tempL += this->player.vec_back*this->player.moveSpd;
		tempR += this->player.vec_back*this->player.moveSpd;
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
