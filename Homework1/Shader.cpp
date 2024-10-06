//=================================================
// Shader.cpp
// 셰이더들을 정리한 cpp파일
// 클래스 함수들을 정의한다
//=================================================
#include "Shader.h"


D3D12_SHADER_BYTECODE CShader::CreateShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderBCode;
	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile, 0, 0, ppd3dShaderBlob, NULL);
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