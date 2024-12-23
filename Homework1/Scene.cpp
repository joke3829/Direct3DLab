//========================================
// Scene.cpp
// 장면들을 정의한 cpp
//========================================
#include "Scene.h"

DirLight::DirLight(ComPtr<ID3D12Device>& pd3dDevice)
{
	LightInfo temp;
	CreateBufferResource(pd3dDevice, m_pd3dMappedLight, temp);
	m_pd3dMappedLight->Map(0, NULL, (void**)&m_pLightInfo);
	m_pLightInfo->xmf4Diffused = XMFLOAT4(1.0, 1.0, 1.0, 1.0);
	m_pLightInfo->xmf4Specular = XMFLOAT4(1.0, 1.0, 1.0, 1.0);
	m_pLightInfo->xmf4Ambient = XMFLOAT4(1.0, 1.0, 1.0, 1.0);
	XMFLOAT3 xmf(-1.0, 1.0, -1.0);
	XMStoreFloat3(&m_pLightInfo->xmf3Dir, XMVector3Normalize(XMLoadFloat3(&xmf)));
}

void DirLight::PrepareRender(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, m_pd3dMappedLight->GetGPUVirtualAddress());
}

void CScene::CreateRootSignature(ComPtr<ID3D12Device>& pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE d3dDescriptorRange[2];
	d3dDescriptorRange[0].NumDescriptors = 1;
	d3dDescriptorRange[0].BaseShaderRegister = 1;
	d3dDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRange[0].RegisterSpace = 0;
	d3dDescriptorRange[0].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[1].NumDescriptors = 1;
	d3dDescriptorRange[1].BaseShaderRegister = 0;
	d3dDescriptorRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[1].RegisterSpace = 0;
	d3dDescriptorRange[1].OffsetInDescriptorsFromTableStart = 1;

	D3D12_DESCRIPTOR_RANGE d3dShadowMapRange;
	d3dShadowMapRange.BaseShaderRegister = 3;
	d3dShadowMapRange.NumDescriptors = 1;
	d3dShadowMapRange.OffsetInDescriptorsFromTableStart = 0;
	d3dShadowMapRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dShadowMapRange.RegisterSpace = 0;

	D3D12_DESCRIPTOR_RANGE d3dTerrainRange[2];
	d3dTerrainRange[0].NumDescriptors = 1;
	d3dTerrainRange[0].BaseShaderRegister = 4;
	d3dTerrainRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dTerrainRange[0].RegisterSpace = 0;
	d3dTerrainRange[0].OffsetInDescriptorsFromTableStart = 0;

	d3dTerrainRange[1].NumDescriptors = 2;
	d3dTerrainRange[1].BaseShaderRegister = 1;
	d3dTerrainRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dTerrainRange[1].RegisterSpace = 0;
	d3dTerrainRange[1].OffsetInDescriptorsFromTableStart = 1;

	// 0. 카메라 정보, 1. 월드, 텍스처 테이블, 2. 지형, 3. 방향성 조명,
	// 4. 경과시간, 5. 반사 행렬, 6 그림자 맵, 7 그림자 맵 카메라
	D3D12_ROOT_PARAMETER d3dRootParameter[8];
	d3dRootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	d3dRootParameter[0].Descriptor.RegisterSpace = 0;
	d3dRootParameter[0].Descriptor.ShaderRegister = 0;
	d3dRootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	d3dRootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	d3dRootParameter[1].DescriptorTable.NumDescriptorRanges = 2;
	d3dRootParameter[1].DescriptorTable.pDescriptorRanges = d3dDescriptorRange;
	d3dRootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	d3dRootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	d3dRootParameter[2].DescriptorTable.NumDescriptorRanges = 2;
	d3dRootParameter[2].DescriptorTable.pDescriptorRanges = d3dTerrainRange;
	d3dRootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	d3dRootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	d3dRootParameter[3].Descriptor.RegisterSpace = 0;
	d3dRootParameter[3].Descriptor.ShaderRegister = 2;
	d3dRootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	d3dRootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	d3dRootParameter[4].Descriptor.RegisterSpace = 0;
	d3dRootParameter[4].Descriptor.ShaderRegister = 5;
	d3dRootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	d3dRootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	d3dRootParameter[5].Descriptor.RegisterSpace = 0;
	d3dRootParameter[5].Descriptor.ShaderRegister = 6;
	d3dRootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	d3dRootParameter[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	d3dRootParameter[6].DescriptorTable.NumDescriptorRanges = 1;
	d3dRootParameter[6].DescriptorTable.pDescriptorRanges = &d3dShadowMapRange;
	d3dRootParameter[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	d3dRootParameter[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	d3dRootParameter[7].Descriptor.RegisterSpace = 0;
	d3dRootParameter[7].Descriptor.ShaderRegister = 7;
	d3dRootParameter[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	
	D3D12_STATIC_SAMPLER_DESC d3dStaticSamplerDesc[2];
	::ZeroMemory(&d3dStaticSamplerDesc[0], sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dStaticSamplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dStaticSamplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dStaticSamplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dStaticSamplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dStaticSamplerDesc[0].MipLODBias = 0.0f;
	d3dStaticSamplerDesc[0].MaxAnisotropy = 1;
	d3dStaticSamplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dStaticSamplerDesc[0].MinLOD = -FLT_MAX;
	d3dStaticSamplerDesc[0].MaxLOD = FLT_MAX;
	d3dStaticSamplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	d3dStaticSamplerDesc[0].RegisterSpace = 0;
	d3dStaticSamplerDesc[0].ShaderRegister = 0;
	d3dStaticSamplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	::ZeroMemory(&d3dStaticSamplerDesc[1], sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dStaticSamplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	d3dStaticSamplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dStaticSamplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dStaticSamplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	d3dStaticSamplerDesc[1].MipLODBias = 0.0f;
	d3dStaticSamplerDesc[1].MaxAnisotropy = 1;
	d3dStaticSamplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	d3dStaticSamplerDesc[1].MinLOD = -FLT_MAX;
	d3dStaticSamplerDesc[1].MaxLOD = FLT_MAX;
	d3dStaticSamplerDesc[1].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	d3dStaticSamplerDesc[1].RegisterSpace = 0;
	d3dStaticSamplerDesc[1].ShaderRegister = 1;
	d3dStaticSamplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
	d3dRootSignatureDesc.NumParameters = 8;
	d3dRootSignatureDesc.NumStaticSamplers = 2;
	d3dRootSignatureDesc.pParameters = d3dRootParameter;
	d3dRootSignatureDesc.pStaticSamplers = d3dStaticSamplerDesc;

	ID3DBlob* pd3dBlob{ nullptr };
	ID3DBlob* pd3dErrorBlob{ nullptr };
	HRESULT hh = ::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pd3dBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dBlob->GetBufferPointer(), pd3dBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)m_pd3dRootSignature.GetAddressOf());

	if (pd3dBlob)
		pd3dBlob->Release();
	if (pd3dErrorBlob)
		pd3dErrorBlob->Release();
}

void CScene::SetCamera(std::shared_ptr<CCamera>& pCamera)
{
	m_pCamera = pCamera;
}

void CScene::SetThirdPersonCamera(bool bThirdPerson)
{
	m_pCamera->SetThirdPerson(bThirdPerson);
}

//========================================================================

void CMenuScene::BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	CreateRootSignature(pd3dDevice);
	m_vObjects.reserve(2);
	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));	// 바탕
	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));	// 설명창

	std::shared_ptr<CShader> pShader = std::make_shared<CMenuShader>();
	pShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
	std::shared_ptr<CMesh> pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 1280, 720);
	std::shared_ptr<CSingleTexture> pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\main.jpg", false);

	m_vObjects[0]->SetMesh(pMesh); m_vObjects[0]->SetShader(pShader); m_vObjects[0]->SetMaterial(pMaterial);
	pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 960, 540);
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\main2.jpg", false);
	m_vObjects[1]->SetMesh(pMesh); m_vObjects[1]->SetShader(pShader); m_vObjects[1]->SetMaterial(pMaterial);

	m_vObjects[1]->SetPosition(XMFLOAT3(0.0, 0.0, -0.5));
	for (int i = 0; i < 2; ++i) {
		m_vObjects[i]->CreateResourceView(pd3dDevice);
	}

}

void CMenuScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	switch (nMessage) {
	case WM_KEYDOWN:
		switch (wParam) {
		case 'n':
		case 'N':
			if (m_bOnExplain) m_bOnExplain = false;
			else m_bOnExplain = true;
			break;
		}
		break;
	case WM_KEYUP:
		break;
	}
}

void CMenuScene::Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	m_pCurrentSetShader.reset();
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature.Get());
	m_pCamera->SetViewportAndScissorRect(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);

	m_vObjects[0]->Render(pd3dCommandList, m_pCurrentSetShader);
	if (m_bOnExplain)
		m_vObjects[1]->Render(pd3dCommandList, m_pCurrentSetShader);
}

// =====================================================================

void CIngameScene::BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	CreateRootSignature(pd3dDevice);
	FrameInfo tempData;
	::CreateBufferResource(pd3dDevice, m_pd3dElapsed, tempData);
	m_pd3dElapsed->Map(0, NULL, (void**)&m_pMappedInfo);

	// 그림자 맵을 위한 리소스와 SRV 생성========================================
	::CreateTexture2DResource(pd3dDevice, m_pd3dShadowMap, 1024, 1024, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_GENERIC_READ);

	D3D12_DESCRIPTOR_HEAP_DESC d3dDHD;
	d3dDHD.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDHD.NodeMask = 0;
	d3dDHD.NumDescriptors = 1;
	d3dDHD.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	pd3dDevice->CreateDescriptorHeap(&d3dDHD, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dSMDescriptorHeap);

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dSRVD;
	D3D12_RESOURCE_DESC d3dRD = m_pd3dShadowMap->GetDesc();
	d3dSRVD.Format = d3dRD.Format;
	d3dSRVD.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	d3dSRVD.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	d3dSRVD.Texture2D.MipLevels = 1;
	d3dSRVD.Texture2D.MostDetailedMip = 0;
	d3dSRVD.Texture2D.PlaneSlice = 0;
	d3dSRVD.Texture2D.ResourceMinLODClamp = 0.0f;
	pd3dDevice->CreateShaderResourceView(m_pd3dShadowMap.Get(), &d3dSRVD, m_pd3dSMDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// RTV 생성
	d3dDHD.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDHD.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	HRESULT hResult = pd3dDevice->CreateDescriptorHeap(&d3dDHD, __uuidof(ID3D12DescriptorHeap), (void**)m_pd3dSMRTVDescriptorHeap.GetAddressOf());
	D3D12_RENDER_TARGET_VIEW_DESC d3dRTVDesc;
	d3dRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	d3dRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dRTVDesc.Texture2D.MipSlice = 0;
	d3dRTVDesc.Texture2D.PlaneSlice = 0;

	pd3dDevice->CreateRenderTargetView(m_pd3dShadowMap.Get(), NULL, m_pd3dSMRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// DSV
	::CreateTexture2DResource(pd3dDevice, m_pShadowDSVBuffer, 1024, 1024, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	d3dDHD.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = pd3dDevice->CreateDescriptorHeap(&d3dDHD, __uuidof(ID3D12DescriptorHeap), (void**)m_pd3dShadowDSVDescriptorHeap.GetAddressOf());

	pd3dDevice->CreateDepthStencilView(m_pShadowDSVBuffer.Get(), NULL, m_pd3dShadowDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	m_vDepthShaders.push_back(std::make_unique<CTexturedDepthShader>());
	m_vDepthShaders.push_back(std::make_unique<CTerrainDepthShader>());
	m_vDepthShaders.push_back(std::make_unique<CPlayerDepthShader>());

	for (int i = 0; i < 3; ++i)
		m_vDepthShaders[i]->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);

	m_pSubCamera = std::make_unique<CCamera>(pd3dDevice);
	//m_pSubCamera->SetCameraEye(XMFLOAT3(-100.0, 400.0, -100.0));
	m_pSubCamera->SetCameraEye(XMFLOAT3(257.0, 400.0, 0.0));
	//m_pSubCamera->SetDir(XMFLOAT3(1.0, -1.0, 1.0));
	m_pSubCamera->SetDir(XMFLOAT3(0.0, -1.0, 1.0));
	m_pSubCamera->SetOrthoMatrix(700.0, 700.0, 1.0, 1000.0);
	m_pSubCamera->UpdateViewPort();

	//===========================================================================

	// 조명
	m_pLight = std::make_unique<DirLight>(pd3dDevice);

	// 0 지형
	m_vObjects.push_back(std::make_unique<CTerrainObject>(pd3dDevice, pd3dCommandList));

	std::unique_ptr<CHeightMapImage> heightMap = std::make_unique<CHeightMapImage>(L"texture\\terrain\\HeightMap.raw", 257, 257, XMFLOAT3(2.0f, 1.0f, 2.0f));
	std::shared_ptr<CMesh> pMesh = std::make_shared<CHeightMapGridMesh>(pd3dDevice, pd3dCommandList, heightMap);
	std::shared_ptr<CSingleTexture> pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\terrain\\Base_Texture.jpg", false);
	std::shared_ptr<CShader> pShader = std::make_shared<CTerrainShader>();
	pShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);

	m_vObjects[0]->SetMesh(pMesh);
	m_vObjects[0]->SetMaterial(pMaterial);
	m_vObjects[0]->SetShader(pShader);

	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\terrain\\Detail_Texture_7.dds", true);
	dynamic_cast<CTerrainObject*>(m_vObjects[0].get())->SetMaterial2(pMaterial);

	// 추가할거 더 추가

	pMesh = std::make_shared<CTexturedCubeMesh>(pd3dDevice, pd3dCommandList, XMFLOAT3(20.0, 20.0, 20.0));
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Stone.dds", true);
	pShader = std::make_shared<CTexturedShader>(); pShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vObjects[1]->SetMesh(pMesh); m_vObjects[1]->SetMaterial(pMaterial); m_vObjects[1]->SetPosition(XMFLOAT3(128.5, 200.0, 385.5)); m_vObjects[1]->SetShader(pShader);

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vObjects[2]->SetMesh(pMesh); m_vObjects[2]->SetMaterial(pMaterial); m_vObjects[2]->SetPosition(XMFLOAT3(192.75, 200.0, 257.0)); m_vObjects[2]->SetShader(pShader);

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vObjects[3]->SetMesh(pMesh); m_vObjects[3]->SetMaterial(pMaterial); m_vObjects[3]->SetPosition(XMFLOAT3(385.5, 200.0, 128.5)); m_vObjects[3]->SetShader(pShader);

	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Ceiling.dds", true);
	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vObjects[4]->SetMesh(pMesh); m_vObjects[4]->SetMaterial(pMaterial); m_vObjects[4]->SetPosition(XMFLOAT3(257.0, 200.0, 385.5)); m_vObjects[4]->SetShader(pShader);

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vObjects[5]->SetMesh(pMesh); m_vObjects[5]->SetMaterial(pMaterial); m_vObjects[5]->SetPosition(XMFLOAT3(321.25, 200.0, 257.0)); m_vObjects[5]->SetShader(pShader);

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vObjects[6]->SetMesh(pMesh); m_vObjects[6]->SetMaterial(pMaterial); m_vObjects[6]->SetPosition(XMFLOAT3(128.5, 200.0, 128.5)); m_vObjects[6]->SetShader(pShader);

	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Lava(Diffuse).dds", true);
	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vObjects[7]->SetMesh(pMesh); m_vObjects[7]->SetMaterial(pMaterial); m_vObjects[7]->SetPosition(XMFLOAT3(385.5, 200.0, 385.5)); m_vObjects[7]->SetShader(pShader);

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vObjects[8]->SetMesh(pMesh); m_vObjects[8]->SetMaterial(pMaterial); m_vObjects[8]->SetPosition(XMFLOAT3(449.75, 200.0, 257.0)); m_vObjects[8]->SetShader(pShader);

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vObjects[9]->SetMesh(pMesh); m_vObjects[9]->SetMaterial(pMaterial); m_vObjects[9]->SetPosition(XMFLOAT3(257.5, 200.0, 128.5)); m_vObjects[9]->SetShader(pShader);

	// 총알
	pMesh = std::make_shared<CTexturedCubeMesh>(pd3dDevice, pd3dCommandList, XMFLOAT3(1.0, 1.0, 1.0));
	m_vObjects.push_back(std::make_unique<BulletObject>(pd3dDevice, pd3dCommandList));
	m_vObjects[10]->SetMesh(pMesh); m_vObjects[10]->SetMaterial(pMaterial); m_vObjects[10]->SetShader(pShader);

	// 빌보드 텍스쳐 11
	pMesh = std::make_shared<CBillboardMesh>(pd3dDevice, pd3dCommandList, XMFLOAT3(0.0, 0.0, 0.0), XMFLOAT2(16.0, 19.2));
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Tree02.png", false);
	pShader = std::make_shared<CBillboardShader>(); pShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
	int index = m_vObjects.size();
	for (int i = 0; i < 171; i += 20) {
		for (int j = 0; j < 257; j += 10) {
			if (heightMap->GetHeight(i / 2, j / 2) > 100.0) {
				m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
				m_vObjects[index]->SetMesh(pMesh); m_vObjects[index]->SetShader(pShader); m_vObjects[index]->SetMaterial(pMaterial); m_vObjects[index]->SetPosition(XMFLOAT3(i, heightMap->GetHeight(i/2, j/2) + 9.6, j));
				++index;
			}
		}
	}
	pMesh = std::make_shared<CBillboardMesh>(pd3dDevice, pd3dCommandList, XMFLOAT3(0.0, 0.0, 0.0), XMFLOAT2(6.0, 6.0));
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Grass01.png", false);

	for (int i = 182; i < 342; i += 6) {
		for (int j = 0; j < 257; j += 6) {
			if (heightMap->GetHeight(i / 2, j / 2) > 90.0) {
				m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
				m_vObjects[index]->SetMesh(pMesh); m_vObjects[index]->SetShader(pShader); m_vObjects[index]->SetMaterial(pMaterial); m_vObjects[index]->SetPosition(XMFLOAT3(i, heightMap->GetHeight(i / 2, j / 2) + 3.0, j));
				++index;
			}
		}
	}

	pMesh = std::make_shared<CBillboardMesh>(pd3dDevice, pd3dCommandList, XMFLOAT3(0.0, 0.0, 0.0), XMFLOAT2(8.0, 8.0));
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Flower01.png", false);

	for (int i = 342; i < 514; i += 6) {
		for (int j = 0; j < 257; j += 6) {
			if (heightMap->GetHeight(i / 2, j / 2) > 90.0) {
				m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
				m_vObjects[index]->SetMesh(pMesh); m_vObjects[index]->SetShader(pShader); m_vObjects[index]->SetMaterial(pMaterial); m_vObjects[index]->SetPosition(XMFLOAT3(i, heightMap->GetHeight(i / 2, j / 2) + 4.0, j));
				++index;
			}
		}
	}

	pMesh = std::make_shared<CBillboardMesh>(pd3dDevice, pd3dCommandList, XMFLOAT3(0.0, 0.0, 0.0), XMFLOAT2(8.0, 8.0));
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Flower02.png", false);

	for (int i = 0; i < 128; i += 10) {
		for (int j = 258; j < 514; j += 10) {
			if (heightMap->GetHeight(i / 2, j / 2) > 100.0) {
				m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
				m_vObjects[index]->SetMesh(pMesh); m_vObjects[index]->SetShader(pShader); m_vObjects[index]->SetMaterial(pMaterial); m_vObjects[index]->SetPosition(XMFLOAT3(i, heightMap->GetHeight(i / 2, j / 2) + 4.0, j));
				++index;
			}
		}
	}

	pMesh = std::make_shared<CBillboardMesh>(pd3dDevice, pd3dCommandList, XMFLOAT3(0.0, 0.0, 0.0), XMFLOAT2(24.0, 24.0));
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Tree01.png", false);

	for (int i = 132; i < 300; i += 10) {
		for (int j = 258; j < 400; j += 10) {
			if (heightMap->GetHeight(i / 2, j / 2) > 100.0) {
				m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
				m_vObjects[index]->SetMesh(pMesh); m_vObjects[index]->SetShader(pShader); m_vObjects[index]->SetMaterial(pMaterial); m_vObjects[index]->SetPosition(XMFLOAT3(i, heightMap->GetHeight(i / 2, j / 2) + 12.0, j));
				++index;
			}
		}
	}

	pMesh = std::make_shared<CBillboardMesh>(pd3dDevice, pd3dCommandList, XMFLOAT3(0.0, 0.0, 0.0), XMFLOAT2(8.0, 20.8));
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Tree03.png", false);

	for (int i = 400; i < 514; i += 10) {
		for (int j = 300; j < 400; j += 10) {
			if (heightMap->GetHeight(i / 2, j / 2) > 100.0) {
				m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
				m_vObjects[index]->SetMesh(pMesh); m_vObjects[index]->SetShader(pShader); m_vObjects[index]->SetMaterial(pMaterial); m_vObjects[index]->SetPosition(XMFLOAT3(i, heightMap->GetHeight(i / 2, j / 2) + 10.4, j));
				++index;
			}
		}
	}

	m_pSmog = std::make_unique<CSmogObject>(pd3dDevice, pd3dCommandList);
	m_pSmog1 = std::make_unique<CSmogObject>(pd3dDevice, pd3dCommandList);
	pMesh = std::make_shared<CParticleMesh>(pd3dDevice, pd3dCommandList);
	m_pSmog->SetMesh(pMesh); m_pSmog1->SetMesh(std::make_shared<CParticleMesh>(pd3dDevice, pd3dCommandList));
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Fire1.dds", true);
	m_pSmog->SetMaterial(pMaterial); m_pSmog1->SetMaterial(pMaterial);
	pShader = std::make_shared<CParticleSOShader>(); pShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
	m_pSmog->SetShader(pShader); m_pSmog1->SetShader(pShader);
	pShader = std::make_shared<CParticleDrawShader>(); pShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
	m_pSmog->SetDrawShader(pShader); m_pSmog1->SetDrawShader(pShader);
	m_pSmog->CreateResourceView(pd3dDevice); m_pSmog1->CreateResourceView(pd3dDevice);
	m_pSmog->SetPosition(XMFLOAT3(200, heightMap->GetHeight(100, 150), 300));
	m_pSmog1->SetPosition(XMFLOAT3(380, heightMap->GetHeight(160, 30), 60));

	//================================================================================

	m_pWater = std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList);
	pShader = std::make_shared<CWaterShader>(); pShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
	pMesh = std::make_shared<CPlaneMesh>(pd3dDevice, pd3dCommandList, 514.0, 514.0);
	m_pWater->SetMesh(pMesh); m_pWater->SetMaterial(pMaterial); m_pWater->SetShader(pShader); m_pWater->SetPosition(XMFLOAT3(257.0, 80.0, 257.0));

	std::ifstream inFile{ "model\\Mi24.bin", std::ios::binary };
	std::unique_ptr<HGameObject> nullp{ nullptr };
	m_pPlayer = std::make_unique<HGameObject>(pd3dDevice, pd3dCommandList, inFile, nullp);
	pShader = std::make_shared<CPlayerShader>();
	pShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
	m_pPlayer->setShader(pShader);
	m_pPlayer->SetPosition(XMFLOAT3(257.0, 200.0, 257.0));
	for (int i = 0; i < 3; ++i) {
		inFile.seekg(std::ios::beg);
		m_vOpposite.push_back(std::make_unique<HGameObject>(pd3dDevice, pd3dCommandList, inFile, nullp));
		m_vOpposite[i]->SetShader(pShader);
		m_vOpposite[i]->CreateResourceView(pd3dDevice);
	}

	m_vOpposite[0]->SetPosition(XMFLOAT3(257.5, 150.0, 128.5));
	m_vOpposite[1]->SetPosition(XMFLOAT3(321.25, 150.0, 257.0));
	m_vOpposite[2]->SetPosition(XMFLOAT3(192.75, 150.0, 257.0));

	m_pCamera->SetTarget(m_pPlayer.get());
	dynamic_cast<BulletObject*>(m_vObjects[10].get())->SetPlayer(m_pPlayer.get());
	for (std::unique_ptr<CGameObject>& temp : m_vObjects) {
		temp->CreateResourceView(pd3dDevice);
	}
	m_pPlayer->CreateResourceView(pd3dDevice);

	m_pWater->CreateResourceView(pd3dDevice);

	m_pSkyBox = std::make_unique<CSkyBoxObject>(pd3dDevice, pd3dCommandList, m_pd3dRootSignature);
}

