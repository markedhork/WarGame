#pragma once
#include <d3dx9.h>


#define MAX_BULLETS				(30)
#define BULLET_SPD				(1.0f)
#define MESH_FILE_PATH			"asset/mesh/cakeBullet.x"


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
	void Draw();
	void Generate(D3DXVECTOR3 pos, D3DXVECTOR3 dir);
	void CreateMeshBuffer();
	void Uninitialize();

private:
	bulletStruct	bullets[MAX_BULLETS];
	LPD3DXMESH		pDXMeshModel;		//���f����\���C���^�t�F�[�X
	LPDIRECT3DTEXTURE9 texture = NULL;

	DWORD			numMaterials;		//�}�e���A���̐�
	D3DMATERIAL9	*material;    // create the material struct
	LPDIRECT3DDEVICE9 pDevice;
};

