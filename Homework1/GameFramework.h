//=================================================
// GameFramework.h
// �����ӿ�ũ Ŭ������ ������ ���
//=================================================
#pragma once
#include "stdafx.h"
#include "Timer.h"
#include "Scene.h"

enum ProgramState { ���θ޴�, �ΰ��� };

class CGameFramework {
public:
	bool InitFramework(HWND hWnd, HINSTANCE hInstance);	// �����ӿ�ũ�� �ʿ��� ��ҵ��� �����Ѵ�.
	void Release();

	void CreateDevice();		// factory, device, fence ����
	void CreateCommandLAQ();	// commandList, allocator, queue ����
	void CreateSwapChain();		// swapChain ����
	void CreateRTVDSV();		// RTV, DSV ����

	void BuildObject();			// ���� ��鿡 ���� ������Ʈ ����

	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam);

	void WaitForGPUComplete();
	void FrameAdvance();		// Render�Լ�

private:
	HWND m_hWnd;
	HINSTANCE m_hInstance;

	ComPtr<IDXGIFactory4> m_pdxgiFactory{ nullptr };
	ComPtr<ID3D12Device> m_pd3dDevice{ nullptr };
	ComPtr<IDXGISwapChain3> m_pdxgiSwapChain{ nullptr };

	bool m_bMsaa4xEnable{ false };
	UINT m_nMsaa4xQualityLevel{ 0 };

	ComPtr<ID3D12CommandAllocator> m_pd3dCommandAllocator{ nullptr };
	ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList{ nullptr };
	ComPtr<ID3D12CommandQueue> m_pd3dCommandQueue{ nullptr };

	static const UINT m_nSwapChainBuffer = 2;
	UINT m_nCurrentSwapChainIndex{ 0 };

	ComPtr<ID3D12Resource> m_pd3dRTVBuffer[m_nSwapChainBuffer];
	ComPtr<ID3D12DescriptorHeap> m_pd3dRTVDescriptorHeap{ nullptr };
	UINT m_nRTVIncrementSize{ 0 };

	ComPtr<ID3D12Resource> m_pd3dDSVBuffer{ nullptr };
	ComPtr<ID3D12DescriptorHeap> m_pd3dDSVDescriptorHeap{ nullptr };
	UINT m_nDSVIncrementSize{ 0 };
	
	// ����ȭ�� ���� fence
	ComPtr<ID3D12Fence> m_pd3dFence{ nullptr };
	HANDLE m_hFenceEvent;
	UINT64 m_nFenceValue{ 0 };

	CGameTimer m_GameTimer;

	_TCHAR m_pszFrameRate[50];

	std::shared_ptr<CCamera> m_pCamera;

	std::unique_ptr<CScene> m_pScene;
	ProgramState m_nProgramState;

	POINT ClientCenter;
};

