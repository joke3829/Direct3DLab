//===================================================
// Shader.h
// ���̴����� ������ ���
// CShader Ŭ������ ��ӹ޾� ���� ���̴��� �����
//===================================================
#pragma once
#include "stdafx.h"

class CShader {
public:
	virtual void CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature) {};

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() { D3D12_INPUT_LAYOUT_DESC p; return p; };
	virtual D3D12_SHADER_BYTECODE CreateShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_BLEND_DESC CreateBlendDesc();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilDesc();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerDesc();

	virtual void SetPipelineState(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList) 
	{
		pd3dCommandList->SetPipelineState(m_pd3dPipelineState.Get());
	}
protected:
	ComPtr<ID3D12PipelineState> m_pd3dPipelineState{ nullptr };
};

class CTexturedShader : public CShader {
public:
	virtual void CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
};

// �޴����� ������ ������� �ʴ� �ؽ�ó���̴�
class CMenuShader : public CShader {
public:
	CMenuShader() {}
	virtual void CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
};

class CTerrainShader : public CShader {
public:
	CTerrainShader() {}
	void CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature);

	D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
};

class CSkyBoxShader : public CShader {
public:
	CSkyBoxShader() {}
	virtual void CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilDesc();
	
};