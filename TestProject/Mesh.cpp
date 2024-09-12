#include "Mesh.h"

//============================================================================
const XMFLOAT3 CVertex::getPosition()
{
	return m_xmf3Position;
}

void CVertex::SetPosition(XMFLOAT3 xmf3Position)
{
	m_xmf3Position = xmf3Position;
}


const XMFLOAT4 CDiffusedVertex::getColor()
{
	return m_xmf4Color;
}

void CDiffusedVertex::setColor(XMFLOAT4 xmf4Color)
{
	m_xmf4Color = xmf4Color;
}

//===========================================================================================

void CMesh::ReleaseUploadBuffer()
{
	if (m_pd3dVertexUploadBuffer)
		m_pd3dVertexUploadBuffer.Reset();
	if (m_pd3dIndexUploadBuffer)
		m_pd3dIndexUploadBuffer.Reset();
}

CDiffusedSquareMesh::CDiffusedSquareMesh(ComPtr<ID3D12Device>& pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, EnumCOLOR eColor, float width, float height)
{
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	XMFLOAT4 xmf4Color;
	switch (eColor) {
	case RED:
		xmf4Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		break;
	case GREEN:
		xmf4Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		break;
	case BLUE:
		xmf4Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		break;
	default:
		xmf4Color = XMFLOAT4(1.0f, 0.25f, 0.75f, 1.0f);
		break;
	}
	float halfWIdth = width / 2;
	float halfHeight = height / 2;
	m_vVertices.push_back(CDiffusedVertex(XMFLOAT3(halfWIdth, halfHeight, 0), xmf4Color));
	m_vVertices.push_back(CDiffusedVertex(XMFLOAT3(halfWIdth, -halfHeight, 0), xmf4Color));
	m_vVertices.push_back(CDiffusedVertex(XMFLOAT3(-halfWIdth, -halfHeight, 0), xmf4Color));
	m_vVertices.push_back(CDiffusedVertex(XMFLOAT3(-halfWIdth, halfHeight, 0), xmf4Color));

	// 0: 오른쪽 위, 1:오른쪽 아래, 2:왼쪽 아래, 3:왼쪽 위
	m_vIndices.push_back(0);
	m_vIndices.push_back(1);
	m_vIndices.push_back(2);
	m_vIndices.push_back(0);
	m_vIndices.push_back(2);
	m_vIndices.push_back(3);

	::CreateVertexBuffer(pd3dDevice, pd3dCommandList, m_pd3dVertexBuffer, m_vVertices, m_pd3dVertexUploadBuffer);
	::CreateVertexBuffer(pd3dDevice, pd3dCommandList, m_pd3dIndexBuffer, m_vIndices, m_pd3dIndexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.SizeInBytes = sizeof(CDiffusedVertex) * m_vVertices.size();
	m_d3dVertexBufferView.StrideInBytes = sizeof(CDiffusedVertex);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_vIndices.size();
}

const size_t CDiffusedSquareMesh::getVtxNum()
{
	return m_vVertices.size();
}

void CDiffusedSquareMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(0, 1, &m_d3dVertexBufferView);
	if (m_pd3dIndexBuffer) {
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_vIndices.size(), 1, 0, 0, 0);
	}
	else
		pd3dCommandList->DrawInstanced(m_vVertices.size(), 1, 0, 0);
}