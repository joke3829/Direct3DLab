#pragma once
#include "stdafx.h"

//================정점 정보들=============================================================================
class CVertex {
public:
	CVertex() : m_xmf3Position(XMFLOAT3(0.0f, 0.0f, 0.0f)) {}
	CVertex(XMFLOAT3 xmf3position) : m_xmf3Position(xmf3position) {}
	CVertex(float x, float y, float z) : m_xmf3Position(XMFLOAT3(x, y, z)) {}

	const XMFLOAT3 getPosition();
	void SetPosition(XMFLOAT3 xmf3Position);
 protected:
	XMFLOAT3 m_xmf3Position;
};

class CDiffusedVertex : public CVertex {
public:
	CDiffusedVertex() : CVertex() {}
	CDiffusedVertex(XMFLOAT3 xmf3Position) : CVertex(xmf3Position) {}
	CDiffusedVertex(float x, float y, float z) : CVertex(x, y, z) {}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Color) : CVertex(xmf3Position) { m_xmf4Color = xmf4Color; }

	const XMFLOAT4 getColor();
	void setColor(XMFLOAT4 xmf4Color);
protected:
	XMFLOAT4 m_xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
};

class CLightingDiffusedVertex : public CDiffusedVertex {
	
};

//=====================================================================================================

class CMesh {
public:
	void ReleaseUploadBuffer();

	virtual void Render() {}
protected:
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	ComPtr<ID3D12Resource> m_pd3dVertexBuffer{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dVertexUploadBuffer{ nullptr };
	//ID3D12Resource* m_pd3dVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;

	ComPtr<ID3D12Resource> m_pd3dIndexBuffer{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dIndexUploadBuffer{ nullptr };
	//ID3D12Resource* m_pd3dIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;
};

class CDiffusedSquareMesh : public CMesh {
public:
	CDiffusedSquareMesh(ComPtr<ID3D12Device>& pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, EnumCOLOR eColor = RED, float width = 5.0f, float height = 5.0f);

	const size_t getVtxNum();	// 점의 개수를 반환

	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
private:
	std::vector<CDiffusedVertex> m_vVertices;
	std::vector<UINT> m_vIndices;
};

