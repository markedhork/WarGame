#include "Bullet.h"
#include <string>
#include "Map.h"

Bullet::Bullet()
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		bullets[i].use = false;
		bulletsEnemy[i].use = false;
	}
}

Bullet::~Bullet()
{
	this->Uninitialize();
}

void Bullet::SetDevice(LPDIRECT3DDEVICE9 pD3Device)
{
	this->pDevice = pD3Device;
}

void Bullet::Update()
{
	D3DXVECTOR2 p0 = D3DXVECTOR2(-7.5f*SIZE, 7.5f*SIZE);
	D3DXVECTOR2 p1 = D3DXVECTOR2(7.5f*SIZE, -7.5f*SIZE);
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (bullets[i].use == true)
		{
			bullets[i].pos += bullets[i].spd;

			if (bullets[i].pos.x<p0.x || bullets[i].pos.x>p1.x || bullets[i].pos.z > p0.y || bullets[i].pos.z < p1.y)
			{
				bullets[i].use = false;
			}
			D3DXVECTOR3 temp = bullets[i].pos;
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
				bullets[i].use = false;
			}
		}
	}
}

int Bullet::UpdateEnemy(D3DXVECTOR3 pos)
{
	int countDMG = 0;
	D3DXVECTOR2 p0 = D3DXVECTOR2(-7.5f*SIZE, 7.5f*SIZE);
	D3DXVECTOR2 p1 = D3DXVECTOR2(7.5f*SIZE, -7.5f*SIZE);
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (bulletsEnemy[i].use == true)
		{
			bulletsEnemy[i].pos += bulletsEnemy[i].spd;

			if (bulletsEnemy[i].pos.x<p0.x || bulletsEnemy[i].pos.x>p1.x || bulletsEnemy[i].pos.z > p0.y || bulletsEnemy[i].pos.z < p1.y)
			{
				bulletsEnemy[i].use = false;
			}
			D3DXVECTOR3 temp = bulletsEnemy[i].pos;
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
				bulletsEnemy[i].use = false;
			}

			float distance = sqrtf((bulletsEnemy[i].pos.x - pos.x)*(bulletsEnemy[i].pos.x - pos.x)
				+ (bulletsEnemy[i].pos.z - pos.z)*(bulletsEnemy[i].pos.z - pos.z));

			if (distance < 0.2f)
			{
				bulletsEnemy[i].use = false;
				countDMG++;
			}
		}
	}
	return countDMG;
}

