#include "stdafx.h"

void CreateBufferResourceS(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12Resource>& pd3dResource, UINT64 nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceState)
{
	D3D12_HEAP_PROPERTIES d3dHP;
	::ZeroMemory(&d3dHP, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHP.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHP.CreationNodeMask = 1;
	d3dHP.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHP.Type = d3dHeapType;
	d3dHP.VisibleNodeMask = 1;

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
	d3dRD.SampleDesc.Count = 1;
	d3dRD.SampleDesc.Quality = 0;
	d3dRD.Width = nBytes;

	pd3dDevice->CreateCommittedResource(&d3dHP, D3D12_HEAP_FLAG_NONE, &d3dRD, d3dResourceState, NULL, __uuidof(ID3D12Resource), (void**)pd3dResource.GetAddressOf());
}