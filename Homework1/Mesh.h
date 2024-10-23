//==========================================
// Mesh.h
// �����̳� �޽��� ������ ��Ƶ� ���
//==========================================
#pragma once
#include "stdafx.h"

enum ���� {
	��, ��, ��, ��, ��, ��
};

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

class TerrainVertex : public Vertex {
public:
	TerrainVertex(XMFLOAT3 xmf3pos, XMFLOAT2 uv1, XMFLOAT2 uv2)
		: Vertex(xmf3pos)
	{
		m_xmf2uv1 = uv1;
		m_xmf2uv2 = uv2;
	}
protected:
	XMFLOAT2 m_xmf2uv1;
	XMFLOAT2 m_xmf2uv2;
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

	D3D_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;
	// �ٿ�� �ڽ� �߰� �ٶ�

	XMFLOAT3 m_obbCenter;
	XMFLOAT3 m_obbExtent;
};

class CTexturedSquareMesh : public CMesh {
public:
	CTexturedSquareMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, float width = 1.0f, float height = 1.0f);
	CTexturedSquareMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, ���� arrow);

	//void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
protected:
	std::vector<TexturedVertex> m_vVertices;
	std::vector<UINT> m_vIndices;
};

//=============================================================================

class CHeightMapImage {
public:
	CHeightMapImage(const wchar_t* filePath, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	float GetHeight(int x, int z);

	std::unique_ptr<BYTE[]> m_pHeightMapPixels;

	int m_nWidth;
	int m_nLength;

	XMFLOAT3 m_xmf3Scale;
};

class CHeightMapGridMesh : public CMesh {
public:
	CHeightMapGridMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::unique_ptr<CHeightMapImage>& pHeightMapImage);
private:
	std::vector<TerrainVertex> m_vVertices;
	std::vector<UINT> m_vIndices;
};


class HMesh : public CMesh {
public:
	HMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::ifstream& inFile);
protected:
	// tex0
	D3D12_VERTEX_BUFFER_VIEW m_d3dTex0BufferView;
	ComPtr<ID3D12Resource> m_pd3dTex0Buffer{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dTex0UploadBuffer{ nullptr };
	UINT m_nTex0;
	// �븻
	D3D12_VERTEX_BUFFER_VIEW m_d3dNormalBufferView;
	ComPtr<ID3D12Resource> m_pd3dNormalBuffer{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dNormalUploadBuffer{ nullptr };
	UINT m_nNormal;
	// ź��Ʈ
	D3D12_VERTEX_BUFFER_VIEW m_d3dTangentBufferView;
	ComPtr<ID3D12Resource> m_pd3dTangentBuffer{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dTangentUploadBuffer{ nullptr };
	UINT m_nTangent;
	// ����ź��Ʈ
	D3D12_VERTEX_BUFFER_VIEW m_d3dBiTangentBufferView;
	ComPtr<ID3D12Resource> m_pd3dBiTangentBuffer{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dBiTangentUploadBuffer{ nullptr };
	UINT m_nBiTangent;
	
	// SubMeshes(�ε��� ����)
};