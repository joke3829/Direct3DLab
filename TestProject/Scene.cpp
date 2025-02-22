#include "Scene.h"

void CScene::CreateGrahicsRootSignature(ID3D12Device* pd3dDevice)
{
	D3D12_ROOT_PARAMETER d3dRootParameter[2];
	// 월드변환
	d3dRootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	d3dRootParameter[0].Descriptor.RegisterSpace = 0;
	d3dRootParameter[0].Descriptor.ShaderRegister = 0;
	d3dRootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	// 카메라(뷰, 투영)
	d3dRootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	d3dRootParameter[1].Descriptor.RegisterSpace = 0;
	d3dRootParameter[1].Descriptor.ShaderRegister = 1;
	d3dRootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_DESC d3dRSD;
	::ZeroMemory(&d3dRSD, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRSD.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	d3dRSD.NumParameters = 2;
	d3dRSD.pParameters = d3dRootParameter;
	d3dRSD.NumStaticSamplers = 0;
	d3dRSD.pStaticSamplers = NULL;

	ID3DBlob* pd3dBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRSD, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dBlob, &pd3dErrorBlob);
	HRESULT hResult = pd3dDevice->CreateRootSignature(0, pd3dBlob->GetBufferPointer(), pd3dBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_pd3dGraphicsRootSignature);

	if (pd3dBlob)
		pd3dBlob->Release();
	if (pd3dErrorBlob)
		pd3dErrorBlob->Release();
}

D3D12_SHADER_BYTECODE CScene::CreateShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile, 0, 0, ppd3dShaderBlob, NULL);
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return d3dShaderByteCode;
}

D3D12_INPUT_LAYOUT_DESC CScene::CreateInputLayout()
{
	// 안되면 여기 한번 체크
	//std::unique_ptr<D3D12_INPUT_ELEMENT_DESC[]> pd3dIEdesc = std::make_unique<D3D12_INPUT_ELEMENT_DESC[]>(2);
	D3D12_INPUT_ELEMENT_DESC* pd3dIEdesc = new D3D12_INPUT_ELEMENT_DESC[2];
	pd3dIEdesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIEdesc[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dILDesc;
	d3dILDesc.NumElements = 2;
	//d3dILDesc.pInputElementDescs = pd3dIEdesc.get();
	d3dILDesc.pInputElementDescs = pd3dIEdesc;

	return d3dILDesc;
}

D3D12_BLEND_DESC CScene::CreateBlendDesc()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	return d3dBlendDesc;
}

D3D12_DEPTH_STENCIL_DESC CScene::CreateDepthStencilDesc()
{
	D3D12_DEPTH_STENCIL_DESC d3dDSDesc;
	::ZeroMemory(&d3dDSDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDSDesc.DepthEnable = TRUE;
	d3dDSDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDSDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDSDesc.StencilEnable = FALSE;
	d3dDSDesc.StencilReadMask = 0x00;
	d3dDSDesc.StencilWriteMask = 0x00;
	d3dDSDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDSDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDSDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;

	return d3dDSDesc;
}

D3D12_RASTERIZER_DESC CScene::CreateRasterizerDesc()
{
	D3D12_RASTERIZER_DESC d3dRDesc;
	::ZeroMemory(&d3dRDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRDesc.AntialiasedLineEnable = FALSE;
	d3dRDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	d3dRDesc.DepthBias = 0;
	d3dRDesc.DepthBiasClamp = 0.0f;
	d3dRDesc.DepthClipEnable = TRUE;
	d3dRDesc.ForcedSampleCount = 0;
	d3dRDesc.FrontCounterClockwise = FALSE;
	d3dRDesc.MultisampleEnable = FALSE;
	d3dRDesc.SlopeScaledDepthBias = 0.0f;

	return d3dRDesc;
}

void CScene::CreatePipelineState(ID3D12Device* pd3dDevice)
{
	ID3DBlob* pd3dBlob{ nullptr };
	ID3DBlob* pd3dPSBlob{ nullptr };

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineDesc;
	::ZeroMemory(&d3dPipelineDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineDesc.pRootSignature = m_pd3dGraphicsRootSignature;
	d3dPipelineDesc.VS = CreateShaderFromFile(L"Shader.hlsl", "VSDiffused", "vs_5_1", &pd3dBlob);
	d3dPipelineDesc.PS = CreateShaderFromFile(L"Shader.hlsl", "PSDiffused", "ps_5_1", &pd3dPSBlob);
	d3dPipelineDesc.InputLayout = CreateInputLayout();
	d3dPipelineDesc.BlendState = CreateBlendDesc();
	d3dPipelineDesc.RasterizerState = CreateRasterizerDesc();
	d3dPipelineDesc.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineDesc.SampleDesc.Count = 1;
	d3dPipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineDesc.NumRenderTargets = 1;
	d3dPipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineDesc.SampleMask = UINT_MAX;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

	// input element 동적 할당 시 제거 코드 여기 작성
	delete[] d3dPipelineDesc.InputLayout.pInputElementDescs;
}

void CScene::PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pd3dCommandList->SetPipelineState(m_pd3dPipelineState);
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	PrepareRender(pd3dCommandList);

}


void CWallScene::BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pCamera = std::make_unique<CCamera>(pd3dDevice);
	m_pCamera->SetCameraEye(XMFLOAT3(0.0, 0.0, -300));
	// 이 작업 이후 바로 gpu 명령이 끝나길 기다린다.
	m_vMeshes.push_back(std::make_shared<CDiffusedSquareMesh>(pd3dDevice, pd3dCommandList, RED, true, 1270.0f, 710.0f));
	m_vGameObjects.push_back(std::make_unique<CWallObject>(pd3dDevice));
}

void CWallScene::SetMeshes()
{
	m_vGameObjects[0]->SetMesh(m_vMeshes[0]);
}

void CWallScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lPAram)
{
	switch (wParam) {
	case 'W': {
		XMFLOAT3 xmf3Eye = m_pCamera->getCameraEye();
		xmf3Eye.z += 5.0;
		m_pCamera->SetCameraEye(xmf3Eye);
		break;
	}
	case 'S': {
		XMFLOAT3 xmf3Eye = m_pCamera->getCameraEye();
		xmf3Eye.z -= 5.0;
		m_pCamera->SetCameraEye(xmf3Eye);
		break;
	}
	}
}


