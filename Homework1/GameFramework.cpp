//=================================================
// GameFramework.cpp
// 프레임워크 클래스를 정리한 cpp파일
//=================================================
#include "GameFramework.h"

bool CGameFramework::InitFramework(HWND hWnd, HINSTANCE hInstance)
{
	m_hWnd = hWnd;
	m_hInstance = hInstance;

	_tcscpy_s(m_pszFrameRate, _T("TestProject ("));

	CreateDevice();
	CreateCommandLAQ();
	CreateSwapChain();
	CreateRTVDSV();

	CoInitialize(NULL);

	m_pCamera = std::make_shared<CCamera>(m_pd3dDevice);
	m_pCamera->SetCameraEye(XMFLOAT3(0.0f, 0.0f, -20.0f));

	m_pScene = std::make_unique<CMenuScene>();
	m_pScene->SetCamera(m_pCamera);

	m_nProgramState = 메인메뉴;
	BuildObject();

	return true;
}

void CGameFramework::Release()
{
	::CloseHandle(m_hFenceEvent);
}

void CGameFramework::CreateDevice()
{
	::CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pdxgiFactory);
	IDXGIAdapter* pdxgiAdapter{ nullptr };
	for (UINT i = 0; m_pdxgiFactory->EnumAdapters(i, &pdxgiAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		if (SUCCEEDED(::D3D12CreateDevice(pdxgiAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_pd3dDevice)))
			break;
	}
	if (!pdxgiAdapter) {
		m_pdxgiFactory->EnumWarpAdapter(__uuidof(IDXGIAdapter), (void**)&pdxgiAdapter);
		::D3D12CreateDevice(pdxgiAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dFeatureMsaaQLevel;
	::ZeroMemory(&d3dFeatureMsaaQLevel, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	d3dFeatureMsaaQLevel.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dFeatureMsaaQLevel.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dFeatureMsaaQLevel.SampleCount = 4;
	d3dFeatureMsaaQLevel.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dFeatureMsaaQLevel, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevel = d3dFeatureMsaaQLevel.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevel > 1) ? true : false;

	m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_nFenceValue = 0;

	if (pdxgiAdapter)
		pdxgiAdapter->Release();
}

void CGameFramework::CreateCommandLAQ()
{
	m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);
	m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator.Get(), NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);

	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);

	m_pd3dCommandList->Close();
}

void CGameFramework::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffer;
	dxgiSwapChainDesc.BufferDesc.Width = FRAME_BUFFER_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = FRAME_BUFFER_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = FIXED_FRAME_RATE;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevel - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.Flags = 0;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	IDXGISwapChain* p = m_pdxgiSwapChain.Get();
	m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue.Get(), &dxgiSwapChainDesc, (IDXGISwapChain**)m_pdxgiSwapChain.GetAddressOf());

	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	m_nCurrentSwapChainIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

void CGameFramework::CreateRTVDSV()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDHD;
	::ZeroMemory(&d3dDHD, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDHD.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDHD.NodeMask = 0;
	d3dDHD.NumDescriptors = m_nSwapChainBuffer;
	d3dDHD.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	m_pd3dDevice->CreateDescriptorHeap(&d3dDHD, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRTVDescriptorHeap);
	m_nRTVIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDHD.NumDescriptors = 1;
	d3dDHD.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	m_pd3dDevice->CreateDescriptorHeap(&d3dDHD, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDSVDescriptorHeap);
	m_nDSVIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dCPUDescriptorHandle = m_pd3dRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffer; ++i) {
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_pd3dRTVBuffer[i]);
		m_pd3dDevice->CreateRenderTargetView(m_pd3dRTVBuffer[i].Get(), NULL, d3dCPUDescriptorHandle);
		d3dCPUDescriptorHandle.ptr += m_nRTVIncrementSize;
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
	d3dRD.MipLevels = 1;
	d3dRD.Width = FRAME_BUFFER_WIDTH;
	d3dRD.Height = FRAME_BUFFER_HEIGHT;
	d3dRD.Alignment = 0;
	d3dRD.DepthOrArraySize = 1;
	d3dRD.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	d3dRD.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dRD.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dRD.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevel - 1) : 0;

	D3D12_CLEAR_VALUE d3dCV;
	::ZeroMemory(&d3dCV, sizeof(D3D12_CLEAR_VALUE));
	d3dCV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dCV.DepthStencil.Depth = 1.0f;
	d3dCV.DepthStencil.Stencil = 0;

	m_pd3dDevice->CreateCommittedResource(&d3dHP, D3D12_HEAP_FLAG_NONE, &d3dRD, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dCV, __uuidof(ID3D12Resource), (void**)&m_pd3dDSVBuffer);
	d3dCPUDescriptorHandle = m_pd3dDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDSVBuffer.Get(), NULL, d3dCPUDescriptorHandle);
}

