//==========================================
// Mesh.h
// �����̳� �޽��� ������ ��Ƶ� ���
//==========================================
#pragma once
#include "stdafx.h"

class Vertex {	// ��ǥ�� ���� ����
public:
	Vertex(XMFLOAT3 xmf3pos) : m_xmf3Position{ xmf3pos } {};	// ������ ���ÿ� �ʱ�ȭ
protected:
	XMFLOAT3 m_xmf3Position;		// ����
};

class LightingVertex : public Vertex {	// ����� ���� ����
public:
	LightingVertex(XMFLOAT3 xmf3pos, XMFLOAT3 xmf3nor) : Vertex(xmf3pos)
	{
		m_xmf3Normal = xmf3nor;
	};
protected:
	XMFLOAT3 m_xmf3Normal;
};


class LightingTexturedVertex : public LightingVertex {	// normal�� uv�� ���� ����
public:
	LightingTexturedVertex(XMFLOAT3 xmf3Pos, XMFLOAT3 xmf3nor, XMFLOAT2 xmf2uv)
		: LightingVertex(xmf3Pos, xmf3nor)
	{
		m_xmf2UV = xmf2uv;
	}
protected:
	XMFLOAT2 m_xmf2UV;
};

class TexturedVertex : public Vertex {	// pos, uv
public:
	TexturedVertex(XMFLOAT3 xmf3pos, XMFLOAT2 xmf2uv)
		: Vertex(xmf3pos)
	{
		m_xmf2uv = xmf2uv;
	}
protected:
	XMFLOAT2 m_xmf2uv;
};

//===========================================================================

class CMesh {
public:
	virtual void ReleaseUploadBuffer();

	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
protected:
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	ComPtr<ID3D12Resource> m_pd3dVertexBuffer{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dVertexUploadBuffer{ nullptr };
	UINT m_nVertices;

	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;
	ComPtr<ID3D12Resource> m_pd3dIndexBuffer{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dIndexUploadBuffer{ nullptr };
	UINT m_nIndices;

	// �ٿ�� �ڽ� �߰� �ٶ�
};

class CTexturedSqureMesh : public CMesh {
public:
	CTexturedSqureMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, float width = 1.0f, float height = 1.0f);

	//void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
protected:
	std::vector<TexturedVertex> m_vVertices;
	std::vector<UINT> m_vIndices;
};