void CIngameScene::ProcessInput(float fElapsedTime)
{
	UCHAR keyBuffer[256];
	GetKeyboardState(keyBuffer);

	if(!autoPilot){
		if (keyBuffer['W'] & 0x80)
			m_pPlayer->move(DIR_FORWARD, fElapsedTime);
		if (keyBuffer['A'] & 0x80)
			m_pPlayer->move(DIR_LEFT, fElapsedTime);
		if (keyBuffer['S'] & 0x80)
			m_pPlayer->move(DIR_BACK, fElapsedTime);
		if (keyBuffer['D'] & 0x80)
			m_pPlayer->move(DIR_RIGHT, fElapsedTime);
		if (keyBuffer[VK_PRIOR] & 0x80)
			m_pPlayer->move(DIR_UP, fElapsedTime);
		if (keyBuffer[VK_NEXT] & 0x80)
			m_pPlayer->move(DIR_DOWN, fElapsedTime);
		if (keyBuffer[VK_SPACE] & 0x80)
			dynamic_cast<BulletObject*>(m_vObjects[10].get())->Shoot();
	}
}

void CIngameScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	switch (nMessage) {
	case WM_KEYDOWN:
		switch (wParam) {
		case 'p':
		case 'P':
			if (autoPilot)
				autoPilot = false;
			else
				autoPilot = true;
			break;
		case 'o':
		case 'O':
			m_bShowShadowMap = !m_bShowShadowMap;
			break;
		}
		break;
	case WM_KEYUP:
		break;
	}
}

