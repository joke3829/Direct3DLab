//=============================================
// Scene.h
// 장면들을 정리한 헤더
// CScene을 상속받아 여러 장면으로 나뉜다.
//=============================================
#pragma once
#include "stdafx.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

class CScene {
public:
	virtual void BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>&pd3dCommandList) {};
	virtual void CreateRootSignature(ComPtr<ID3D12Device>& pd3dDevice);

	void SetCamera(std::shared_ptr<CCamera>& pCamera);

	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessage, WPARAM wParma, LPARAM lParam) {};
	virtual void ProcessInput(float fElapsedTime) {}

	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList) {};
protected:
	ComPtr<ID3D12RootSignature> m_pd3dRootSignature{ nullptr };
	std::shared_ptr<CCamera> m_pCamera;

	std::shared_ptr<CShader> m_pCurrentSetShader{ nullptr };	// 현재 pipeline에 set돼있는 pipelineState를 저장
};

class CMenuScene : public CScene {
public:
	void BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);

	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam);

	void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
private:
	std::vector<std::unique_ptr<CGameObject>> m_vObjects;
	bool m_bOnExplain{ false };
};

class CIngameScene : public CScene {
public:
	void BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);

	//void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam);

	void ProcessInput(float fElapsedTime);
	
	void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
private:
	std::vector<std::unique_ptr<CGameObject>> m_vObjects;
	std::unique_ptr<CSkyBoxObject> m_pSkyBox;
	std::unique_ptr<HGameObject> m_pPlayer;
};