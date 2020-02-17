#include "MeshClass.h"
#include <string>
void MeshClass::CreateMeshBuffer(HWND hwnd, LPDIRECT3DDEVICE9 pDevice)
{
	this->pDevice = pDevice;
	this->mesh_members = new MeshMember[MAX_MESH];

	for (int i = 0; i < MAX_MESH; i++) {
		MeshAllocation memAllocator;
		memAllocator.SetDevice(pDevice);
		LPD3DXBUFFER	bufShipMaterials;		//マテリアルなどのインタフェース
		//D3DXCreateAnimationController(,)

		if (D3D_OK != D3DXLoadMeshHierarchyFromX(
			g_MeshFiles[i].filename,
			D3DXMESH_MANAGED,
			pDevice,
			&memAllocator,
			NULL,
			&mesh_members[i].TopFrame,
			&mesh_members[i].AnimationController))
		{
			MessageBox(hwnd, "FAILED TO LOAD MESH FROM X FILE", "エラー", MB_OK);
			OutputDebugStringA("FAILED TO LOAD MESH FROM X FILE\n");
		}


		// allocate memory for the FinalMatrices array
		mesh_members[i].FinalMatrices = new D3DXMATRIX[memAllocator.maxFrames];
		ZeroMemory(mesh_members[i].FinalMatrices, sizeof(D3DXMATRIX) * memAllocator.maxFrames);

		// set up the ppFrameMatrices pointers
		link_frames(i, (CUSTOM_FRAME*)mesh_members[i].TopFrame);


		mesh_members[i].currentTrack = 0;

		LPD3DXANIMATIONSET pAnimationSet = NULL;
		mesh_members[i].Action[IDLE] = this->GetIndexOfAnimSet(i, "Idle");
		mesh_members[i].Action[MOVE] = this->GetIndexOfAnimSet(i, "Move");
		mesh_members[i].Action[AIM] = this->GetIndexOfAnimSet(i, "Aim");
		mesh_members[i].Action[RELOAD] = this->GetIndexOfAnimSet(i, "Reload");

		mesh_members[i].AnimationController->GetAnimationSet(mesh_members[i].Action[IDLE], &pAnimationSet);
		mesh_members[i].AnimationController->SetTrackAnimationSet(mesh_members[i].currentTrack, pAnimationSet);

		pAnimationSet->Release();
		//mesh_members[i].AnimationController->UnkeyAllTrackEvents(mesh_members[i].tracks[anim]);
		//mesh_members[i].AnimationController->KeyTrackSpeed(mesh_members[i].tracks[anim], 1.0f, 0.01f, 0.25f, D3DXTRANSITION_LINEAR);
		//mesh_members[i].AnimationController->KeyTrackWeight(mesh_members[i].tracks[anim], 1.0f, 0.01f, 0.25f, D3DXTRANSITION_LINEAR);

		mesh_members[i].AnimationController->SetTrackEnable(mesh_members[i].currentTrack, TRUE);

	}
}

void MeshClass::update_frames(CUSTOM_FRAME * pFrame, D3DXMATRIX * pParentMatrix)
{
	// combine the frame's matrix with the parent's matrix, if any
	if (pParentMatrix)
		pFrame->CombTransformationMatrix = pFrame->TransformationMatrix * *pParentMatrix;
	else
		pFrame->CombTransformationMatrix = pFrame->TransformationMatrix;

	// run for all siblings
	if (pFrame->pFrameSibling)
		update_frames((CUSTOM_FRAME*)pFrame->pFrameSibling, pParentMatrix);

	// run for the first child (which will then run all other children)
	if (pFrame->pFrameFirstChild)
		update_frames((CUSTOM_FRAME*)pFrame->pFrameFirstChild,
			&pFrame->CombTransformationMatrix);
}

void MeshClass::update_mesh_containers(int index, CUSTOM_FRAME * pFrame)
{
	// cast the pFrame's mesh container pointer to a CUSTOM_MESHCONTAINER*
	CUSTOM_MESHCONTAINER* pMeshContainer = (CUSTOM_MESHCONTAINER*)pFrame->pMeshContainer;

	if (pMeshContainer && pMeshContainer->pSkinInfo)
	{
		UINT NumFrames = pMeshContainer->pSkinInfo->GetNumBones();    // find how many frames
		// for each frame in the mesh container...
		for (UINT i = 0; i < NumFrames; i++)
		{
			// set FinalMatrices to that frame's offset matrix
			this->mesh_members[index].FinalMatrices[i] = *pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i);

			// multiply that by the animated frame matrix
			this->mesh_members[index].FinalMatrices[i] *= *pMeshContainer->ppFrameMatrices[i];
		}

		void* pSrc = NULL;    // a void pointer for the original mesh
		void* pDst = NULL;    // a void pointer for the modified mesh

		// lock the two meshes
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(NULL, &pSrc);
		pMeshContainer->pFinalMesh->LockVertexBuffer(NULL, &pDst);

		// store the animated mesh into FinalMesh
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(this->mesh_members[index].FinalMatrices, NULL, pSrc, pDst);

		// unlock the two meshes
		pMeshContainer->pFinalMesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
	}

	// run for all siblings
	if (pFrame->pFrameSibling)
		update_mesh_containers(index, (CUSTOM_FRAME*)pFrame->pFrameSibling);

	// run for the first child (which will then run all other children)
	if (pFrame->pFrameFirstChild)
		update_mesh_containers(index, (CUSTOM_FRAME*)pFrame->pFrameFirstChild);
}

