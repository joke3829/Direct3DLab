#include "Mesh.h"

void CMesh::ReleaseUploadBuffer()
{
	m_pd3dVertexUploadBuffer.Reset();
	m_pd3dIndexUploadBuffer.Reset();
}

void CMesh::Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
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

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

//================================================================================

CHeightMapImage::CHeightMapImage(const wchar_t* filePath, int nWidth, int nLength, XMFLOAT3 xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;
	std::unique_ptr<BYTE[]> pHeightMapPixels = std::make_unique<BYTE[]>(m_nWidth * m_nLength);
	HANDLE hFile = ::CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels.get(), (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	m_pHeightMapPixels = std::make_unique<BYTE[]>(m_nWidth * m_nLength);
	for (int z = 0; z < m_nLength; ++z) {
		for (int x = 0; x < m_nWidth; ++x) {
			m_pHeightMapPixels[x + ((m_nLength - 1 - z) * m_nWidth)] = pHeightMapPixels[x + (z * m_nWidth)];
		}
	}
}

float CHeightMapImage::GetHeight(int x, int z)
{
	//float f = m_pHeightMapPixels[x + (z * m_nWidth)];
	return m_pHeightMapPixels[x + (z * m_nWidth)];
}

CHeightMapGridMesh::CHeightMapGridMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::unique_ptr<CHeightMapImage>& pHeightMapImage)
{
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	XMFLOAT3 xmf3Scale = pHeightMapImage->m_xmf3Scale;
	int Width = pHeightMapImage->m_nWidth; int Length = pHeightMapImage->m_nLength;

	float fHeight;
	for (int z = 0; z < Length; ++z) {
		for (int x = 0; x < Width; ++x) {
			fHeight = pHeightMapImage->GetHeight(x, z);
			m_vVertices.push_back(TerrainVertex(XMFLOAT3(x * xmf3Scale.x, fHeight * xmf3Scale.y, z * xmf3Scale.z),
				XMFLOAT2(float(x)/ float(Width - 1), float(Length - 1 - z) / float(Length - 1)), 
				XMFLOAT2(float(x) / float(xmf3Scale.x * 0.5f), float(z) / float(xmf3Scale.z * 0.5f))));
		}
	}

	::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dVertexUploadBuffer, m_vVertices, m_pd3dVertexBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.SizeInBytes = sizeof(TerrainVertex) * m_vVertices.size();
	m_d3dVertexBufferView.StrideInBytes = sizeof(TerrainVertex);

	for (int z = 0; z < Length - 1; ++z) {
		if (z % 2 == 0) {
			for (int x = 0; x < Width; ++x) {
				if (x == 0 && z > 0)
					m_vIndices.push_back(UINT(x + (z * Width)));
				m_vIndices.push_back(UINT(x + (z * Width)));
				m_vIndices.push_back(UINT((x + (z * Width)) + Width));
			}
		}
		else {
			for (int x = Width - 1; x >= 0; --x) {
				if (x == (Width - 1))
					m_vIndices.push_back(UINT(x + (z * Width)));
				m_vIndices.push_back(UINT(x + (z * Width)));
				m_vIndices.push_back(UINT((x + (z * Width)) + Width));
			}
		}
	}

	::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dIndexUploadBuffer, m_vIndices, m_pd3dIndexBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_vIndices.size();

	m_nVertices = m_vVertices.size();
	m_nIndices = m_vIndices.size();
}