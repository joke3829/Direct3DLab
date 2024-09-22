#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "Camera.h"
#include "Shader.h"

class CScene {
public:
	CScene() {}
	virtual void BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
	virtual void ReleaseObject() {};

	virtual void AnimateObject() {};

	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lPAram) {};

	D3D12_SHADER_BYTECODE CreateShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
	D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	D3D12_BLEND_DESC CreateBlendDesc();
	D3D12_DEPTH_STENCIL_DESC CreateDepthStencilDesc();
	D3D12_RASTERIZER_DESC CreateRasterizerDesc();

	virtual void CreateGrahicsRootSignature(ID3D12Device* pd3dDevice);
	virtual void CreatePipelineState(ID3D12Device* pd3dDevice);

	virtual void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
protected:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature{ nullptr };
	ID3D12PipelineState* m_pd3dPipelineState{ nullptr };
};


class CWallScene : public CScene {
public:
	CWallScene() : CScene() {}
	void BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetMeshes();

	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lPAram);

	//void AnimateObject();

	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
private:
	std::vector<std::shared_ptr<CMesh>> m_vMeshes;
	std::vector<std::unique_ptr<CGameObject>> m_vGameObjects;
	std::unique_ptr<CCamera> m_pCamera;
};

class CLightScene : public CScene {
public:
	void BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void CreateGrahicsRootSignature(ID3D12Device* pd3dDevice);

	void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
private:
	std::vector<std::unique_ptr<BaseShader>> m_vShaders;
};

// ===========================================================================================
class NoShaderScene : public CScene {
public:
	NoShaderScene() : CScene() { }
	void BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	//void CreateGrahicsRootSignature(ID3D12Device* pd3dDevice);
	void CreatePipelineState(ID3D12Device* pd3dDevice);
};