void MeshClass::draw_mesh(int index, CUSTOM_FRAME * pFrame)
{
	// cast the pFrame's mesh container pointer to a CUSTOM_MESHCONTAINER*
	CUSTOM_MESHCONTAINER* pMeshContainer = (CUSTOM_MESHCONTAINER*)pFrame->pMeshContainer;

	if (pMeshContainer)
	{
		// for each material...
		for (UINT i = 0; i < pMeshContainer->NumMaterials; i++)
		{
			// set the material
			pDevice->SetMaterial(&pMeshContainer->pMaterials[i].MatD3D);
			// set the texture
			pDevice->SetTexture(0, pMeshContainer->pTextures[i]);

			// draw the subset!
			pMeshContainer->pFinalMesh->DrawSubset(i);
		}
	}

	// run for all siblings
	if (pFrame->pFrameSibling)
		draw_mesh(index, (CUSTOM_FRAME*)pFrame->pFrameSibling);

	// run for the first child (which will then run all other children)
	if (pFrame->pFrameFirstChild)
		draw_mesh(index, (CUSTOM_FRAME*)pFrame->pFrameFirstChild);
}

DWORD MeshClass::GetIndexOfAnimSet(int meshIndex, LPCSTR string)
{
	HRESULT hr;
	LPD3DXANIMATIONSET pAS;
	DWORD dwRet = -1;

	for (DWORD i = 0; i < mesh_members[meshIndex].AnimationController->GetNumAnimationSets(); ++i)
	{
		hr = mesh_members[meshIndex].AnimationController->GetAnimationSet(i, &pAS);
		if (FAILED(hr))
			continue;

		if (pAS->GetName() &&
			!strncmp(pAS->GetName(), string, min(strlen(pAS->GetName()), strlen(string))))
		{
			dwRet = i;
			pAS->Release();
			break;
		}
		pAS->Release();
	}
	return dwRet;
}

void MeshClass::link_frames(int index, CUSTOM_FRAME * pFrame)
{
	// cast the pFrame's mesh container pointer to a CUSTOM_MESHCONTAINER*
	CUSTOM_MESHCONTAINER* pMeshContainer = (CUSTOM_MESHCONTAINER*)pFrame->pMeshContainer;

	// if there is a mesh container and if it has skin info...
	if (pMeshContainer && pMeshContainer->pSkinInfo)
	{
		// loop through each frame in the mesh container
		for (UINT i = 0; i < pMeshContainer->pSkinInfo->GetNumBones(); i++)
		{
			CUSTOM_FRAME* pTempFrame;    // a temporary frame pointer

			// find each frame by name
			pTempFrame = (CUSTOM_FRAME*)D3DXFrameFind(this->mesh_members[index].TopFrame,
				pMeshContainer->pSkinInfo->GetBoneName(i));
			// and set up a pointer to it
			pMeshContainer->ppFrameMatrices[i] = &pTempFrame->CombTransformationMatrix;
		}
	}

	// run for all siblings
	if (pFrame->pFrameSibling)
		link_frames(index, (CUSTOM_FRAME*)pFrame->pFrameSibling);

	// run for the first child (which will then run all other children)
	if (pFrame->pFrameFirstChild)
		link_frames(index, (CUSTOM_FRAME*)pFrame->pFrameFirstChild);
}

void MeshAllocation::SetDevice(LPDIRECT3DDEVICE9 device)
{
	this->pD3DDevice = device;
}

