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

	// 0. 카메라 정보, 1. 월드, 텍스처 테이블, 2. 지형, 3. 방향성 조명
	D3D12_ROOT_PARAMETER d3dRootParameter[4];
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
	
	D3D12_STATIC_SAMPLER_DESC d3dStaticSamplerDesc;
	::ZeroMemory(&d3dStaticSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dStaticSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dStaticSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dStaticSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dStaticSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dStaticSamplerDesc.MipLODBias = 0.0f;
	d3dStaticSamplerDesc.MaxAnisotropy = 1;
	d3dStaticSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dStaticSamplerDesc.MinLOD = -FLT_MAX;
	d3dStaticSamplerDesc.MaxLOD = FLT_MAX;
	d3dStaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	d3dStaticSamplerDesc.RegisterSpace = 0;
	d3dStaticSamplerDesc.ShaderRegister = 0;
	d3dStaticSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	d3dRootSignatureDesc.NumParameters = 4;
	d3dRootSignatureDesc.NumStaticSamplers = 1;
	d3dRootSignatureDesc.pParameters = d3dRootParameter;
	d3dRootSignatureDesc.pStaticSamplers = &d3dStaticSamplerDesc;

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
	pMesh = std::make_shared<CBillboardMesh>(pd3dDevice, pd3dCommandList, XMFLOAT3(257.0, 200.0, 257.0), XMFLOAT2(50.0, 60.0));
	pMaterial = std::make_shared<CSingleTexture>(pd3dDevice, pd3dCommandList, L"texture\\Tree02.png", false);
	pShader = std::make_shared<CBillboardShader>(); pShader->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
	m_vObjects.push_back(std::make_unique<CGameObject>(pd3dDevice, pd3dCommandList));
	m_vObjects[11]->SetMesh(pMesh); m_vObjects[11]->SetShader(pShader); m_vObjects[11]->SetMaterial(pMaterial);

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
		}
		break;
	case WM_KEYUP:
		break;
	}
}

void CIngameScene::Animate(float fElapsedTime)
{
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

void CIngameScene::Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	m_pCurrentSetShader.reset();
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature.Get());
	m_pCamera->SetViewportAndScissorRect(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);

	m_pLight->PrepareRender(pd3dCommandList);

	m_pPlayer->Render(pd3dCommandList, m_pCurrentSetShader);

	for (int i = 0; i < 3; ++i) {
		if(m_bAlive[i])
			m_vOpposite[i]->Render(pd3dCommandList, m_pCurrentSetShader);
	}

	for (std::unique_ptr<CGameObject>& temp : m_vObjects) {
		temp->Render(pd3dCommandList, m_pCurrentSetShader);
	}

	m_pWater->Render(pd3dCommandList, m_pCurrentSetShader);

	m_pSkyBox->UpdatePosition(m_pCamera->getCameraEye());
	m_pSkyBox->Render(pd3dCommandList, m_pCurrentSetShader);
}

