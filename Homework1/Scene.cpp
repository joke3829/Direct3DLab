//========================================
// Scene.cpp
// 장면들을 정의한 cpp
//========================================
#include "Scene.h"

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

	// 0. 카메라 정보, 1. 월드, 텍스처 테이블, 2. 지형
	D3D12_ROOT_PARAMETER d3dRootParameter[3];
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
	d3dRootSignatureDesc.NumParameters = 3;
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
	m_vObjects.push_back(std::make_unique<CTerrainObject>(pd3dDevice, pd3dCommandList));
	// 0 지형

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

	for (std::unique_ptr<CGameObject>& temp : m_vObjects) {
		temp->CreateResourceView(pd3dDevice);
	}

	m_pSkyBox = std::make_unique<CSkyBoxObject>(pd3dDevice, pd3dCommandList, m_pd3dRootSignature);
}

void CIngameScene::ProcessInput()
{
	UCHAR keyBuffer[256];
	GetKeyboardState(keyBuffer);

	if (keyBuffer['W'] & 0x80)
		m_pCamera->move();
}

void CIngameScene::Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	m_pCurrentSetShader.reset();
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature.Get());
	m_pCamera->SetViewportAndScissorRect(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);

	for (std::unique_ptr<CGameObject>& temp : m_vObjects) {
		temp->Render(pd3dCommandList, m_pCurrentSetShader);
	}
	
	m_pSkyBox->UpdatePosition(m_pCamera->getCameraEye());
	m_pSkyBox->Render(pd3dCommandList, m_pCurrentSetShader);
}

