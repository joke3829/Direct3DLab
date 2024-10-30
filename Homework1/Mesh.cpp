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

CTexturedSquareMesh::CTexturedSquareMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, float width, float height)
{
	m_vVertices.reserve(4);
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

CTexturedSquareMesh::CTexturedSquareMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, 방향 arrow)
{
	m_vVertices.reserve(4);
	m_vIndices.reserve(6);

	// 0왼쪽 위, 1 오른쪽 위, 2 오른쪽 아래, 3왼쪽 아래
	switch (arrow) {
	case 좌:
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, 512.0f, -512.0f), XMFLOAT2(0.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, 512.0f, 512.0f), XMFLOAT2(1.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, -512.0f, 512.0f), XMFLOAT2(1.0f, 1.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, -512.0, -512.0f), XMFLOAT2(0.0f, 1.0f)));
		break;
	case 우:
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, 512.0f, 512.0f), XMFLOAT2(0.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, 512.0f, -512.0f), XMFLOAT2(1.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, -512.0f, -512.0f), XMFLOAT2(1.0f, 1.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, -512.0, 512.0f), XMFLOAT2(0.0f, 1.0f)));
		break;
	case 앞:
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, 512.0f, 512.0f), XMFLOAT2(0.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, 512.0f, 512.0f), XMFLOAT2(1.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, -512.0f, 512.0f), XMFLOAT2(1.0f, 1.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, -512.0, 512.0f), XMFLOAT2(0.0f, 1.0f)));
		break;
	case 뒤:
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, 512.0f, -512.0f), XMFLOAT2(0.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, 512.0f, -512.0f), XMFLOAT2(1.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, -512.0f, -512.0f), XMFLOAT2(1.0f, 1.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, -512.0, -512.0f), XMFLOAT2(0.0f, 1.0f)));
		break;
	case 상:
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, 512.0f, -512.0f), XMFLOAT2(0.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, 512.0f, -512.0f), XMFLOAT2(1.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, 512.0f, 512.0f), XMFLOAT2(1.0f, 1.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, 512.0, 512.0f), XMFLOAT2(0.0f, 1.0f)));
		break;
	case 하:
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, -512.0f, 512.0f), XMFLOAT2(0.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, -512.0f, 512.0f), XMFLOAT2(1.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, -512.0f, -512.0f), XMFLOAT2(1.0f, 1.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, -512.0, -512.0f), XMFLOAT2(0.0f, 1.0f)));
		break;
	}
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

//=========================================================================

