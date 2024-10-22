#include "Object.h"

CSingleTexture::CSingleTexture(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, const wchar_t* pszFileName, bool bDDS)
{
	std::unique_ptr<uint8_t[]> decodedData;	// dds 데이터로도 사용
	std::vector<D3D12_SUBRESOURCE_DATA> vSubresources;
	DDS_ALPHA_MODE ddsAlphaMode = DDS_ALPHA_MODE_UNKNOWN;
	bool blsCubeMap = false;
	D3D12_SUBRESOURCE_DATA subResourceData;
	HRESULT hResult;
	if (bDDS)
		LoadDDSTextureFromFileEx(pd3dDevice.Get(), pszFileName, 0, D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_DEFAULT, m_pd3dTexture.GetAddressOf(), decodedData, vSubresources, &ddsAlphaMode, &blsCubeMap);
	else 
		hResult = LoadWICTextureFromFileEx(pd3dDevice.Get(), pszFileName, 0, D3D12_RESOURCE_FLAG_NONE, WIC_LOADER_DEFAULT, m_pd3dTexture.GetAddressOf(), decodedData, subResourceData);

	D3D12_HEAP_PROPERTIES d3dHP;
	::ZeroMemory(&d3dHP, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHP.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHP.CreationNodeMask = 1;
	d3dHP.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHP.Type = D3D12_HEAP_TYPE_UPLOAD;
	d3dHP.VisibleNodeMask = 1;

	UINT64 nBytes;
	UINT nSubResources = (UINT)vSubresources.size();
	if (bDDS) {
		nBytes = GetRequiredIntermediateSize(m_pd3dTexture.Get(), 0, nSubResources);
	}
	else
		nBytes = GetRequiredIntermediateSize(m_pd3dTexture.Get(), 0, 1);

	D3D12_RESOURCE_DESC d3dRD;
	::ZeroMemory(&d3dRD, sizeof(D3D12_RESOURCE_DESC));
	d3dRD.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3dRD.Format = DXGI_FORMAT_UNKNOWN;
	d3dRD.MipLevels = 1;
	d3dRD.Alignment = 0;
	d3dRD.DepthOrArraySize = 1;
	d3dRD.Flags = D3D12_RESOURCE_FLAG_NONE;
	d3dRD.Height = 1;
	d3dRD.Width = nBytes;
	d3dRD.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dRD.SampleDesc.Count = 1;
	d3dRD.SampleDesc.Quality = 0;

	pd3dDevice->CreateCommittedResource(&d3dHP, D3D12_HEAP_FLAG_NONE, &d3dRD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)m_pd3dUploadBuffer.GetAddressOf());

	if (bDDS)
		::UpdateSubresources(pd3dCommandList.Get(), m_pd3dTexture.Get(), m_pd3dUploadBuffer.Get(), 0, 0, nSubResources, &vSubresources[0]);
	else
		::UpdateSubresources(pd3dCommandList.Get(), m_pd3dTexture.Get(), m_pd3dUploadBuffer.Get(), 0, 0, 1, &subResourceData);

	D3D12_RESOURCE_BARRIER d3dRB;
	::ZeroMemory(&d3dRB, sizeof(D3D12_RESOURCE_BARRIER));
	d3dRB.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dRB.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dRB.Transition.pResource = m_pd3dTexture.Get();
	d3dRB.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	d3dRB.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	d3dRB.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	pd3dCommandList->ResourceBarrier(1, &d3dRB);
}

const ComPtr<ID3D12Resource>& CSingleTexture::getTexture()
{
	return m_pd3dTexture;
}

void CSingleTexture::ReleaseUploadBuffer()
{
	m_pd3dUploadBuffer.Reset();
}

//==============================================================================

