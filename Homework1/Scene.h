//=============================================
// Scene.h
// ������ ������ ���
// CScene�� ��ӹ޾� ���� ������� ������.
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

	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessage, WPARAM wParma, LPARAM lParam) {};

	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList) {};
protected:
	ComPtr<ID3D12RootSignature> m_pd3dRootSignature{ nullptr };
	std::shared_ptr<CCamera> m_pCamera;
};

class CMenuScene : public CScene {
public:
	void BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);

	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam);

	void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
private:
	std::vector<std::unique_ptr<CShader>> m_vShaders;
};

class CIngameScene : public CScene {
public:
	void BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);

	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam);
	
	void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
private:
	std::vector<std::unique_ptr<CShader>> m_vShaders;
};