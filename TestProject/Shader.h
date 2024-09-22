#pragma once
#include "stdafx.h"
#include "GameObject.h"

class BaseShader {
public:
	virtual void CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ID3D12RootSignature* pd3dRootSignature) {};
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) {}

	D3D12_SHADER_BYTECODE CreateShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() {};
	virtual D3D12_BLEND_DESC CreateBlendDesc();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilDesc();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerDesc();


protected:
	ComPtr<ID3D12PipelineState> m_pd3dPipelineState;
	std::vector<std::unique_ptr<CGameObject>> m_vObjects;
};

class CLightShader : public BaseShader {
public:
	D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	void CreatePipelineState(ComPtr<ID3D12Device>& pd3dDevice, ID3D12RootSignature* pd3dRootSignature);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);

};