CGameObject::CGameObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());

	::CreateBufferResource(pd3dDevice, m_pd3dWorldBuffer, m_xmf4x4World);

	m_pd3dWorldBuffer->Map(0, NULL, (void**)&m_pMappedWorld);
	XMStoreFloat4x4(m_pMappedWorld, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	//*m_pMappedWorld = m_xmf4x4World;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Pos)
{
	m_xmf4x4World._41 = xmf3Pos.x;
	m_xmf4x4World._42 = xmf3Pos.y;
	m_xmf4x4World._43 = xmf3Pos.z;
}

void CGameObject::CreateDescriptorHeap(ComPtr<ID3D12Device>& pd3dDevice)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDHD;
	d3dDHD.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDHD.NodeMask = 0;
	d3dDHD.NumDescriptors = 2;
	d3dDHD.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	pd3dDevice->CreateDescriptorHeap(&d3dDHD, __uuidof(ID3D12DescriptorHeap), (void**)m_pd3dCbvSrvDescriptor.GetAddressOf());
}

void CGameObject::CreateResourceView(ComPtr<ID3D12Device>& pd3dDevice)
{
	CreateDescriptorHeap(pd3dDevice);
	UINT nIncreamentSize = pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE d3dCPUDescriptorHandle = m_pd3dCbvSrvDescriptor->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVD;
	d3dCBVD.BufferLocation = m_pd3dWorldBuffer->GetGPUVirtualAddress();
	d3dCBVD.SizeInBytes = (sizeof(m_xmf4x4World) + 255) & ~255;
	pd3dDevice->CreateConstantBufferView(&d3dCBVD, d3dCPUDescriptorHandle);
	d3dCPUDescriptorHandle.ptr += nIncreamentSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dSRVD;
	D3D12_RESOURCE_DESC d3dRD = m_pMaterial->getTexture()->GetDesc();
	d3dSRVD.Format = d3dRD.Format;
	d3dSRVD.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	d3dSRVD.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	d3dSRVD.Texture2D.MipLevels = -1;
	d3dSRVD.Texture2D.MostDetailedMip = 0;
	d3dSRVD.Texture2D.ResourceMinLODClamp = 0.0f;
	d3dSRVD.Texture2D.PlaneSlice = 0;


	pd3dDevice->CreateShaderResourceView(m_pMaterial->getTexture().Get(), &d3dSRVD, d3dCPUDescriptorHandle);
}

void CGameObject::SetShaderVariables(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	XMStoreFloat4x4(m_pMappedWorld, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	pd3dCommandList->SetGraphicsRootDescriptorTable(1, m_pd3dCbvSrvDescriptor->GetGPUDescriptorHandleForHeapStart());
}

void CGameObject::Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::shared_ptr<CShader>& currentSetShader)
{
	pd3dCommandList->SetDescriptorHeaps(1, m_pd3dCbvSrvDescriptor.GetAddressOf());
	SetShaderVariables(pd3dCommandList);
	if (m_pShader && m_pShader != currentSetShader) {
		m_pShader->SetPipelineState(pd3dCommandList);
		currentSetShader = m_pShader;
	}
	if (m_pMesh) {
		m_pMesh->Render(pd3dCommandList);
	}
}

//==========================================================================================

void CTerrainObject::CreateDescriptorHeap(ComPtr<ID3D12Device>& pd3dDevice)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDHD;
	d3dDHD.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDHD.NodeMask = 0;
	d3dDHD.NumDescriptors = 3;
	d3dDHD.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	pd3dDevice->CreateDescriptorHeap(&d3dDHD, __uuidof(ID3D12DescriptorHeap), (void**)m_pd3dCbvSrvDescriptor.GetAddressOf());
}