HMesh::HMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::ifstream& inFile)
{
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	char nStrLength{};
	std::string str;

	UINT nFactor{};	// 해당 요소의 개수
	inFile.read((char*)&nFactor, sizeof(int));

	// 메시의 이름
	inFile.read(&nStrLength, sizeof(char));
	str.assign(nStrLength, ' ');
	inFile.read(str.data(), nStrLength);

	// 여기서부터
	while (1) {
		inFile.read(&nStrLength, sizeof(char));
		str.assign(nStrLength, ' ');
		inFile.read(str.data(), nStrLength);
		if ("<Bounds>:" == str) {
			inFile.read((char*)&m_obbCenter, sizeof(XMFLOAT3));
			inFile.read((char*)&m_obbExtent, sizeof(XMFLOAT3));
			m_OBB = BoundingOrientedBox(m_obbCenter, m_obbExtent, XMFLOAT4(0.0, 0.0, 0.0, 1.0));
		}
		else if ("<Positions>:" == str) {
			inFile.read((char*)&nFactor, sizeof(int));
			std::vector<XMFLOAT3> positions;
			positions.assign(nFactor, XMFLOAT3(0.0, 0.0, 0.0));
			inFile.read((char*)positions.data(), sizeof(XMFLOAT3) * nFactor);
			CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dVertexUploadBuffer, positions, m_pd3dVertexBuffer);
			m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
			m_d3dVertexBufferView.SizeInBytes = sizeof(XMFLOAT3) * nFactor;
			m_d3dVertexBufferView.StrideInBytes = sizeof(XMFLOAT3);
			m_nVertices = nFactor;
		}
		else if ("<Colors>:" == str) {
			inFile.read((char*)&nFactor, sizeof(int));
		}
		else if ("<TextureCoords0>:" == str) {
			inFile.read((char*)&nFactor, sizeof(int));
			std::vector<XMFLOAT2> uv;
			uv.assign(nFactor, XMFLOAT2(0.0, 0.0));
			inFile.read((char*)uv.data(), sizeof(XMFLOAT2) * nFactor);
			CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dTex0UploadBuffer, uv, m_pd3dTex0Buffer);
			m_d3dTex0BufferView.BufferLocation = m_pd3dTex0Buffer->GetGPUVirtualAddress();
			m_d3dTex0BufferView.SizeInBytes = sizeof(XMFLOAT2) * nFactor;
			m_d3dTex0BufferView.StrideInBytes = sizeof(XMFLOAT2);
			m_nTex0 = nFactor;
		}
		else if ("<TextureCoords1>:" == str) {
			inFile.read((char*)&nFactor, sizeof(int));
		}
		else if ("<Normals>:" == str) {
			inFile.read((char*)&nFactor, sizeof(int));
			std::vector<XMFLOAT3> normals;
			normals.assign(nFactor, XMFLOAT3(0.0, 0.0, 0.0));
			inFile.read((char*)normals.data(), sizeof(XMFLOAT3) * nFactor);
			CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dNormalUploadBuffer, normals, m_pd3dNormalBuffer);
			m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
			m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * nFactor;
			m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
			m_nNormal = nFactor;
		}
		else if ("<Tangents>:" == str) {
			inFile.read((char*)&nFactor, sizeof(int));
			std::vector<XMFLOAT3> tang;
			tang.assign(nFactor, XMFLOAT3(0.0, 0.0, 0.0));
			inFile.read((char*)tang.data(), sizeof(XMFLOAT3) * nFactor);
			CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dTangentUploadBuffer, tang, m_pd3dTangentBuffer);
			m_d3dTangentBufferView.BufferLocation = m_pd3dTangentBuffer->GetGPUVirtualAddress();
			m_d3dTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * nFactor;
			m_d3dTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
			m_nTangent = nFactor;
		}
		else if ("<BiTangents>:" == str) {
			inFile.read((char*)&nFactor, sizeof(int));
			std::vector<XMFLOAT3> bitang;
			bitang.assign(nFactor, XMFLOAT3(0.0, 0.0, 0.0));
			inFile.read((char*)bitang.data(), sizeof(XMFLOAT3) * nFactor);
			CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dBiTangentUploadBuffer, bitang, m_pd3dBiTangentBuffer);
			m_d3dBiTangentBufferView.BufferLocation = m_pd3dBiTangentBuffer->GetGPUVirtualAddress();
			m_d3dBiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * nFactor;
			m_d3dBiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
			m_nBiTangent = nFactor;
		}
		else if ("<SubMeshes>:" == str) {
			inFile.read((char*)&nFactor, sizeof(int));

			inFile.read(&nStrLength, sizeof(char));
			str.assign(nStrLength, ' ');
			inFile.read(str.data(), nStrLength);

			inFile.read((char*)&nFactor, sizeof(int));
			// 인덱스 개수
			inFile.read((char*)&nFactor, sizeof(int));

			std::vector<UINT> index;
			index.assign(nFactor, 0);
			inFile.read((char*)index.data(), sizeof(UINT) * nFactor);
			CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dIndexUploadBuffer, index, m_pd3dIndexBuffer);
			m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
			m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
			m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * nFactor;
			m_nIndices = nFactor;
		}
		else if ("</Mesh>") {
			break;
		}
	}
}

void HMesh::Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	D3D12_VERTEX_BUFFER_VIEW d3dBuffers[5] =
	{ m_d3dVertexBufferView, m_d3dTex0BufferView, m_d3dNormalBufferView, m_d3dTangentBufferView, m_d3dBiTangentBufferView };
	pd3dCommandList->IASetVertexBuffers(0, 5, d3dBuffers);
	if (m_pd3dIndexBuffer) {
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
		pd3dCommandList->DrawInstanced(m_nVertices, 1, 0, 0);
}