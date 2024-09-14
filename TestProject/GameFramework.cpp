#include "GameFramework.h"

void CGameFramework::InitFramework(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;

	_tcscpy_s(m_pszFrameRate, _T("TestProject ("));

	CreateDevice();
	CreateCommandLAQ();
	CreateSwapChain();
	CreateRTVDSV();

	m_pScene = std::make_unique<CWallScene>();

	m_pScene->CreateGrahicsRootSignature(m_pd3dDevice.Get());
	m_pScene->CreatePipelineState(m_pd3dDevice.Get());
	BuildObject();
}

void CGameFramework::BuildObject()
{
	m_pScene->BuildObject(m_pd3dDevice, m_pd3dCommandList);
	WaitForGPUComplete();
	dynamic_cast<CWallScene*>(m_pScene.get())->SetMeshes();
}

void CGameFramework::Release()
{
	WaitForGPUComplete();

	::CloseHandle(m_hFenceEvent);

	for (int i = 0; i < m_nSwapChainBuffers; ++i) 
		if (m_pd3dRenderTargetBuffers[i])
			m_pd3dRenderTargetBuffers[i]->Release();
	if (m_pd3dRTVDescriptorHeap)
		m_pd3dRTVDescriptorHeap->Release();
	if (m_pd3dDepthStencilBuffer)
		m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDSVDescriptorHeap)
		m_pd3dDSVDescriptorHeap->Release();

	if (m_pd3dCommandAllocator)
		m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue)
		m_pd3dCommandQueue->Release();
	if (m_pd3dPipelineState)
		m_pd3dPipelineState->Release();
	if (m_pd3dCommandList)
		m_pd3dCommandList->Release();

	if (m_pd3dFence)
		m_pd3dFence->Release();

	if (m_pdxgiSwapChain)
		m_pdxgiSwapChain->Release();
	//if (m_pd3dDevice)
		//m_pd3dDevice->Release();
	if (m_pdxgiFactory)
		m_pdxgiFactory->Release();
}

void CGameFramework::CreateDevice()
{
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pdxgiFactory);
	IDXGIAdapter* pdxgiAdapter{ nullptr };
	for (UINT i = 0; m_pdxgiFactory->EnumAdapters(i, &pdxgiAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		DXGI_ADAPTER_DESC dxgiAdapterDesc;
		pdxgiAdapter->GetDesc(&dxgiAdapterDesc);
		if (SUCCEEDED(D3D12CreateDevice(pdxgiAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_pd3dDevice)))
			break;
	}
	if (!pdxgiAdapter) {
		m_pdxgiFactory->EnumWarpAdapter(__uuidof(IDXGIAdapter), (void**)&pdxgiAdapter);
		D3D12CreateDevice(pdxgiAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
	m_nFenceValue = 0;
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	if (pdxgiAdapter)
		pdxgiAdapter->Release();
}

void CGameFramework::CreateCommandLAQ()
{
	m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);
	m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);

	m_pd3dCommandList->Close();
}

void CGameFramework::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = FRAME_BUFFER_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = FRAME_BUFFER_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.Flags = 0;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);

	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

void CGameFramework::CreateRTVDSV()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRTVDescriptorHeap);

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDSVDescriptorHeap);
	m_nRTVIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_nDSVIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dCPUDesHandle = m_pd3dRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; ++i) {
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_pd3dRenderTargetBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_pd3dRenderTargetBuffers[i], NULL, d3dCPUDesHandle);
		d3dCPUDesHandle.ptr += m_nRTVIncrementSize;
	}

	D3D12_HEAP_PROPERTIES d3dHP;
	::ZeroMemory(&d3dHP, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHP.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHP.CreationNodeMask = 1;
	d3dHP.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHP.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHP.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dRD;
	::ZeroMemory(&d3dRD, sizeof(D3D12_RESOURCE_DESC));
	d3dRD.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dRD.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dRD.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	d3dRD.Alignment = 0;
	d3dRD.DepthOrArraySize = 1;
	d3dRD.Height = FRAME_BUFFER_HEIGHT;
	d3dRD.Width = FRAME_BUFFER_WIDTH;
	d3dRD.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dRD.MipLevels = 1;
	d3dRD.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dRD.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;

	D3D12_CLEAR_VALUE d3dCV;
	::ZeroMemory(&d3dCV, sizeof(D3D12_CLEAR_VALUE));
	d3dCV.DepthStencil.Depth = 1.0f;
	d3dCV.DepthStencil.Stencil = 0;
	d3dCV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	m_pd3dDevice->CreateCommittedResource(&d3dHP, D3D12_HEAP_FLAG_NONE, &d3dRD, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dCV, __uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dCPUDesHandleD = m_pd3dDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dCPUDesHandleD);
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return 0;
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		}
		break;
	}
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
		break;
	}
}

void CGameFramework::WaitForGPUComplete()
{
	++m_nFenceValue;
	m_pd3dCommandQueue->Signal(m_pd3dFence, m_nFenceValue);
	if (m_pd3dFence->GetCompletedValue() < m_nFenceValue) {
		m_pd3dFence->SetEventOnCompletion(m_nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(60.0f);

	// 입력 처리

	// 애니메이션 처리

	m_pd3dCommandAllocator->Reset();
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Transition.pResource = m_pd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	D3D12_VIEWPORT d3dvp;
	d3dvp.Height = FRAME_BUFFER_HEIGHT;
	d3dvp.MaxDepth = 1.0f;
	d3dvp.MinDepth = 0.0f;
	d3dvp.Width = FRAME_BUFFER_WIDTH;
	d3dvp.TopLeftX = 0;
	d3dvp.TopLeftY = 0;

	D3D12_RECT d3dRect;
	d3dRect.left = 0;
	d3dRect.right = 1280;
	d3dRect.top = 0;
	d3dRect.bottom = 720;
	m_pd3dCommandList->RSSetViewports(1, &d3dvp);
	m_pd3dCommandList->RSSetScissorRects(1, &d3dRect);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dCPUHandle = m_pd3dRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dCPUHandle.ptr += (m_nSwapChainBufferIndex * m_nRTVIncrementSize);
	float color[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dCPUHandle, color, 0, NULL);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dCPUHandleD = m_pd3dDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->ClearDepthStencilView(d3dCPUHandleD, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, NULL);

	m_pd3dCommandList->OMSetRenderTargets(1, &d3dCPUHandle, TRUE, &d3dCPUHandleD);


	// 렌더링 코드 추가
	m_pScene->Render(m_pd3dCommandList);


	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	m_pd3dCommandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGPUComplete();

	m_pdxgiSwapChain->Present(0, 0);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 13, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}