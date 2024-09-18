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
#include <vector>
#include <memory>

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

enum EnumCOLOR{RED, GREEN, BLUE};

template<class T>
void CreateVertexBuffer(ComPtr<ID3D12Device>& pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ComPtr<ID3D12Resource>& pd3dVertexBuffer, std::vector<T>& vData,
	ComPtr<ID3D12Resource>& pd3dVertexUploadBuffer = nullptr)
{
	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.Width = sizeof(T) * vData.size();
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;

	pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST,
		NULL, __uuidof(ID3D12Resource), (void**)&pd3dVertexBuffer);

	//if (pd3dVertexUploadBuffer) {
		d3dHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)&pd3dVertexUploadBuffer);

		D3D12_RANGE d3dReadRange = { 0, 0 };
		T* pBufferDataBegin = NULL;
		HRESULT hr = pd3dVertexUploadBuffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
		::memcpy(pBufferDataBegin, vData.data(), sizeof(T) * vData.size());
		pd3dVertexUploadBuffer->Unmap(0, NULL);

		pd3dCommandList->CopyResource(pd3dVertexBuffer.Get(), pd3dVertexUploadBuffer.Get());

		D3D12_RESOURCE_BARRIER d3dResourceBarrier;
		::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
		d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dResourceBarrier.Transition.pResource = pd3dVertexBuffer.Get();
		d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	//}
}

template<class T>
void CreateUploadBuffer(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12Resource>& pd3dResource, T& Tdata)
{
	D3D12_HEAP_PROPERTIES d3dHeapPro;
	::ZeroMemory(&d3dHeapPro, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPro.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPro.CreationNodeMask = 1;
	d3dHeapPro.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPro.Type = D3D12_HEAP_TYPE_UPLOAD;
	d3dHeapPro.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Width = ((sizeof(Tdata) + 255) & ~255);
	HRESULT result = pd3dDevice->CreateCommittedResource(&d3dHeapPro, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL, __uuidof(ID3D12Resource), (void**)&pd3dResource);
}