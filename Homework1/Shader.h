//===================================================
// Shader.h
// ���̴����� ������ ���
// CShader Ŭ������ ��ӹ޾� ���� ���̴��� �����
//===================================================
#pragma once
#include "stdafx.h"
#include "Object.h"

class CShader {
public:
	virtual void CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12RootSignature>& pd3dRootSignature) {};

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() { D3D12_INPUT_LAYOUT_DESC p; return p; };
	virtual D3D12_SHADER_BYTECODE CreateShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_BLEND_DESC CreateBlendDesc();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilDesc();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerDesc();

	virtual void SetPipelineState(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList) {}

	virtual void BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList) {};
	
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lPAram) {}

	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList) {};
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
	virtual void BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);

	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lPAram);

	virtual void SetPipelineState(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
protected:
	std::vector<std::unique_ptr<CGameObject>> m_vObjects;
	bool m_bOnExplain{ false };
};