void CGameFramework::BuildObject()
{
	m_pd3dCommandAllocator->Reset();
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);

	m_pScene->BuildObject(m_pd3dDevice, m_pd3dCommandList);

	m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandList[]{ m_pd3dCommandList.Get()};
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandList);
	WaitForGPUComplete();
}


LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	switch (nMessage) {
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessage, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		OnProcessingMouseMessage(hWnd, nMessage, wParam, lParam);
		break;
	}
	return 0;
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	switch (nMessage) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case 'w':
		case 'W':
			//m_pCamera->forward = true;
			break;
		default:
			m_pScene->OnProcessingKeyboardMessage(hWnd, nMessage, wParam, lParam);
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam) {
		case 'w':
		case 'W':
			//m_pCamera->forward = false;
			break;
		default:
			m_pScene->OnProcessingKeyboardMessage(hWnd, nMessage, wParam, lParam);
			break;
		}
		break;
	}
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	switch (m_nProgramState) {
	case 메인메뉴:
		switch (nMessage) {
		case WM_LBUTTONDOWN:
			m_pCamera->UpdateProjMatrix();
			m_pScene.reset();
			m_pScene = std::make_unique<CIngameScene>();
			m_pScene->SetCamera(m_pCamera);
			BuildObject();
			m_pCamera->SetCameraEye(XMFLOAT3(0.0, 50.0, 0.0));
			m_nProgramState = 인게임;
			ClientCenter.x = 640; ClientCenter.y = 360;
			ClientToScreen(hWnd, &ClientCenter);
			SetCursorPos(ClientCenter.x, ClientCenter.y);
			break;
		}
		break;
	case 인게임:
		switch (nMessage) {
		case WM_MOUSEMOVE: {
			POINT currentCursor;
			GetCursorPos(&currentCursor);
			int cxDelta = currentCursor.x - ClientCenter.x;
			int cyDelta = currentCursor.y - ClientCenter.y;
			m_pCamera->Rotate(cxDelta, cyDelta);
			SetCursorPos(ClientCenter.x, ClientCenter.y);
			break;
		}
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			
			break;
		}
		break;
	}
}

void CGameFramework::WaitForGPUComplete()
{
	++m_nFenceValue;
	m_pd3dCommandQueue->Signal(m_pd3dFence.Get(), m_nFenceValue);
	if (m_pd3dFence->GetCompletedValue() < m_nFenceValue) {
		m_pd3dFence->SetEventOnCompletion(m_nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}

}

void CGameFramework::ProcessInput()
{
	UCHAR keyBuffer[256];
	GetKeyboardState(keyBuffer);

	if (keyBuffer['W'] & 0x80)
		m_pCamera->move();
}

void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(FIXED_FRAME_RATE);

	ProcessInput();

	m_pd3dCommandAllocator->Reset();
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Transition.pResource = m_pd3dRTVBuffer[m_nCurrentSwapChainIndex].Get();
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dCPUHandle = m_pd3dRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dCPUHandle.ptr += (m_nCurrentSwapChainIndex * m_nRTVIncrementSize);
	float color[4] = { 0.0, 0.0, 0.0, 1.0 };
	m_pd3dCommandList->ClearRenderTargetView(d3dCPUHandle, color, 0, NULL);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dCPUHandleD = m_pd3dDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->ClearDepthStencilView(d3dCPUHandleD, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pd3dCommandList->OMSetRenderTargets(1, &d3dCPUHandle, TRUE, &d3dCPUHandleD);
	//====================================
	//m_pCamera->move();
	//=====================================
	// 렌더링 코드
	m_pScene->Render(m_pd3dCommandList);


	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = {m_pd3dCommandList.Get()};
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGPUComplete();

	m_pdxgiSwapChain->Present(0, 0);

	m_nCurrentSwapChainIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 13, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}