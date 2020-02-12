#pragma once
#include <d3dx9.h>
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3dx9.lib")
#include "Texture.h"

#define FVF_VERTEX3D (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)


struct Sprite
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 rot;
	D3DXVECTOR2 size;
	int index;
};

struct Mesh
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 rot;
	D3DXVECTOR3 size;
	int index;
};

struct VERTEX_3D
{
	D3DXVECTOR3 vtx;	//���_���W
	D3DXVECTOR3 nor;	//�@���x�N�g��
	D3DCOLOR diffuse;	//���ˌ�
	D3DXVECTOR2 tex;	//�e�N�X�`�����W
};