void CIngameScene::Animate(float fElapsedTime)
{
	m_pMappedInfo->m_fElapsedTime = fElapsedTime;
	m_pMappedInfo->m_bShowShadowMap = m_bShowShadowMap;
	if (autoPilot) {
		m_pPlayer->AutoPilot(m_vObjects, fElapsedTime);
	}
	if (dynamic_cast<BulletObject*>(m_vObjects[10].get())->getExist()) {
		dynamic_cast<BulletObject*>(m_vObjects[10].get())->Animate(fElapsedTime);
		m_vObjects[10]->UpdateOBB();
	}
	XMFLOAT4X4 temp{};
	for (int i = 0; i < 3; ++i) {
		if (m_bAlive[i]) {
			if (dynamic_cast<BulletObject*>(m_vObjects[10].get())->getExist()) {
				if (m_vOpposite[i]->collisionCheck(m_vObjects[10]->getOBB(), temp)) {
					m_bAlive[i] = false;
					m_fResurrection[i] = 0.0f;
					dynamic_cast<BulletObject*>(m_vObjects[10].get())->setExist(false);
				}
			}
		}
		else {
			m_fResurrection[i] += fElapsedTime;
			if (m_fResurrection[i] >= 3.0f)
				m_bAlive[i] = true;
		}
	}
}