void Bullet::Draw()
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (this->bullets[i].use == true)
		{
			D3DXMATRIX mtxWorld;
			D3DXMatrixIdentity(&mtxWorld);	//ワールド行列の単位行列の初期化
			D3DXMATRIX mtxRot;
			D3DXMATRIX mtxTrs;
			D3DXMATRIX mtxScl;
			D3DXMatrixScaling(&mtxScl, 0.1f, 0.1f, 0.1f);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);	//World*Rotation
			D3DXMatrixIdentity(&mtxRot);
			D3DXVECTOR3 vec_x;
			D3DXVECTOR3 vec_y;
			D3DXVECTOR3 vec_z;
			vec_y = D3DXVECTOR3(0, 1, 0);
			vec_z = bullets[i].dir;
			//外積
			D3DXVec3Cross(&vec_x, &vec_y, &vec_z);
			//11,12,13-x  21,22,23-y  31,32,33-z
			mtxRot._11 = vec_x.x;
			mtxRot._12 = vec_x.y;
			mtxRot._13 = vec_x.z;
			mtxRot._21 = vec_y.x;
			mtxRot._22 = vec_y.y;
			mtxRot._23 = vec_y.z;
			mtxRot._31 = vec_z.x;
			mtxRot._32 = vec_z.y;
			mtxRot._33 = vec_z.z;
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);	//World*Rotation
			//D3DXMatrixRotationQuaternion
			D3DXMatrixTranslation(&mtxTrs, this->bullets[i].pos.x, this->bullets[i].pos.y, this->bullets[i].pos.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrs);	//World*Tranlation
			pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);
			pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
			for (int i = 0; i < numMaterials; i++)    // loop through each subset
			{
				pDevice->SetTexture(0, texture);
				pDevice->SetMaterial(&material[i]);    // set the appropriate material for the subset
				pDXMeshModel->DrawSubset(i);    // draw the subset
			}

		}
		if (this->bulletsEnemy[i].use == true)
		{
			D3DXMATRIX mtxWorld;
			D3DXMatrixIdentity(&mtxWorld);	//ワールド行列の単位行列の初期化
			D3DXMATRIX mtxRot;
			D3DXMATRIX mtxTrs;
			D3DXMATRIX mtxScl;
			D3DXMatrixScaling(&mtxScl, 0.1f, 0.1f, 0.1f);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);	//World*Rotation
			D3DXMatrixIdentity(&mtxRot);
			D3DXVECTOR3 vec_x;
			D3DXVECTOR3 vec_y;
			D3DXVECTOR3 vec_z;
			vec_y = D3DXVECTOR3(0, 1, 0);
			vec_z = bulletsEnemy[i].dir;
			//外積
			D3DXVec3Cross(&vec_x, &vec_y, &vec_z);
			//11,12,13-x  21,22,23-y  31,32,33-z
			mtxRot._11 = vec_x.x;
			mtxRot._12 = vec_x.y;
			mtxRot._13 = vec_x.z;
			mtxRot._21 = vec_y.x;
			mtxRot._22 = vec_y.y;
			mtxRot._23 = vec_y.z;
			mtxRot._31 = vec_z.x;
			mtxRot._32 = vec_z.y;
			mtxRot._33 = vec_z.z;
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);	//World*Rotation
			//D3DXMatrixRotationQuaternion
			D3DXMatrixTranslation(&mtxTrs, this->bulletsEnemy[i].pos.x, this->bulletsEnemy[i].pos.y, this->bulletsEnemy[i].pos.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrs);	//World*Tranlation
			pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);
			pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
			for (int i = 0; i < numMaterials; i++)    // loop through each subset
			{
				pDevice->SetTexture(0, texture);
				pDevice->SetMaterial(&material[i]);    // set the appropriate material for the subset
				pDXMeshModel->DrawSubset(i);    // draw the subset
			}

		}
	}
}

void Bullet::Generate(D3DXVECTOR3 pos, D3DXVECTOR3 dir)
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (bullets[i].use == false)
		{
			bullets[i].use = true;
			bullets[i].dir = dir;
			bullets[i].pos = pos;
			bullets[i].pos.y += 0.4f;
			bullets[i].pos += dir * 0.1f;
			bullets[i].spd = dir * BULLET_SPD;
			break;
		}
	}
}

void Bullet::GenerateEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 dir)
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (bulletsEnemy[i].use == false)
		{
			bulletsEnemy[i].use = true;
			bulletsEnemy[i].dir = dir;
			bulletsEnemy[i].pos = pos;
			bulletsEnemy[i].pos.y += 0.4f;
			bulletsEnemy[i].pos += dir * 0.1f;
			bulletsEnemy[i].spd = dir * BULLET_SPD;
			break;
		}
	}
}

void Bullet::CreateMeshBuffer()
{
	LPD3DXBUFFER	bufShipMaterials;		//マテリアルなどのインタフェース


	if (D3D_OK != D3DXLoadMeshFromX(
		MESH_FILE_PATH,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&bufShipMaterials,
		NULL,
		&numMaterials,
		&pDXMeshModel))
	{
		OutputDebugStringA("FAILED TO LOAD MESH FROM X FILE\n");
	}

	D3DXMATERIAL* tempMaterials = (D3DXMATERIAL*)bufShipMaterials->GetBufferPointer();
	material = new D3DMATERIAL9[numMaterials];
	for (DWORD i = 0; i < numMaterials; i++)    // for each material...
	{
		material[i] = tempMaterials[i].MatD3D;    // get the material info...
		material[i].Ambient = material[i].Diffuse;    // and make ambient the same as diffuse

		std::string addr = "asset/textures/";
		addr += tempMaterials[i].pTextureFilename;
		// if there is a texture to load, load it
		if (FAILED(D3DXCreateTextureFromFileA(this->pDevice,
			addr.c_str(),
			&texture)))
			texture = NULL;    // if there is no texture, set the texture to NULL
	}
}

void Bullet::Uninitialize()
{
	if (pDXMeshModel != NULL)
	{
		pDXMeshModel->Release();
		pDXMeshModel = NULL;
	}
}
