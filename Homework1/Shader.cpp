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

// CTexturedShader ====================================================================================

void CTexturedShader::CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineState;
	::ZeroMemory(&d3dPipelineState, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineState.pRootSignature = pd3dRootSignature.Get();
	d3dPipelineState.PS 
	pd3dDevice->CreateGraphicsPipelineState()
}