//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "Scene.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers, int nRootParameters)
{
	m_nTextureType = nTextureType;

	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_ppd3dTextures = new ID3D12Resource * [m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_ppd3dTextures[i] = NULL;
		m_ppd3dTextureUploadBuffers = new ID3D12Resource * [m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_ppd3dTextureUploadBuffers[i] = NULL;
		m_pd3dSrvGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pd3dSrvGpuDescriptorHandles[i].ptr = NULL;

		m_pnResourceTypes = new UINT[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pnResourceTypes[i] = -1;

		m_pdxgiBufferFormats = new DXGI_FORMAT[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pnResourceTypes[i] = DXGI_FORMAT_UNKNOWN;
		m_pnBufferElements = new int[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pnBufferElements[i] = 0;
	}
	m_nRootParameters = nRootParameters;
	if (nRootParameters > 0) m_pnRootParameterIndices = new int[nRootParameters];
	for (int i = 0; i < m_nRootParameters; i++) m_pnRootParameterIndices[i] = -1;

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
		delete[] m_ppd3dTextures;
	}
	if (m_pnResourceTypes) delete[] m_pnResourceTypes;
	if (m_pdxgiBufferFormats) delete[] m_pdxgiBufferFormats;
	if (m_pnBufferElements) delete[] m_pnBufferElements;

	if (m_pnRootParameterIndices) delete[] m_pnRootParameterIndices;
	if (m_pd3dSrvGpuDescriptorHandles) delete[] m_pd3dSrvGpuDescriptorHandles;

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootParameterIndex(int nIndex, UINT nRootParameterIndex)
{
	m_pnRootParameterIndices[nIndex] = nRootParameterIndex;
}

void CTexture::SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_nRootParameters == m_nTextures)
	{
		for (int i = 0; i < m_nRootParameters; i++)
		{
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[i], m_pd3dSrvGpuDescriptorHandles[i]);
		}
	}
	else
	{
		pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[0], m_pd3dSrvGpuDescriptorHandles[0]);
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[nParameterIndex], m_pd3dSrvGpuDescriptorHandles[nTextureIndex]);
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void CTexture::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nIndex)
{
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ);
}

