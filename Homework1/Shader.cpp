//=================================================
// Shader.cpp
// 셰이더들을 정리한 cpp파일
// 클래스 함수들을 정의한다
//=================================================
#include "Shader.h"
#include "Mesh.h"


D3D12_SHADER_BYTECODE CShader::CreateShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderBCode;
	HRESULT hResult = ::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile, 0, 0, ppd3dShaderBlob, NULL);
	d3dShaderBCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderBCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	return d3dShaderBCode;
}

D3D12_BLEND_DESC CShader::CreateBlendDesc()
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

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilDesc()
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

D3D12_RASTERIZER_DESC CShader::CreateRasterizerDesc()
{
	D3D12_RASTERIZER_DESC d3dRDesc;
	::ZeroMemory(&d3dRDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRDesc.CullMode = D3D12_CULL_MODE_BACK;
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

// CTexturedShader ====================================================================================

D3D12_INPUT_LAYOUT_DESC CTexturedShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[3];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.NumElements = 3;
	d3dInputLayoutDesc.pInputElementDescs = pd3dIE;
	return d3dInputLayoutDesc;
}

void CTexturedShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSTextured", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSTextured", "ps_5_1", &pd3dPBlob);
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

D3D12_INPUT_LAYOUT_DESC CTexturedDepthShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[3];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.NumElements = 3;
	d3dInputLayoutDesc.pInputElementDescs = pd3dIE;
	return d3dInputLayoutDesc;
}

void CTexturedDepthShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSTexturedDepth", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSTexturedDepth", "ps_5_1", &pd3dPBlob);
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

//==============================================================================

D3D12_INPUT_LAYOUT_DESC CMenuShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[2];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dILDesc;
	d3dILDesc.pInputElementDescs = pd3dIE;
	d3dILDesc.NumElements = 2;
	return d3dILDesc;
}

void CMenuShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSNoLightTextured", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSNoLightTextured", "ps_5_1", &pd3dPBlob);
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}


//==========================================================================

D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[3];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[2] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dILDesc;
	d3dILDesc.pInputElementDescs = pd3dIE;
	d3dILDesc.NumElements = 3;
	return d3dILDesc;
}

void CTerrainShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSTerrain", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSTerrain", "ps_5_1", &pd3dPBlob);
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

D3D12_INPUT_LAYOUT_DESC CTerrainDepthShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[3];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[2] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dILDesc;
	d3dILDesc.pInputElementDescs = pd3dIE;
	d3dILDesc.NumElements = 3;
	return d3dILDesc;
}

void CTerrainDepthShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSTerrainDepth", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSTerrainDepth", "ps_5_1", &pd3dPBlob);
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

// =====================================================================

void CSkyBoxShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSSkyBox", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSSkyBox", "ps_5_1", &pd3dPBlob);
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

D3D12_INPUT_LAYOUT_DESC CSkyBoxShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[2];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dILDesc;
	d3dILDesc.pInputElementDescs = pd3dIE;
	d3dILDesc.NumElements = 2;
	return d3dILDesc;
}
D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilDesc()
{
	D3D12_DEPTH_STENCIL_DESC d3dDSDesc;
	::ZeroMemory(&d3dDSDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDSDesc.DepthEnable = FALSE;
	d3dDSDesc.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;
	d3dDSDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
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

//=================================================================

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[5];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.NumElements = 5;
	d3dInputLayoutDesc.pInputElementDescs = pd3dIE;
	return d3dInputLayoutDesc;
}

void CPlayerShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSStandard", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSStandard", "ps_5_1", &pd3dPBlob);
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

D3D12_INPUT_LAYOUT_DESC CPlayerDepthShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[5];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.NumElements = 5;
	d3dInputLayoutDesc.pInputElementDescs = pd3dIE;
	return d3dInputLayoutDesc;
}

void CPlayerDepthShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSStandardDepth", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSStandardDepth", "ps_5_1", &pd3dPBlob);
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

//==================================================================

void CWaterShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSWater", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSWater", "ps_5_1", &pd3dPBlob);
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

D3D12_INPUT_LAYOUT_DESC CWaterShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[1];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dIL;
	d3dIL.NumElements = 1;
	d3dIL.pInputElementDescs = pd3dIE;

	return d3dIL;  
}

D3D12_BLEND_DESC CWaterShader::CreateBlendDesc()
{
	D3D12_BLEND_DESC d3dBD;
	::ZeroMemory(&d3dBD, sizeof(D3D12_BLEND_DESC));
	d3dBD.AlphaToCoverageEnable = FALSE;
	d3dBD.IndependentBlendEnable = FALSE;
	d3dBD.RenderTarget[0].BlendEnable = TRUE;
	d3dBD.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBD.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBD.RenderTarget[0].DestBlend = D3D12_BLEND_DEST_ALPHA;
	d3dBD.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	d3dBD.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBD.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBD.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBD.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBD.RenderTarget[0].LogicOpEnable = FALSE;

	return d3dBD;
}

