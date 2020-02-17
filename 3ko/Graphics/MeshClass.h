#pragma once
#include <d3dx9.h>

#define MESH_FILENAME_MAX (64)
#define SAFE_RELEASE(p){if(p) {(p)->Release(); (p)=NULL;}}
#define SAFE_DELETE(p) {if(p) {delete(p); (p)=NULL;}}
#define SAFE_DELETE_ARRAY(p) {if(p) {delete[](p); (p)=NULL;}}

// mesh�t�@�C���Ǘ��\����
typedef struct MeshFile_tag
{
	char filename[MESH_FILENAME_MAX]; // mesh�t�@�C����

} MeshFile;

static const MeshFile g_MeshFiles[] = {

	{ "asset/mesh/Soldier01.x"},
	{ "asset/mesh/Soldier02.x"},
	{ "asset/mesh/Soldier03.x"},
};


typedef enum MeshIndex
{
	MESH_SOLDIER01,
	MESH_SOLDIER02,
	MESH_SOLDIER03,

	MAX_MESH,

};

enum PLAYER_ANIM_INDEX
{
	IDLE,
	MOVE,
	AIM,
	RELOAD,

	MAX_ANIM,
};

struct MeshMember
{
	LPD3DXFRAME TopFrame = NULL;    // the top-most frame of the model
	D3DXMATRIX* FinalMatrices = NULL;    // a buffer for the matrices in their final state
	LPD3DXANIMATIONCONTROLLER AnimationController = NULL;    // an object that handles animation

	UINT currentTrack;
	DWORD Action[MAX_ANIM] = {};
};
struct CUSTOM_FRAME : public D3DXFRAME
{
	D3DXMATRIX CombTransformationMatrix;    // transform data multiplied by all parents' data
};

struct CUSTOM_MESHCONTAINER : public D3DXMESHCONTAINER
{
	D3DXMATRIX** ppFrameMatrices;    // an array of matrices to store current animation data
	LPD3DXMESH pFinalMesh;    // a duplicate of the mesh, representing it's current pose
	LPDIRECT3DTEXTURE9* pTextures;    // an array of applicable textures
};

class MeshClass
{
public:
	void CreateMeshBuffer(HWND hwnd, LPDIRECT3DDEVICE9 pDevice);
	void update_frames(CUSTOM_FRAME* pFrame, D3DXMATRIX* pParentMatrix);
	void update_mesh_containers(int index, CUSTOM_FRAME* pFrame);
	void draw_mesh(int index, CUSTOM_FRAME* pFrame);

public:
	MeshMember				*mesh_members;
private:
	DWORD GetIndexOfAnimSet(int meshIndex, LPCSTR string);
	void link_frames(int index, CUSTOM_FRAME* pFrame);
private:
	LPDIRECT3DDEVICE9		pDevice;


};

class MeshAllocation :public ID3DXAllocateHierarchy
{
public:
	void SetDevice(LPDIRECT3DDEVICE9 device);
	STDMETHOD(CreateFrame)(LPCSTR Name, LPD3DXFRAME* ppNewFrame);
	STDMETHOD(CreateMeshContainer)(LPCSTR Name,
		CONST D3DXMESHDATA* pMeshData,
		CONST D3DXMATERIAL* pMaterials,
		CONST D3DXEFFECTINSTANCE* pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD* pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER* ppNewMeshContainer);
	STDMETHOD(DestroyFrame)(LPD3DXFRAME pFrameToFree);
	STDMETHOD(DestroyMeshContainer)(LPD3DXMESHCONTAINER pMeshContainerToFree);

public:
	int maxFrames;
private:
	LPDIRECT3DDEVICE9		pD3DDevice;

};