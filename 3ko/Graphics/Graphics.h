#pragma once
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#include "..\\RenderWindow.h"
#include "Vertex.h"
#include <d3d9.h>
#include "..\\Timer.h"
#include "Camera.h"
#include <string>
#include "Texture.h"
#include "MeshClass.h"
#include "../Map.h"

class Graphics
{
public:
	Graphics() {};
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	void RenderFrame_end();
	void Set(Sprite* sprite, int totalSprite, Mesh * mesh, int meshTotal);
	void SwitchTrack(int objIndex, int trackIndex);

	void Draw();
	void DrawSprite();
	void DrawUI();
	void DrawGround();
	void DrawSky();

	LPDIRECT3DDEVICE9 GetDevice();
	Camera camera;
	int playerHP, enemyHP;

	MeshClass			meshController;

private:
	bool InitializeDirectX(HWND hwnd);
	bool UninitializeDirectX(HWND hwnd);
	void Init_light();
	bool InitializeVB();

	LPDIRECT3DDEVICE9	pD3DDevice;
	LPDIRECT3D9			pD3D;
	LPD3DXSPRITE		pD3Dspt;

	Texture				textureController;

	int					windowWidth = 0;
	int					windowHeight = 0;
	Timer				fpsTimer;
	DWORD				*cTime;

	int					totalSprites;
	int					totalMesh;
	Sprite *			pSprite;
	Mesh *				pMesh;

	D3DXMATRIX			mtxWorld;												//���[���h�}�g���N�X

	LPDIRECT3DVERTEXBUFFER9 pVB_Ground;
	LPDIRECT3DVERTEXBUFFER9 pVB_billboard;
	LPDIRECT3DVERTEXBUFFER9 pVB_Cube;
};



