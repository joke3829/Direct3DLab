#include "Shader.h"

D3D12_SHADER_BYTECODE BaseShader::CreateShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile, 0, 0, ppd3dShaderBlob, NULL);
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	return d3dShaderByteCode;
}

D3D12_BLEND_DESC BaseShader::CreateBlendDesc()
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

D3D12_DEPTH_STENCIL_DESC BaseShader::CreateDepthStencilDesc()
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

D3D12_RASTERIZER_DESC BaseShader::CreateRasterizerDesc()
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

//==============================================================================================

D3D12_INPUT_LAYOUT_DESC CLightShader::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* pd3dElments = new D3D12_INPUT_ELEMENT_DESC[2];
	pd3dElments[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dElments[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dILDesc;
	d3dILDesc.NumElements = 2;
	d3dILDesc.pInputElementDescs = pd3dElments;

	return d3dILDesc;
}


void CLightShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
	ID3DBlob* pd3dBlob{ nullptr };
	ID3DBlob* pd3dPSBlob{ nullptr };

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.BlendState = CreateBlendDesc();
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerDesc();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilDesc();
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.VS = CreateShaderFromFile(L"Shader.hlsl", "VSLight", "vs_5_1", &pd3dBlob);
	d3dPipelineStateDesc.PS = CreateShaderFromFile(L"Shader,hlsl", "PSLight", "ps_5_1", &pd3dPSBlob);
	d3dPipelineStateDesc.SampleMask = UINT_MAX;

	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

	delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CLightShader::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_vObjects.size(); ++i)
		m_vObjects[i]->Render(pd3dCommandList);
}