//===================================================================

void CBillboardShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	ID3DBlob* pd3dGBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSBillboard", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.GS = CreateShaderFromFile(L"Shader.hlsl", "GSBillboard", "gs_5_1", &pd3dGBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSBillboard", "ps_5_1", &pd3dPBlob);
	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();
	if (pd3dGBlob)
		pd3dGBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

D3D12_INPUT_LAYOUT_DESC CBillboardShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[2];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[1] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	::ZeroMemory(&d3dInputLayoutDesc, sizeof(D3D12_INPUT_LAYOUT_DESC));
	d3dInputLayoutDesc.NumElements = 2;
	d3dInputLayoutDesc.pInputElementDescs = pd3dIE;
	return d3dInputLayoutDesc;
}

D3D12_RASTERIZER_DESC CBillboardShader::CreateRasterizerDesc()
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

//====================================================================

void CParticleSOShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dGBlob{ nullptr };
	//ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 0;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSSOParticle", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.GS = CreateShaderFromFile(L"Shader.hlsl", "GSSOParticle", "gs_5_1", &pd3dGBlob);
	d3dPipelineState.PS.BytecodeLength = 0; d3dPipelineState.PS.pShaderBytecode = NULL;
	d3dPipelineState.StreamOutput = CreateStreamOutputDesc();
	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dGBlob)
		pd3dGBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
	delete[] d3dPipelineState.StreamOutput.pBufferStrides;
	delete[] d3dPipelineState.StreamOutput.pSODeclaration;
}

D3D12_STREAM_OUTPUT_DESC CParticleSOShader::CreateStreamOutputDesc()
{
	D3D12_STREAM_OUTPUT_DESC d3dSODesc;
	::ZeroMemory(&d3dSODesc, sizeof(D3D12_STREAM_OUTPUT_DESC));
	D3D12_SO_DECLARATION_ENTRY* pd3dSOEntry = new D3D12_SO_DECLARATION_ENTRY[5];
	pd3dSOEntry[0] = { 0, "POSITION", 0, 0, 3, 0 };
	pd3dSOEntry[1] = { 0, "VELOCITY", 0, 0, 3, 0 };
	pd3dSOEntry[2] = { 0, "SIZE", 0, 0, 1, 0 };
	pd3dSOEntry[3] = { 0, "LIFETIME", 0, 0, 1, 0 };
	pd3dSOEntry[4] = { 0, "PARTICLETYPE", 0, 0, 1, 0 };

	UINT* pStride = new UINT[1];
	pStride[0] = sizeof(CParticleVertex);
	
	d3dSODesc.NumEntries = 5;
	d3dSODesc.pSODeclaration = pd3dSOEntry;
	d3dSODesc.NumStrides = 1;
	d3dSODesc.pBufferStrides = pStride;
	d3dSODesc.RasterizedStream = D3D12_SO_NO_RASTERIZED_STREAM;

	return d3dSODesc;
}

D3D12_INPUT_LAYOUT_DESC CParticleSOShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dILDesc;

	D3D12_INPUT_ELEMENT_DESC* pd3dIEDesc = new D3D12_INPUT_ELEMENT_DESC[5];
	pd3dIEDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIEDesc[1] = { "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIEDesc[2] = { "SIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIEDesc[3] = { "LIFETIME", 0, DXGI_FORMAT_R32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIEDesc[4] = { "PARTICLETYPE", 0, DXGI_FORMAT_R32_UINT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	d3dILDesc.NumElements = 5;
	d3dILDesc.pInputElementDescs = pd3dIEDesc;
	return d3dILDesc;
}

void CParticleDrawShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dGBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSDrawParticle", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.GS = CreateShaderFromFile(L"Shader.hlsl", "GSDrawParticle", "gs_5_1", &pd3dGBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSDrawParticle", "ps_5_1", &pd3dPBlob);
	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dGBlob)
		pd3dGBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

D3D12_INPUT_LAYOUT_DESC CParticleDrawShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dILDesc;

	D3D12_INPUT_ELEMENT_DESC* pd3dIEDesc = new D3D12_INPUT_ELEMENT_DESC[5];
	pd3dIEDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIEDesc[1] = { "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIEDesc[2] = { "SIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIEDesc[3] = { "LIFETIME", 0, DXGI_FORMAT_R32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIEDesc[4] = { "PARTICLETYPE", 0, DXGI_FORMAT_R32_UINT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	d3dILDesc.NumElements = 5;
	d3dILDesc.pInputElementDescs = pd3dIEDesc;
	return d3dILDesc;
}

D3D12_RASTERIZER_DESC CParticleDrawShader::CreateRasterizerDesc()
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

//===============================================================

void CMirrorInitializeShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSNoLightTextured", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSMirrorInitialize", "ps_5_1", &pd3dPBlob);
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

D3D12_INPUT_LAYOUT_DESC CMirrorInitializeShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[2];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dILDesc;
	d3dILDesc.pInputElementDescs = pd3dIE;
	d3dILDesc.NumElements = 2;
	return d3dILDesc;
}