void CTerrainObject::CreateResourceView(ComPtr<ID3D12Device>& pd3dDevice)
{
	CreateDescriptorHeap(pd3dDevice);
	UINT nIncreamentSize = pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE d3dCPUDescriptorHandle = m_pd3dCbvSrvDescriptor->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVD;
	d3dCBVD.BufferLocation = m_pd3dWorldBuffer->GetGPUVirtualAddress();
	d3dCBVD.SizeInBytes = (sizeof(m_xmf4x4World) + 255) & ~255;
	pd3dDevice->CreateConstantBufferView(&d3dCBVD, d3dCPUDescriptorHandle);
	d3dCPUDescriptorHandle.ptr += nIncreamentSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dSRVD;
	D3D12_RESOURCE_DESC d3dRD = m_pMaterial->getTexture()->GetDesc();
	d3dSRVD.Format = d3dRD.Format;
	d3dSRVD.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	d3dSRVD.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	d3dSRVD.Texture2D.MipLevels = -1;
	d3dSRVD.Texture2D.MostDetailedMip = 0;
	d3dSRVD.Texture2D.ResourceMinLODClamp = 0.0f;
	d3dSRVD.Texture2D.PlaneSlice = 0;


	pd3dDevice->CreateShaderResourceView(m_pMaterial->getTexture().Get(), &d3dSRVD, d3dCPUDescriptorHandle);
	d3dCPUDescriptorHandle.ptr += nIncreamentSize;

	d3dRD = m_pMaterial2->getTexture()->GetDesc();
	d3dSRVD.Format = d3dRD.Format;

	pd3dDevice->CreateShaderResourceView(m_pMaterial2->getTexture().Get(), &d3dSRVD, d3dCPUDescriptorHandle);
	
}

void CTerrainObject::SetShaderVariables(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	XMStoreFloat4x4(m_pMappedWorld, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_pd3dCbvSrvDescriptor->GetGPUDescriptorHandleForHeapStart());
}

//=========================================================

CSkyBoxObject::CSkyBoxObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	m_vPlanes.reserve(6);
	m_vPlanes.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vPlanes.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vPlanes.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vPlanes.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vPlanes.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vPlanes.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));

	std::shared_ptr<CShader> pShader = std::make_shared<CSkyBoxShader>();
	pShader->CreatePipelineState(pd3dDevice, pd3dRootSignature);
	m_vPlanes[0]->SetShader(pShader);
	m_vPlanes[1]->SetShader(pShader);
	m_vPlanes[2]->SetShader(pShader);
	m_vPlanes[3]->SetShader(pShader);
	m_vPlanes[4]->SetShader(pShader);
	m_vPlanes[5]->SetShader(pShader);

	std::shared_ptr<CMesh> pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 좌);
	std::shared_ptr<CSingleTexture> pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\skybox\\SkyBox_Left_0.dds", true);
	m_vPlanes[0]->SetMaterial(pMaterial); m_vPlanes[0]->SetMesh(pMesh);

	pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 우);
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\skybox\\SkyBox_Right_0.dds", true);
	m_vPlanes[1]->SetMaterial(pMaterial); m_vPlanes[1]->SetMesh(pMesh);

	pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 앞);
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\skybox\\SkyBox_Front_0.dds", true);
	m_vPlanes[2]->SetMaterial(pMaterial); m_vPlanes[2]->SetMesh(pMesh);

	pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 뒤);
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\skybox\\SkyBox_Back_0.dds", true);
	m_vPlanes[3]->SetMaterial(pMaterial); m_vPlanes[3]->SetMesh(pMesh);

	pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 상);
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\skybox\\SkyBox_Top_0.dds", true);
	m_vPlanes[4]->SetMaterial(pMaterial); m_vPlanes[4]->SetMesh(pMesh);

	pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 하);
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\skybox\\SkyBox_Bottom_0.dds", true);
	m_vPlanes[5]->SetMaterial(pMaterial); m_vPlanes[5]->SetMesh(pMesh);

	for (int i = 0; i < 6; ++i) {
		m_vPlanes[i]->CreateResourceView(pd3dDevice);
	}
}

void CSkyBoxObject::UpdatePosition(XMFLOAT3 pos) {
	for (int i = 0; i < 6; ++i) {
		m_vPlanes[i]->SetPosition(pos);
	}
}

void CSkyBoxObject::Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::shared_ptr<CShader>& currentSetShader)
{
	for (int i = 0; i < 6; ++i)
		m_vPlanes[i]->Render(pd3dCommandList, currentSetShader);
}