STDMETHODIMP_(HRESULT __stdcall) MeshAllocation::CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame)
{
	// Create and initialize the custom frame struct
	CUSTOM_FRAME* pFrame = new CUSTOM_FRAME;
	*ppNewFrame = pFrame;
	ZeroMemory(pFrame, sizeof(CUSTOM_FRAME));

	// Copy the name 
	if (Name)
	{
		UINT len = strlen(Name) + 1;
		pFrame->Name = new char[len];
		memcpy(pFrame->Name, Name, len);
	}

	return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) MeshAllocation::CreateMeshContainer(LPCSTR Name, const D3DXMESHDATA * pMeshData, const D3DXMATERIAL * pMaterials, const D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, const DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer)
{
	// Create and initialize the custom mesh container
	struct CUSTOM_MESHCONTAINER *pMeshContainer = new CUSTOM_MESHCONTAINER;
	*ppNewMeshContainer = pMeshContainer;
	ZeroMemory(pMeshContainer, sizeof(CUSTOM_MESHCONTAINER));

	// Check to make sure its a normal mesh
	if (pMeshData->Type != D3DXMESHTYPE_MESH)
	{
		DestroyMeshContainer(pMeshContainer);
		return E_FAIL;
	}

	// Copy the name
	if (Name)
	{
		UINT len = strlen(Name) + 1;
		pMeshContainer->Name = new char[len];
		memcpy(pMeshContainer->Name, Name, len);
	}

	// Copy the mesh data
	pMeshContainer->MeshData.Type = pMeshData->Type;
	pMeshContainer->MeshData.pMesh = pMeshData->pMesh;
	pMeshContainer->MeshData.pMesh->AddRef();

	// Copy the materials
	pMeshContainer->pMaterials = new D3DXMATERIAL[NumMaterials];
	for (DWORD i = 0; i < NumMaterials; i++)
	{
		pMeshContainer->pMaterials[i] = pMaterials[i];
		pMeshContainer->pMaterials[i].MatD3D.Ambient = pMaterials[i].MatD3D.Diffuse;
	}

	// Don't bother with the effect instance
	pMeshContainer->pEffects = NULL;

	// Copy the number of material
	pMeshContainer->NumMaterials = max(1, NumMaterials);

	// Copy the adjacency data
	pMeshContainer->pAdjacency = new DWORD[pMeshData->pMesh->GetNumFaces() * 3];
	memcpy(pMeshContainer->pAdjacency,
		pAdjacency,
		sizeof(DWORD) * pMeshData->pMesh->GetNumFaces() * 3);

	// Only do these steps if there is skin info
	if (pSkinInfo)
	{
		// Copy the skin data
		pMeshContainer->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		// Allocate memory for the pointers to the frame matrices
		pMeshContainer->ppFrameMatrices = new D3DXMATRIX*[pSkinInfo->GetNumBones()];
		for (DWORD i = 0; i < pSkinInfo->GetNumBones(); i++)
		{
			pMeshContainer->ppFrameMatrices[i] = NULL;
		}
	}

	// Create a duplicate of the mesh
	pMeshContainer->MeshData.pMesh->CloneMesh(
		pMeshContainer->MeshData.pMesh->GetOptions(),
		NULL,
		this->pD3DDevice,
		&pMeshContainer->pFinalMesh);

	// Load the textures for each material (if any)
	pMeshContainer->pTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
	for (DWORD i = 0; i < NumMaterials; i++)
	{
		pMeshContainer->pTextures[i] = 0;
		std::string addr = "asset/textures/";
		addr += pMaterials[i].pTextureFilename;
		if (pMaterials[i].pTextureFilename)
		{
			D3DXCreateTextureFromFileA(this->pD3DDevice,
				addr.c_str(),
				&pMeshContainer->pTextures[i]);
		}
	}

	// Find the maximum number of frames
	if (pMeshContainer->pSkinInfo)
		maxFrames = max(maxFrames, (int)pMeshContainer->pSkinInfo->GetNumBones());

	return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) MeshAllocation::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	// free the name and the frame
	SAFE_DELETE_ARRAY(pFrameToFree->Name);
	SAFE_DELETE(pFrameToFree);

	return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) MeshAllocation::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	// cast the pointer to a CUSTOM_MESHCONTAINER
	CUSTOM_MESHCONTAINER* pMeshContainer = (CUSTOM_MESHCONTAINER*)pMeshContainerToFree;

	// free or release all allocated memory
	SAFE_DELETE_ARRAY(pMeshContainer->Name);
	SAFE_RELEASE(pMeshContainer->MeshData.pMesh);
	SAFE_RELEASE(pMeshContainer->pFinalMesh);
	SAFE_DELETE_ARRAY(pMeshContainer->pMaterials);
	SAFE_DELETE_ARRAY(pMeshContainer->pAdjacency);
	SAFE_RELEASE(pMeshContainer->pSkinInfo);
	SAFE_DELETE_ARRAY(pMeshContainer->ppFrameMatrices);
	for (UINT i = 0; i < pMeshContainer->NumMaterials; i++)
		SAFE_RELEASE(pMeshContainer->pTextures[i]);
	SAFE_DELETE(pMeshContainer);

	return S_OK;
}
