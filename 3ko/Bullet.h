#pragma once
#include <d3dx9.h>


#define MAX_BULLETS				(200)
#define BULLET_SPD				(0.1f)
#define MESH_FILE_PATH			"asset/mesh/Bullet.x"


struct bulletStruct
{
	bool use = false;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 spd;
	D3DXVECTOR3 dir;
};

class Bullet
{
public:
	Bullet();
	~Bullet();

	void SetDevice(LPDIRECT3DDEVICE9 pD3Device);
	void Update();
	int UpdateEnemy(D3DXVECTOR3 pos);
	void Draw();
	void Generate(D3DXVECTOR3 pos, D3DXVECTOR3 dir);
	void GenerateEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 dir);
	void CreateMeshBuffer();
	void Uninitialize();

private:
	bulletStruct	bullets[MAX_BULLETS];
	bulletStruct	bulletsEnemy[MAX_BULLETS];
	LPD3DXMESH		pDXMeshModel;		//モデルを表すインタフェース
	LPDIRECT3DTEXTURE9 texture = NULL;

	DWORD			numMaterials;		//マテリアルの数s
	D3DMATERIAL9	*material;    // create the material struct
	LPDIRECT3DDEVICE9 pDevice;
};

