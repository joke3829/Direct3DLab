//===================================================
// Shader.h
// 셰이더들을 정리한 헤더
// CShader 클래스를 상속받아 여러 셰이더를 만든다
//===================================================
#pragma once
#include "stdafx.h"

class CShader {
public:
	virtual void CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature) {};

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() {};
	virtual D3D12_SHADER_BYTECODE CreateShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);

	virtual void Render(ComPtr<ID3D12Device>& pd3dDevice);
protected:
	ComPtr<ID3D12PipelineState> m_pd3dPipelineState{ nullptr };
};

class CTexturedShader : public CShader {
public:
	virtual void CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature);
};