D3D12_DEPTH_STENCIL_DESC CMirrorInitializeShader::CreateDepthStencilDesc()
{
	D3D12_DEPTH_STENCIL_DESC d3dDSDesc;
	::ZeroMemory(&d3dDSDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDSDesc.DepthEnable = TRUE;
	d3dDSDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDSDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDSDesc.StencilEnable = TRUE;
	d3dDSDesc.StencilReadMask = 0xff;
	d3dDSDesc.StencilWriteMask = 0xff;
	d3dDSDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDSDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	d3dDSDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDSDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;

	return d3dDSDesc;
}


void CMirrorDepthInitShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSMirrorDepthInit", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSMirrorInitialize", "ps_5_1", &pd3dPBlob);
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

D3D12_INPUT_LAYOUT_DESC CMirrorDepthInitShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[2];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dILDesc;
	d3dILDesc.pInputElementDescs = pd3dIE;
	d3dILDesc.NumElements = 2;
	return d3dILDesc;
}

D3D12_DEPTH_STENCIL_DESC CMirrorDepthInitShader::CreateDepthStencilDesc()
{
	D3D12_DEPTH_STENCIL_DESC d3dDSDesc;
	::ZeroMemory(&d3dDSDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDSDesc.DepthEnable = TRUE;
	d3dDSDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	d3dDSDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDSDesc.StencilEnable = TRUE;
	d3dDSDesc.StencilReadMask = 0xff;
	d3dDSDesc.StencilWriteMask = 0xff;
	d3dDSDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	d3dDSDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDSDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;

	return d3dDSDesc;
}

void CReflectShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSReflect", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSNoLightTextured", "ps_5_1", &pd3dPBlob);
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

D3D12_INPUT_LAYOUT_DESC CReflectShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[2];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dILDesc;
	d3dILDesc.pInputElementDescs = pd3dIE;
	d3dILDesc.NumElements = 2;
	return d3dILDesc;
}
D3D12_DEPTH_STENCIL_DESC CReflectShader::CreateDepthStencilDesc()
{
	D3D12_DEPTH_STENCIL_DESC d3dDSDesc;
	::ZeroMemory(&d3dDSDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDSDesc.DepthEnable = TRUE;
	d3dDSDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDSDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDSDesc.StencilEnable = TRUE;
	d3dDSDesc.StencilReadMask = 0xff;
	d3dDSDesc.StencilWriteMask = 0xff;
	d3dDSDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	d3dDSDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDSDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDSDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;

	return d3dDSDesc;
}
D3D12_RASTERIZER_DESC CReflectShader::CreateRasterizerDesc()
{
	D3D12_RASTERIZER_DESC d3dRDesc;
	::ZeroMemory(&d3dRDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRDesc.AntialiasedLineEnable = FALSE;
	d3dRDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	d3dRDesc.DepthBias = 0;
	d3dRDesc.DepthBiasClamp = 0.0f;
	d3dRDesc.DepthClipEnable = TRUE;
	d3dRDesc.ForcedSampleCount = 0;
	d3dRDesc.FrontCounterClockwise = TRUE;
	d3dRDesc.MultisampleEnable = FALSE;
	d3dRDesc.SlopeScaledDepthBias = 0.0f;

	return d3dRDesc;
}


void CBlendMirrorShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	ID3DBlob* pd3dVBlob{ nullptr };
	ID3DBlob* pd3dPBlob{ nullptr };
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.InputLayout = CreateInputLayout();
	d3dPipelineState.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineState.RasterizerState = CreateRasterizerDesc();
	d3dPipelineState.BlendState = CreateBlendDesc();
	d3dPipelineState.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineState.NumRenderTargets = 1;
	d3dPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineState.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineState.SampleDesc.Count = 1;
	d3dPipelineState.SampleMask = UINT_MAX;
	d3dPipelineState.VS = CreateShaderFromFile(L"Shader.hlsl", "VSNoLightTextured", "vs_5_1", &pd3dVBlob);
	d3dPipelineState.PS = CreateShaderFromFile(L"Shader.hlsl", "PSBlendMirror", "ps_5_1", &pd3dPBlob);
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineState, __uuidof(ID3D12PipelineState), (void**)m_pd3dPipelineState.GetAddressOf());

	if (pd3dVBlob)
		pd3dVBlob->Release();
	if (pd3dPBlob)
		pd3dPBlob->Release();

	delete[] d3dPipelineState.InputLayout.pInputElementDescs;
}

D3D12_INPUT_LAYOUT_DESC CBlendMirrorShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dIE = new D3D12_INPUT_ELEMENT_DESC[2];
	pd3dIE[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dIE[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dILDesc;
	d3dILDesc.pInputElementDescs = pd3dIE;
	d3dILDesc.NumElements = 2;
	return d3dILDesc;
}

D3D12_BLEND_DESC CBlendMirrorShader::CreateBlendDesc()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_DEST_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	return d3dBlendDesc;
}