void CWallScene::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	PrepareRender(pd3dCommandList);
	m_pCamera->SetViewportAndScissorRect(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);
	for (int i = 0; i < m_vGameObjects.size(); ++i)
		m_vGameObjects[i]->Render(pd3dCommandList);
}


void CLightScene::BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

}

void CLightScene::CreateGrahicsRootSignature(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE d3dDescriptorRanges[2];
	// 0번 월드변환
	d3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRanges[0].NumDescriptors = 1;
	d3dDescriptorRanges[0].BaseShaderRegister = 0;
	d3dDescriptorRanges[0].RegisterSpace = 0;
	d3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;
	// 1번 마테리얼
	d3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRanges[1].NumDescriptors = 1;
	d3dDescriptorRanges[1].BaseShaderRegister = 2;
	d3dDescriptorRanges[1].RegisterSpace = 0;
	d3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 1;

	D3D12_ROOT_PARAMETER d3dRootParameter[4];
	// 0번 카메라 정보
	d3dRootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	d3dRootParameter[0].Descriptor.RegisterSpace = 0;
	d3dRootParameter[0].Descriptor.ShaderRegister = 1;
	d3dRootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	// 1번 월드, 마테리얼 (서술자 테이블)
	d3dRootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	d3dRootParameter[1].DescriptorTable.NumDescriptorRanges = 2;
	d3dRootParameter[1].DescriptorTable.pDescriptorRanges = d3dDescriptorRanges;
	d3dRootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// 2번 전역 조명 정보(방향성 조명)
	d3dRootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	d3dRootParameter[2].Descriptor.RegisterSpace = 0;
	d3dRootParameter[2].Descriptor.ShaderRegister = 3;
	d3dRootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	d3dRootSignatureDesc.NumParameters = 3;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pParameters = d3dRootParameter;
	d3dRootSignatureDesc.pStaticSamplers = nullptr;

	ID3DBlob* pd3dBlob{ nullptr };
	ID3DBlob* pd3dErrorBlob{ nullptr };

	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dBlob->GetBufferPointer(), pd3dBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_pd3dGraphicsRootSignature);

	pd3dBlob->Release();
	pd3dErrorBlob->Release();

}


//========================================================================

void NoShaderScene::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pd3dCommandList->SetPipelineState(m_pd3dPipelineState);
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pd3dCommandList->DrawInstanced(3, 1, 0, 0);
}

void NoShaderScene::CreatePipelineState(ID3D12Device* pd3dDevice)
{
	ID3DBlob* pd3dBlob{ nullptr };
	ID3DBlob* pd3dPSBlob{ nullptr };

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineDesc;
	::ZeroMemory(&d3dPipelineDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineDesc.pRootSignature = m_pd3dGraphicsRootSignature;
	d3dPipelineDesc.VS = CreateShaderFromFile(L"Shader.hlsl", "VSNoShader", "vs_5_1", &pd3dBlob);
	d3dPipelineDesc.PS = CreateShaderFromFile(L"Shader.hlsl", "PSNoShader", "ps_5_1", &pd3dPSBlob);
	d3dPipelineDesc.InputLayout.NumElements = 0;
	d3dPipelineDesc.InputLayout.pInputElementDescs = NULL;
	d3dPipelineDesc.BlendState = CreateBlendDesc();
	d3dPipelineDesc.RasterizerState = CreateRasterizerDesc();
	d3dPipelineDesc.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineDesc.SampleDesc.Count = 1;
	d3dPipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineDesc.NumRenderTargets = 1;
	d3dPipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineDesc.SampleMask = UINT_MAX;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

	// input element 동적 할당 시 제거 코드 여기 작성
}