void CIngameScene::PreRender(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	D3D12_RESOURCE_BARRIER d3dRD;
	::ZeroMemory(&d3dRD, sizeof(D3D12_RESOURCE_BARRIER));
	d3dRD.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dRD.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dRD.Transition.pResource = m_pd3dShadowMap.Get();
	d3dRD.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	d3dRD.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dRD.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	pd3dCommandList->ResourceBarrier(1, &d3dRD);
	// ShadowMap Render===================================
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRTVHandle = m_pd3dSMRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDSVHandle = m_pd3dShadowDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	float color[4] = { 1.0, 1.0, 1.0, 1.0 };
	pd3dCommandList->ClearRenderTargetView(d3dRTVHandle, color, 0, NULL);
	pd3dCommandList->ClearDepthStencilView(d3dDSVHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	pd3dCommandList->OMSetRenderTargets(1, &d3dRTVHandle, TRUE, &d3dDSVHandle);
	// 루트 시그니처
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature.Get());
	// 카메라
	m_pSubCamera->SetViewportAndScissorRect(pd3dCommandList);
	m_pSubCamera->UpdateShaderVariables(pd3dCommandList, 7);
	// 렌더링
	m_pCurrentSetShader.reset();
	// 지형
	m_vDepthShaders[1]->SetPipelineState(pd3dCommandList);
	m_vObjects[0]->Render(pd3dCommandList, m_pCurrentSetShader, false);
	// 박스
	m_vDepthShaders[0]->SetPipelineState(pd3dCommandList);
	for (int i = 1; i < 10; ++i)
		m_vObjects[i]->Render(pd3dCommandList, m_pCurrentSetShader, false);
	// 플레이어
	m_vDepthShaders[2]->SetPipelineState(pd3dCommandList);
	for (int i = 0; i < 3; ++i)
		m_vOpposite[i]->Render(pd3dCommandList, m_pCurrentSetShader, false);
	m_pPlayer->Render(pd3dCommandList, m_pCurrentSetShader, false);

	// ====================================================


	d3dRD.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dRD.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;

	pd3dCommandList->ResourceBarrier(1, &d3dRD);
}

