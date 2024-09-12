#pragma once
#include "stdafx.h"
#include "Timer.h"
#include "Scene.h"

class CGameFramework
{
public:
	void InitFramework(HINSTANCE hInstance, HWND hWnd);
	void BuildObject();
	void Release();

	void CreateDevice();
	void CreateSwapChain();
	void CreateCommandLAQ();
	void CreateRTVDSV();

	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lPAram);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lPAram);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lPAram);

	void WaitForGPUComplete();
	void FrameAdvance();
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	IDXGIFactory4* m_pdxgiFactory{ nullptr };
	ComPtr<ID3D12Device> m_pd3dDevice{ nullptr };
	//ID3D12Device* m_pd3dDevice{ nullptr };
	IDXGISwapChain3* m_pdxgiSwapChain{ nullptr };

	bool m_bMsaa4xEnable{ false };
	UINT m_nMsaa4xQualityLevels = 0;

	ID3D12CommandAllocator* m_pd3dCommandAllocator{ nullptr };
	ID3D12GraphicsCommandList* m_pd3dCommandList{ nullptr };
	ID3D12CommandQueue* m_pd3dCommandQueue{ nullptr };

	static const UINT m_nSwapChainBuffers = 2;
	UINT m_nSwapChainBufferIndex{ 0 };

	ID3D12Fence* m_pd3dFence{ nullptr };
	HANDLE m_hFenceEvent;
	UINT64 m_nFenceValue;

	ID3D12Resource* m_pd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3dRTVDescriptorHeap{ nullptr };
	UINT m_nRTVIncrementSize;

	ID3D12Resource* m_pd3dDepthStencilBuffer{ nullptr };
	ID3D12DescriptorHeap* m_pd3dDSVDescriptorHeap{ nullptr };
	UINT m_nDSVIncrementSize;

	ID3D12PipelineState* m_pd3dPipelineState{ nullptr };

	CGameTimer m_GameTimer;

	_TCHAR m_pszFrameRate[50];

	std::unique_ptr<CScene> m_pScene;
};

