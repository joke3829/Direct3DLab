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

class CBillboardVertex : public Vertex {
public:
	CBillboardVertex(XMFLOAT3 xmf3Pos, XMFLOAT2 xmf2Size)
		: Vertex(xmf3Pos)
	{
		m_xmf2Size = xmf2Size;
	}
protected:
	XMFLOAT2 m_xmf2Size;	// x = width, y = height
};

class CParticleVertex : public Vertex {	// �� �������� �����Ѵ�.
public:
	CParticleVertex(XMFLOAT3 pos, XMFLOAT3 dir)
		: Vertex(pos)
	{
		m_xmf3Dir = dir;
		m_fSize = 0.0f;
		m_fLifeTime = 0.0f;
		m_nType = 0;
	}
protected:
	XMFLOAT3 m_xmf3Dir;	// ��ƼŬ�� ������ ����
	float m_fSize;
	float m_fLifeTime;
	UINT m_nType;	// 0: �߻��, 1: ����
};

//===========================================================================

class CMesh {
public:
	virtual void ReleaseUploadBuffer();

	BoundingOrientedBox& getOBB() { return m_OBB; }

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

	BoundingOrientedBox m_OBB = BoundingOrientedBox();

	XMFLOAT3 m_obbCenter;
	XMFLOAT3 m_obbExtent;
};

class CPlaneMesh : public CMesh {
public:
	CPlaneMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, float width = 1.0f, float height = 1.0f);
};

class CTexturedSquareMesh : public CMesh {
public:
	CTexturedSquareMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, float width = 1.0f, float height = 1.0f);
	CTexturedSquareMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, ���� arrow);
	CTexturedSquareMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, ���� arrow, float fSize);
	//void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
protected:
	std::vector<TexturedVertex> m_vVertices;
	std::vector<UINT> m_vIndices;
};

class CTexturedCubeMesh : public CMesh {
public:
	CTexturedCubeMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, XMFLOAT3 extent);
protected:
	std::vector<LightingTexturedVertex> m_vVertices;
	std::vector<UINT> m_vIndices;
};

class CTexturedNoLightingCubeMesh : public CMesh {
public:
	CTexturedNoLightingCubeMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, XMFLOAT3 extent);
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

	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
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

class CBillboardMesh : public CMesh {
public:
	CBillboardMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, XMFLOAT3 xmf3Center, XMFLOAT2 xmf2SIze);
private:
	std::vector<CBillboardVertex> m_vVertices;
};

class CParticleMesh : public CMesh {
public:
	CParticleMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);

	void Reset();

	void OnePathRender(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
	void TwoPathRender(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
	void PostRender(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
protected:
	bool m_bStart;
	UINT m_nMaxParticle;

	ComPtr<ID3D12Resource> m_pd3dStreamOutput;
	ComPtr<ID3D12Resource> m_pd3dDrawBuffer;

	ComPtr<ID3D12Resource> m_pFilledSizeBuffer;

	ComPtr<ID3D12Resource> m_pd3dReadBackBuffer;
	ComPtr<ID3D12Resource> m_pd3dUploadBuffer;
	UINT64* m_pMappedPointer;

	D3D12_STREAM_OUTPUT_BUFFER_VIEW m_d3dStreamOutputBufferView;
};