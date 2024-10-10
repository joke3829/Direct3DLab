#include "Mesh.h"

void CMesh::ReleaseUploadBuffer()
{
	m_pd3dVertexUploadBuffer.Reset();
	m_pd3dIndexUploadBuffer.Reset();
}

void CMesh::Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->IASetVertexBuffers(0, 1, &m_d3dVertexBufferView);
	if (m_pd3dIndexBuffer) {
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
		pd3dCommandList->DrawInstanced(m_nVertices, 1, 0, 0);
}

//=======================================================================

CTexturedSqureMesh::CTexturedSqureMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, float width, float height)
{
	m_vVertices.reserve(6);
	m_vIndices.reserve(6);

	float hWidth = width / 2; float hHeight = height / 2;
	// 0왼쪽 위, 1 오른쪽 위, 2 오른쪽 아래, 3왼쪽 아래
	m_vVertices.push_back(TexturedVertex(XMFLOAT3(-hWidth, hHeight, 0.5f), XMFLOAT2(0.0f, 0.0f)));
	m_vVertices.push_back(TexturedVertex(XMFLOAT3(hWidth, hHeight, 0.5f), XMFLOAT2(1.0f, 0.0f)));
	m_vVertices.push_back(TexturedVertex(XMFLOAT3(hWidth, -hHeight, 0.5f), XMFLOAT2(1.0f, 1.0f)));
	m_vVertices.push_back(TexturedVertex(XMFLOAT3(-hWidth, -hHeight, 0.5f), XMFLOAT2(0.0f, 1.0f)));
	m_nVertices = m_vVertices.size();

	m_vIndices.push_back(0);
	m_vIndices.push_back(1);
	m_vIndices.push_back(3);
	m_vIndices.push_back(1);
	m_vIndices.push_back(2);
	m_vIndices.push_back(3);
	m_nIndices = m_vIndices.size();

	CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dVertexUploadBuffer, m_vVertices, m_pd3dVertexBuffer);
	CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dIndexUploadBuffer, m_vIndices, m_pd3dIndexBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexUploadBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.SizeInBytes = sizeof(TexturedVertex) * m_nVertices;
	m_d3dVertexBufferView.StrideInBytes = sizeof(TexturedVertex);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexUploadBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}