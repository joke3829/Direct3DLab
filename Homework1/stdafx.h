// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <string>
#include <wrl.h>
#include <shellapi.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>

#include <dxgidebug.h>

#include <timeapi.h>
#include <array>
#include <vector>
#include <memory>
#include <fstream>

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d3dcompiler.lib");
#pragma comment(lib, "d3d12.lib");
#pragma comment(lib, "dxgi.lib");

#pragma comment(lib, "dxguid.lib");

#pragma comment(lib, "Winmm.lib");

#define FRAME_BUFFER_WIDTH 1280
#define FRAME_BUFFER_HEIGHT 720

#define FIXED_FRAME_RATE 60



// 버텍스 버퍼용 함수
template<class T>
void CreateBufferResource(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, ComPtr<ID3D12Resource>& pd3dResource, std::vector<T>& vData, ComPtr<ID3D12Resource>& pd3dDefaultResource)
{
	D3D12_HEAP_PROPERTIES d3dHP;
	::ZeroMemory(&d3dHP, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHP.Type = D3D12_HEAP_TYPE_UPLOAD;
	d3dHP.VisibleNodeMask = 1;
	d3dHP.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHP.CreationNodeMask = 1;
	d3dHP.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;

	D3D12_RESOURCE_DESC d3dRD;
	::ZeroMemory(&d3dRD, sizeof(D3D12_RESOURCE_DESC));
	d3dRD.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3dRD.Format = DXGI_FORMAT_UNKNOWN;
	d3dRD.MipLevels = 1;
	d3dRD.Alignment = 0;
	d3dRD.DepthOrArraySize = 1;
	d3dRD.Flags = D3D12_RESOURCE_FLAG_NONE;
	d3dRD.Height = 1;
	d3dRD.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dRD.Width = sizeof(T) * vData.size();
	d3dRD.SampleDesc.Count = 1;
	d3dRD.SampleDesc.Quality = 0;

	
	pd3dDevice->CreateCommittedResource(&d3dHP, D3D12_HEAP_FLAG_NONE, &d3dRD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)pd3dResource.GetAddressOf());

	d3dHP.Type = D3D12_HEAP_TYPE_DEFAULT;
	pd3dDevice->CreateCommittedResource(&d3dHP, D3D12_HEAP_FLAG_NONE, &d3dRD, D3D12_RESOURCE_STATE_COPY_DEST, NULL, __uuidof(ID3D12Resource), (void**)pd3dDefaultResource.GetAddressOf());
	T* pDataBegin = NULL;
	D3D12_RANGE d3dRange{ 0, 0 };
	pd3dResource->Map(0, &d3dRange, (void**)&pDataBegin);
	::memcpy(pDataBegin, vData.data(), sizeof(T) * vData.size());
	pd3dResource->Unmap(0, NULL);

	pd3dCommandList->CopyResource(pd3dDefaultResource.Get(), pd3dResource.Get());
	D3D12_RESOURCE_BARRIER d3dRB;
	::ZeroMemory(&d3dRB, sizeof(D3D12_RESOURCE_BARRIER));
	d3dRB.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dRB.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dRB.Transition.pResource = pd3dDefaultResource.Get();
	d3dRB.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	d3dRB.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	d3dRB.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

	pd3dCommandList->ResourceBarrier(1, &d3dRB);
}

// 업로드 버퍼용 함수
template<class T>
void CreateBufferResource(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12Resource>& pd3dResource, T& TData)
{
	D3D12_HEAP_PROPERTIES d3dHP;
	::ZeroMemory(&d3dHP, sizeof(D3D12_HEAP_PROPERTIES));

	d3dHP.Type = D3D12_HEAP_TYPE_UPLOAD;
	d3dHP.VisibleNodeMask = 1;
	d3dHP.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHP.CreationNodeMask = 1;
	d3dHP.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;

	D3D12_RESOURCE_DESC d3dRD;
	::ZeroMemory(&d3dRD, sizeof(D3D12_RESOURCE_DESC));
	d3dRD.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3dRD.Format = DXGI_FORMAT_UNKNOWN;
	d3dRD.MipLevels = 1;
	d3dRD.Alignment = 0;
	d3dRD.DepthOrArraySize = 1;
	d3dRD.Flags = D3D12_RESOURCE_FLAG_NONE;
	d3dRD.Height = 1;
	d3dRD.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dRD.Width = (sizeof(TData) + 255) & ~255;
	d3dRD.SampleDesc.Count = 1;
	d3dRD.SampleDesc.Quality = 0;


	HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHP, D3D12_HEAP_FLAG_NONE, &d3dRD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)pd3dResource.GetAddressOf());
}

void CreateBufferResourceS(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12Resource>& pd3dResource, UINT64 nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceState);

enum eDirection { DIR_FORWARD, DIR_BACK, DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN };
