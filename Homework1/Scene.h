//=============================================
// Scene.h
// 장면들을 정리한 헤더
// CScene을 상속받아 여러 장면으로 나뉜다.
//=============================================
#pragma once
#include "stdafx.h"
#include "Shader.h"
#include "Camera.h"

class CScene {
public:
	virtual void BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>&pd3dCommandList) {};
	virtual void CreateRootSignature(ComPtr<ID3D12Device>& pd3dDevice);

	void SetCamera(std::shared_ptr<CCamera>& pCamera);

	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList) {};
protected:
	ComPtr<ID3D12RootSignature> m_pd3dRootSignature{ nullptr };
	std::shared_ptr<CCamera> m_pCamera;
};

class CMenuScene : public CScene {
public:
	void BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
	void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
private:
	std::vector<std::unique_ptr<CShader>> m_vShaders;
};