void CTexture::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ/*D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/);
}

void CTexture::LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = RESOURCE_BUFFER;
	m_pdxgiBufferFormats[nIndex] = ndxgiFormat;
	m_pnBufferElements[nIndex] = nElements;
	m_ppd3dTextures[nIndex] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pData, nElements * nStride, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, &m_ppd3dTextureUploadBuffers[nIndex]);
}

ID3D12Resource* CTexture::CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTexture2DResource(pd3dDevice, pd3dCommandList, nWidth, nHeight, nElements, nMipLevels, dxgiFormat, d3dResourceFlags, d3dResourceStates, pd3dClearValue);
	return(m_ppd3dTextures[nIndex]);
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc(int nIndex)
{
	ID3D12Resource* pShaderResource = GetResource(nIndex);
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	int nTextureType = GetTextureType(nIndex);
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: //[]
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.Format = m_pdxgiBufferFormats[nIndex];
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = m_pnBufferElements[nIndex];
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial()
{
}

CMaterial::~CMaterial()
{
	if (m_pTexture) m_pTexture->Release();
	if (m_pShader) m_pShader->Release();
}

void CMaterial::SetTexture(CTexture* pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CMaterial::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pShader)
	{
		m_pShader->UpdateShaderVariables(pd3dCommandList);
	}
	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader) m_pShader->ReleaseShaderVariables();
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject(int nMeshes)
{
	m_xmf4x4World = Matrix4x4::Identity();

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh * [m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	ReleaseShaderVariables();

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}
	if (m_pMaterial) m_pMaterial->Release();
}

void CGameObject::SetMesh(int nIndex, CMesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}

void CGameObject::SetShader(CShader* pShader)
{
	if (!m_pMaterial)
	{
		CMaterial* pMaterial = new CMaterial();
		SetMaterial(pMaterial);
	}
	if (m_pMaterial) m_pMaterial->SetShader(pShader);
}

void CGameObject::SetMaterial(CMaterial* pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void**)&m_pcbMappedGameObject);
}

void CGameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}
	if (m_pMaterial) m_pMaterial->ReleaseShaderVariables();
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);

	if (m_pcbMappedGameObject)
	{
		XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
		if (m_pMaterial) m_pcbMappedGameObject->m_nMaterial = m_pMaterial->m_nReflection;
	}
}

void CGameObject::Animate(float fDeltaTime)
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	if (m_pMaterial && m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
	UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}

	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::SetLookAt(XMFLOAT3& xmf3Target, XMFLOAT3& xmf3Up)
{
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(xmf3Position, xmf3Target, xmf3Up);
	m_xmf4x4World._11 = mtxLookAt._11; m_xmf4x4World._12 = mtxLookAt._21; m_xmf4x4World._13 = mtxLookAt._31;
	m_xmf4x4World._21 = mtxLookAt._12; m_xmf4x4World._22 = mtxLookAt._22; m_xmf4x4World._23 = mtxLookAt._32;
	m_xmf4x4World._31 = mtxLookAt._13; m_xmf4x4World._32 = mtxLookAt._23; m_xmf4x4World._33 = mtxLookAt._33;
	/*
		XMFLOAT3 xmf3Look = Vector3::Normalize(Vector3::Subtract(xmf3Target, xmf3Position));
		XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
		xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);
		m_xmf4x4World._11 = xmf3Right.x; m_xmf4x4World._12 = xmf3Right.y; m_xmf4x4World._13 = xmf3Right.z;
		m_xmf4x4World._21 = xmf3Up.x; m_xmf4x4World._22 = xmf3Up.y; m_xmf4x4World._23 = xmf3Up.z;
		m_xmf4x4World._31 = xmf3Look.x; m_xmf4x4World._32 = xmf3Look.y; m_xmf4x4World._33 = xmf3Look.z;
		*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CRotatingObject::CRotatingObject(int nMeshes)
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 15.0f;
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float fDeltaTime)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fDeltaTime);
}

void CRotatingObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CRevolvingObject::CRevolvingObject(int nMeshes)
{
	m_xmf3RevolutionAxis = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_fRevolutionSpeed = 0.0f;
}

CRevolvingObject::~CRevolvingObject()
{
}

void CRevolvingObject::Animate(float fDeltaTime)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3RevolutionAxis), XMConvertToRadians(m_fRevolutionSpeed * fDeltaTime));
	m_xmf4x4World = Matrix4x4::Multiply(m_xmf4x4World, mtxRotate);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(0)
{
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength);

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	long cxBlocks = (nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (nLength - 1) / czQuadsPerBlock;

#ifdef _WITH_VERTICES_AS_SCALING
	nWidth = int(nWidth * xmf3Scale.x);
	nLength = int(nLength * xmf3Scale.z);
	nBlockWidth = int(nBlockWidth * xmf3Scale.x);
	nBlockLength = int(nBlockLength * xmf3Scale.z);
#endif

	m_nWidth = nWidth;
	m_nLength = nLength;

	m_xmf3Scale = xmf3Scale;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new CMesh * [m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;

	CHeightMapGridMesh* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pTerrainTexture = new CTexture(3, RESOURCE_TEXTURE2D, 0, 3);

	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Image/Base_Texture.dds", RESOURCE_TEXTURE2D, 0);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Image/Detail_Texture_7.dds", RESOURCE_TEXTURE2D, 1);
	pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Image/HeightMap(Alpha).dds", RESOURCE_TEXTURE2D, 2);
	CScene::CreateShaderResourceViews(pd3dDevice, pTerrainTexture, 0, 7);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

#ifdef _WITH_TERRAIN_TESSELATION
	CTerrainTessellationShader* pTerrainShader = new CTerrainTessellationShader();
#else
	CTerrainShader* pTerrainShader = new CTerrainShader();
#endif
	pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = CScene::CreateConstantBufferView(pd3dDevice, m_pd3dcbGameObject, ncbElementBytes);
	SetCbvGPUDescriptorHandle(d3dCbvGPUDescriptorHandle);

	CMaterial* pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);

	SetMaterial(pTerrainMaterial);

	SetShader(pTerrainShader);
}

float CHeightMapTerrain::GetHeight(float fx, float fz) //World Coordinates
{ 
	float fHeight = m_pHeightMapImage->GetHeight(fx, fz, m_xmf3Scale);

	return(fHeight); 
} 

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
CSkyBox::CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(6)
{
	CTexturedRectMesh* pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	SetMesh(0, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	SetMesh(1, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, 0.0f);
	SetMesh(2, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, +10.0f, 0.0f, 0.0f);
	SetMesh(3, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, +10.0f, 0.0f);
	SetMesh(4, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, -10.0f, 0.0f);
	SetMesh(5, pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pSkyBoxTexture = new CTexture(6, RESOURCE_TEXTURE2D, 0, 1);

	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Front_0.dds", RESOURCE_TEXTURE2D, 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Back_0.dds", RESOURCE_TEXTURE2D, 1);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Left_0.dds", RESOURCE_TEXTURE2D, 2);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Right_0.dds", RESOURCE_TEXTURE2D, 3);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Top_0.dds", RESOURCE_TEXTURE2D, 4);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Image/SkyBox_Bottom_0.dds", RESOURCE_TEXTURE2D, 5);
	CScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 0, 6);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = CScene::CreateConstantBufferView(pd3dDevice, m_pd3dcbGameObject, ncbElementBytes);
	SetCbvGPUDescriptorHandle(d3dCbvGPUDescriptorHandle);

	CMaterial* pSkyBoxMaterial = new CMaterial();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);

	SetMaterial(pSkyBoxMaterial);

	SetShader(pSkyBoxShader);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	OnPrepareRender();

	if (m_pMaterial && m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
	UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_pMaterial && m_pMaterial->m_pTexture) m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0, i);
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
CGrassObject::CGrassObject()
{
}

CGrassObject::~CGrassObject()
{
}

void CGrassObject::Animate(float fDeltaTime)
{
	if (m_fRotationAngle <= -1.5f) m_fRotationDelta = 1.0f;
	if (m_fRotationAngle >= +1.5f) m_fRotationDelta = -1.0f;
	m_fRotationAngle += m_fRotationDelta * fDeltaTime;

	Rotate(0.0f, 0.0f, m_fRotationAngle);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
CDynamicCubeMappingObject::CDynamicCubeMappingObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LONG nCubeMapSize, D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle, D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle, CShader* pShader)
{
//Camera[6]
	for (int j = 0; j < 6; j++)
	{
		m_ppCameras[j] = new CCamera();
		m_ppCameras[j]->SetViewport(0, 0, nCubeMapSize, nCubeMapSize, 0.0f, 1.0f);
		m_ppCameras[j]->SetScissorRect(0, 0, nCubeMapSize, nCubeMapSize);
		m_ppCameras[j]->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		m_ppCameras[j]->GenerateProjectionMatrix(0.1f, 5000.0f, 1.0f, 90.0f);
	}

//Depth Buffer & View
	D3D12_CLEAR_VALUE d3dDsbClearValue = { DXGI_FORMAT_D24_UNORM_S8_UINT, { 1.0f, 0 } };
	m_pd3dDepthStencilBuffer = ::CreateTexture2DResource(pd3dDevice, pd3dCommandList, nCubeMapSize, nCubeMapSize, 1, 1, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dDsbClearValue);

	m_d3dDsvCPUDescriptorHandle = d3dDsvCPUDescriptorHandle;
	pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, m_d3dDsvCPUDescriptorHandle);

	CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0, 1);
	D3D12_CLEAR_VALUE d3dRtbClearValue = { DXGI_FORMAT_R8G8B8A8_UNORM, { 0.0f, 0.0f, 0.0f, 1.0f } };
	ID3D12Resource* pd3dResource = pTexture->CreateTexture(pd3dDevice, pd3dCommandList, nCubeMapSize, nCubeMapSize, 6, 1, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, &d3dRtbClearValue, RESOURCE_TEXTURE_CUBE, 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pTexture, 0, 11);

	D3D12_RENDER_TARGET_VIEW_DESC d3dRTVDesc;
	d3dRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
	d3dRTVDesc.Texture2DArray.MipSlice = 0;
	d3dRTVDesc.Texture2DArray.PlaneSlice = 0;
	d3dRTVDesc.Texture2DArray.ArraySize = 1;

	for (int j = 0; j < 6; j++)
	{
		m_pd3dRtvCPUDescriptorHandles[j] = d3dRtvCPUDescriptorHandle;
		d3dRTVDesc.Texture2DArray.FirstArraySlice = j; //i-번째 렌더 타겟 뷰는 텍스쳐 큐브의 i-번째 버퍼에서 시작
		pd3dDevice->CreateRenderTargetView(pd3dResource, &d3dRTVDesc, m_pd3dRtvCPUDescriptorHandles[j]);
		d3dRtvCPUDescriptorHandle.ptr += ::gnRtvDescriptorIncrementSize;
	}

	CMaterial* pMaterial = new CMaterial();
	pMaterial->m_pReflection = new MATERIAL;
	pMaterial->m_pReflection->m_xmf4Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pMaterial->m_pReflection->m_xmf4Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pMaterial->m_pReflection->m_xmf4Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 3.0f);
	pMaterial->m_pReflection->m_xmf4Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	pMaterial->SetTexture(pTexture);

	SetMaterial(pMaterial);
}

CDynamicCubeMappingObject::~CDynamicCubeMappingObject()
{
	for (int j = 0; j < 6; j++)
	{
		if (m_ppCameras[j])
		{
			m_ppCameras[j]->ReleaseShaderVariables();
			delete m_ppCameras[j];
		}
	}

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
}

void CDynamicCubeMappingObject::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, CScene* pScene)
{
	pScene->OnPrepareRender(pd3dCommandList);

	static XMFLOAT3 pxmf3LookAts[6] = { XMFLOAT3(+100.0f, 0.0f, 0.0f), XMFLOAT3(-100.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, +100.0f, 0.0f), XMFLOAT3(0.0f, -100.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, +100.0f), XMFLOAT3(0.0f, 0.0f, -100.0f) };
	static XMFLOAT3 pxmf3Ups[6] = { XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, +1.0f), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT3(0.0f, +1.0f, 0.0f) };

	float pfClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	::SynchronizeResourceTransition(pd3dCommandList, m_pMaterial->m_pTexture->GetResource(0), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);

	XMFLOAT3 xmf3Position = GetPosition();
	for (int j = 0; j < 6; j++)
	{
		m_ppCameras[j]->SetPosition(xmf3Position);
		m_ppCameras[j]->GenerateViewMatrix(xmf3Position, Vector3::Add(xmf3Position, pxmf3LookAts[j]), pxmf3Ups[j]);

		pd3dCommandList->ClearRenderTargetView(m_pd3dRtvCPUDescriptorHandles[j], pfClearColor, 0, NULL);
		pd3dCommandList->ClearDepthStencilView(m_d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

		pd3dCommandList->OMSetRenderTargets(1, &m_pd3dRtvCPUDescriptorHandles[j], TRUE, &m_d3dDsvCPUDescriptorHandle);

		pScene->Render(pd3dCommandList, m_ppCameras[j]);
	}

	::SynchronizeResourceTransition(pd3dCommandList, m_pMaterial->m_pTexture->GetResource(0), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
}