void CIngameScene::Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	m_pCurrentSetShader.reset();
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature.Get());
	m_pCamera->SetViewportAndScissorRect(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->SetGraphicsRootConstantBufferView(4, m_pd3dElapsed->GetGPUVirtualAddress());	// 경과 시간 보내기
	
	// shadowMap Set
	pd3dCommandList->SetDescriptorHeaps(1, m_pd3dSMDescriptorHeap.GetAddressOf());
	pd3dCommandList->SetGraphicsRootDescriptorTable(6, m_pd3dSMDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	m_pLight->PrepareRender(pd3dCommandList);

	m_pSkyBox->UpdatePosition(m_pCamera->getCameraEye());
	m_pSkyBox->Render(pd3dCommandList, m_pCurrentSetShader);

	m_pPlayer->Render(pd3dCommandList, m_pCurrentSetShader);

	for (int i = 0; i < 3; ++i) {
		if(m_bAlive[i])
			m_vOpposite[i]->Render(pd3dCommandList, m_pCurrentSetShader);
	}

	for (std::unique_ptr<CGameObject>& temp : m_vObjects) {
		temp->Render(pd3dCommandList, m_pCurrentSetShader);
	}

	m_pSmog->Render(pd3dCommandList, m_pCurrentSetShader);
	m_pSmog1->Render(pd3dCommandList, m_pCurrentSetShader);

	m_pWater->Render(pd3dCommandList, m_pCurrentSetShader);
}

void CIngameScene::PostRender(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	m_pSmog->PostRender(pd3dCommandList);
	m_pSmog1->PostRender(pd3dCommandList);
}

//=================================================

void CRoomScene::BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	CreateRootSignature(pd3dDevice);
	XMFLOAT4X4 temp;
	::CreateBufferResource(pd3dDevice, m_pd3dReflectBuffer, temp);
	m_pd3dReflectBuffer->Map(0, NULL, (void**)&m_pMappedMatrix);

	std::shared_ptr<CMesh> pMesh;
	std::shared_ptr<CSingleTexture> pMaterial;
	// 0~5 벽
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Stone.dds", true);
	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 앞, 180.0f);
	m_vObjects[0]->SetMesh(pMesh); m_vObjects[0]->SetMaterial(pMaterial);

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 뒤, 180.0f);
	m_vObjects[1]->SetMesh(pMesh); m_vObjects[1]->SetMaterial(pMaterial);

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 상, 180.0f);
	m_vObjects[2]->SetMesh(pMesh); m_vObjects[2]->SetMaterial(pMaterial);

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 하, 180.0f);
	m_vObjects[3]->SetMesh(pMesh); m_vObjects[3]->SetMaterial(pMaterial);

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 좌, 180.0f);
	m_vObjects[4]->SetMesh(pMesh); m_vObjects[4]->SetMaterial(pMaterial);

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 우, 180.0f);
	m_vObjects[5]->SetMesh(pMesh); m_vObjects[5]->SetMaterial(pMaterial);

	// 6~8 객체

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	pMesh = std::make_shared<CTexturedNoLightingCubeMesh>(pd3dDevice, pd3dCommandList, XMFLOAT3(15.0, 50.0, 15.0));
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Ceiling.dds", true);
	m_vObjects[6]->SetMesh(pMesh); m_vObjects[6]->SetMaterial(pMaterial); m_vObjects[6]->SetPosition(XMFLOAT3(0.0, 20.0, 60.0));

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	pMesh = std::make_shared<CTexturedNoLightingCubeMesh>(pd3dDevice, pd3dCommandList, XMFLOAT3(30.0, 30.0, 30.0));
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Lava(Diffuse).dds", true);
	m_vObjects[7]->SetMesh(pMesh); m_vObjects[7]->SetMaterial(pMaterial); m_vObjects[7]->SetPosition(XMFLOAT3(50.0, -80.0, 100.0));

	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	pMesh = std::make_shared<CTexturedNoLightingCubeMesh>(pd3dDevice, pd3dCommandList, XMFLOAT3(13.0, 13.0, 60.0));
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\skybox\\SkyBox_Back_0.dds", true);
	m_vObjects[8]->SetMesh(pMesh); m_vObjects[8]->SetMaterial(pMaterial); m_vObjects[8]->SetPosition(XMFLOAT3(-70.0, -10.0, -20.0));

	// 9 거울
	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	pMesh = std::make_shared<CTexturedSquareMesh>(pd3dDevice, pd3dCommandList, 320.0f, 320.0f);
	m_vObjects[9]->SetMesh(pMesh); m_vObjects[9]->SetMaterial(pMaterial); m_vObjects[9]->SetPosition(XMFLOAT3(0.0f, 0.0f, 179.0f));

	for (std::unique_ptr<CGameObject>& temp : m_vObjects) {
		temp->CreateResourceView(pd3dDevice);
	}

	// shaders
	m_pStandardShader = std::make_unique<CMenuShader>(); m_pStandardShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
	m_pMirrorInitShader = std::make_unique<CMirrorInitializeShader>(); m_pMirrorInitShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
	m_pDepthInitShader = std::make_unique<CMirrorDepthInitShader>(); m_pDepthInitShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
	m_pReflectShader = std::make_unique<CReflectShader>(); m_pReflectShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
	m_pBlendMirrorShader = std::make_unique<CBlendMirrorShader>(); m_pBlendMirrorShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
}

void CRoomScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{

}

void CRoomScene::ProcessInput(float fElapsedTime)
{
	UCHAR keyBuffer[256];
	GetKeyboardState(keyBuffer);

	if (keyBuffer['W'] & 0x80)
		m_pCamera->move(앞, fElapsedTime);
	if (keyBuffer['A'] & 0x80)
		m_pCamera->move(좌, fElapsedTime);
	if (keyBuffer['S'] & 0x80)
		m_pCamera->move(뒤, fElapsedTime);
	if (keyBuffer['D'] & 0x80)
		m_pCamera->move(우, fElapsedTime);
}
void CRoomScene::Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	m_pCurrentSetShader.reset();
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature.Get());
	m_pCamera->SetViewportAndScissorRect(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);

	// 1. 일반 객체 그리기
	m_pStandardShader->SetPipelineState(pd3dCommandList);

	for (int i = 0; i < 9; ++i) {
		m_vObjects[i]->Render(pd3dCommandList, m_pCurrentSetShader);
	}

	m_pMirrorInitShader->SetPipelineState(pd3dCommandList);
	// 2. 거울 스텐실 초기화
	pd3dCommandList->OMSetStencilRef(0xff);
	m_vObjects[9]->Render(pd3dCommandList, m_pCurrentSetShader);

	// 3. 거울 깊이 초기화
	m_pDepthInitShader->SetPipelineState(pd3dCommandList);
	m_vObjects[9]->Render(pd3dCommandList, m_pCurrentSetShader);

	XMVECTOR reflectPlane = XMVectorSet(0.0, 0.0, -1.0, 179.0f);
	XMStoreFloat4x4(m_pMappedMatrix, XMMatrixTranspose(XMMatrixReflect(reflectPlane)));
	pd3dCommandList->SetGraphicsRootConstantBufferView(5, m_pd3dReflectBuffer->GetGPUVirtualAddress());

	// 4. 반사된 객체 그리기
	m_pReflectShader->SetPipelineState(pd3dCommandList);
	for (int i = 1; i < 9; ++i) {
		m_vObjects[i]->Render(pd3dCommandList, m_pCurrentSetShader);
	}

	// 5. 거울 블렌딩

	m_pBlendMirrorShader->SetPipelineState(pd3dCommandList);
	m_vObjects[9]->Render(pd3dCommandList, m_pCurrentSetShader);
}