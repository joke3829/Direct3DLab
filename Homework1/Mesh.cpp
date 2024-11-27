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
	// 0���� ��, 1 ������ ��, 2 ������ �Ʒ�, 3���� �Ʒ�
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

CTexturedSquareMesh::CTexturedSquareMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, ���� arrow)
{
	m_vVertices.reserve(4);
	m_vIndices.reserve(6);

	// 0���� ��, 1 ������ ��, 2 ������ �Ʒ�, 3���� �Ʒ�
	switch (arrow) {
	case ��:
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, 512.0f, -512.0f), XMFLOAT2(0.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, 512.0f, 512.0f), XMFLOAT2(1.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, -512.0f, 512.0f), XMFLOAT2(1.0f, 1.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, -512.0, -512.0f), XMFLOAT2(0.0f, 1.0f)));
		break;
	case ��:
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, 512.0f, 512.0f), XMFLOAT2(0.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, 512.0f, -512.0f), XMFLOAT2(1.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, -512.0f, -512.0f), XMFLOAT2(1.0f, 1.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, -512.0, 512.0f), XMFLOAT2(0.0f, 1.0f)));
		break;
	case ��:
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, 512.0f, 512.0f), XMFLOAT2(0.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, 512.0f, 512.0f), XMFLOAT2(1.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, -512.0f, 512.0f), XMFLOAT2(1.0f, 1.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, -512.0, 512.0f), XMFLOAT2(0.0f, 1.0f)));
		break;
	case ��:
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, 512.0f, -512.0f), XMFLOAT2(0.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, 512.0f, -512.0f), XMFLOAT2(1.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, -512.0f, -512.0f), XMFLOAT2(1.0f, 1.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, -512.0, -512.0f), XMFLOAT2(0.0f, 1.0f)));
		break;
	case ��:
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, 512.0f, -512.0f), XMFLOAT2(0.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, 512.0f, -512.0f), XMFLOAT2(1.0f, 0.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(512.0f, 512.0f, 512.0f), XMFLOAT2(1.0f, 1.0f)));
		m_vVertices.push_back(TexturedVertex(XMFLOAT3(-512.0f, 512.0, 512.0f), XMFLOAT2(0.0f, 1.0f)));
		break;
	case ��:
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

//====================================================

CTexturedCubeMesh::CTexturedCubeMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, XMFLOAT3 extent)
{
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_OBB = BoundingOrientedBox(XMFLOAT3(0.0, 0.0, 0.0), extent, XMFLOAT4(0.0, 0.0, 0.0, 1.0));
	m_vVertices.reserve(24);
	float x = extent.x;
	float y = extent.y;
	float z = extent.z;
	// -z
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(-x, y, -z), XMFLOAT3(0.0, 0.0, -1.0), XMFLOAT2(0.0, 0.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(x, y, -z), XMFLOAT3(0.0, 0.0, -1.0), XMFLOAT2(1.0, 0.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(x, -y, -z), XMFLOAT3(0.0, 0.0, -1.0), XMFLOAT2(1.0, 1.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(-x, -y, -z), XMFLOAT3(0.0, 0.0, -1.0), XMFLOAT2(0.0, 1.0)));
	// x
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(x, y, -z), XMFLOAT3(1.0, 0.0, 0.0), XMFLOAT2(0.0, 0.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(x, y, z), XMFLOAT3(1.0, 0.0, 0.0), XMFLOAT2(1.0, 0.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(x, -y, z), XMFLOAT3(1.0, 0.0, 0.0), XMFLOAT2(1.0, 1.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(x, -y, -z), XMFLOAT3(1.0, 0.0, 0.0), XMFLOAT2(0.0, 1.0)));
	// z
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(x, y, z), XMFLOAT3(0.0, 0.0, 1.0), XMFLOAT2(0.0, 0.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(-x, y, z), XMFLOAT3(0.0, 0.0, 1.0), XMFLOAT2(1.0, 0.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(-x, -y, z), XMFLOAT3(0.0, 0.0, 1.0), XMFLOAT2(1.0, 1.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(x, -y, z), XMFLOAT3(0.0, 0.0, 1.0), XMFLOAT2(0.0, 1.0)));
	// -x
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(-x, y, z), XMFLOAT3(-1.0, 0.0, 0.0), XMFLOAT2(0.0, 0.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(-x, y, -z), XMFLOAT3(-1.0, 0.0, 0.0), XMFLOAT2(1.0, 0.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(-x, -y, -z), XMFLOAT3(-1.0, 0.0, 0.0), XMFLOAT2(1.0, 1.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(-x, -y, z), XMFLOAT3(-1.0, 0.0, 0.0), XMFLOAT2(0.0, 1.0)));
	// y
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(-x, y, z), XMFLOAT3(0.0, 1.0, 0.0), XMFLOAT2(0.0, 0.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(x, y, z), XMFLOAT3(0.0, 1.0, 0.0), XMFLOAT2(1.0, 0.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(x, y, -z), XMFLOAT3(0.0, 1.0, 0.0), XMFLOAT2(1.0, 1.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(-x, y, -z), XMFLOAT3(0.0, 1.0, 0.0), XMFLOAT2(0.0, 1.0)));
	//-y
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(-x, -y, -z), XMFLOAT3(0.0, -1.0, 0.0), XMFLOAT2(0.0, 0.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(x, -y, -z), XMFLOAT3(0.0, -1.0, 0.0), XMFLOAT2(1.0, 0.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(x, -y, z), XMFLOAT3(0.0, -1.0, 0.0), XMFLOAT2(1.0, 1.0)));
	m_vVertices.push_back(LightingTexturedVertex(XMFLOAT3(-x, -y, z), XMFLOAT3(0.0, -1.0, 0.0), XMFLOAT2(0.0, 1.0)));

	m_nVertices = m_vVertices.size();
	m_vIndices.reserve(36);
	// 0~3 -z
	m_vIndices.push_back(0);
	m_vIndices.push_back(1);
	m_vIndices.push_back(2);
	m_vIndices.push_back(0);
	m_vIndices.push_back(2);
	m_vIndices.push_back(3);
	// 4~7 x
	m_vIndices.push_back(4);
	m_vIndices.push_back(5);
	m_vIndices.push_back(6);
	m_vIndices.push_back(4);
	m_vIndices.push_back(6);
	m_vIndices.push_back(7);
	// 8~11 z
	m_vIndices.push_back(8);
	m_vIndices.push_back(9);
	m_vIndices.push_back(10);
	m_vIndices.push_back(8);
	m_vIndices.push_back(10);
	m_vIndices.push_back(11);
	// 12~15 -x
	m_vIndices.push_back(12);
	m_vIndices.push_back(13);
	m_vIndices.push_back(14);
	m_vIndices.push_back(12);
	m_vIndices.push_back(14);
	m_vIndices.push_back(15);
	// 16~19 y
	m_vIndices.push_back(16);
	m_vIndices.push_back(17);
	m_vIndices.push_back(18);
	m_vIndices.push_back(16);
	m_vIndices.push_back(18);
	m_vIndices.push_back(19);
	// 20~23 -y
	m_vIndices.push_back(20);
	m_vIndices.push_back(21);
	m_vIndices.push_back(22);
	m_vIndices.push_back(20);
	m_vIndices.push_back(22);
	m_vIndices.push_back(23);
	m_nIndices = m_vIndices.size();

	::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dVertexUploadBuffer, m_vVertices, m_pd3dVertexBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.SizeInBytes = sizeof(LightingTexturedVertex) * m_nVertices;
	m_d3dVertexBufferView.StrideInBytes = sizeof(LightingTexturedVertex);

	::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dIndexUploadBuffer, m_vIndices, m_pd3dIndexBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

//=========================================================================

HMesh::HMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::ifstream& inFile)
{
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	char nStrLength{};
	std::string str;

	UINT nFactor{};	// �ش� ����� ����
	inFile.read((char*)&nFactor, sizeof(int));

	// �޽��� �̸�
	inFile.read(&nStrLength, sizeof(char));
	str.assign(nStrLength, ' ');
	inFile.read(str.data(), nStrLength);

	// ���⼭����
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
			// �ε��� ����
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

CBillboardMesh::CBillboardMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, XMFLOAT3 xmf3Center, XMFLOAT2 xmf2SIze)
{
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_OBB = BoundingOrientedBox();

	m_vVertices.push_back(CBillboardVertex(xmf3Center, xmf2SIze));
	m_nVertices = m_vVertices.size();
	::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dVertexUploadBuffer, m_vVertices, m_pd3dVertexBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.SizeInBytes = m_nVertices * sizeof(CBillboardVertex);
	m_d3dVertexBufferView.StrideInBytes = sizeof(CBillboardVertex);
}

//============================================================

CParticleMesh::CParticleMesh(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	std::vector<CParticleVertex> m_vVertices;
	m_vVertices.push_back(CParticleVertex(XMFLOAT3(0.0, 0.0, 0.0), XMFLOAT3(0.0, 0.0, 0.0)));
	::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dVertexUploadBuffer, m_vVertices, m_pd3dVertexBuffer);

	m_bStart = true;
	m_nVertices = 1;
	m_nMaxParticle = 100;
	
	::CreateBufferResource(pd3dDevice, m_pd3dStreamOutput, sizeof(CParticleVertex) * m_nMaxParticle, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_STREAM_OUT);
	::CreateBufferResource(pd3dDevice, m_pd3dDrawBuffer, sizeof(CParticleVertex) * m_nMaxParticle, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	::CreateBufferResource(pd3dDevice, m_pd3dReadBackBuffer, sizeof(UINT64), D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_STATE_COPY_DEST);
	::CreateBufferResource(pd3dDevice, m_pd3dUploadBuffer, sizeof(UINT64), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_COPY_SOURCE);
	m_pd3dUploadBuffer->Map(0, NULL, (void**)&m_pMappedPointer);
}

void CParticleMesh::Reset()
{
	m_bStart = true;
}

void CParticleMesh::OnePathRender(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	if (m_bStart) {
		m_nVertices = 1;
		m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
		m_d3dVertexBufferView.SizeInBytes = sizeof(CParticleVertex);
		m_d3dVertexBufferView.StrideInBytes = sizeof(CParticleVertex);
		m_bStart = false;
	}
	else {
		m_d3dVertexBufferView.BufferLocation = m_pd3dDrawBuffer->GetGPUVirtualAddress();
		m_d3dVertexBufferView.SizeInBytes = sizeof(CParticleVertex) * m_nVertices;
		m_d3dVertexBufferView.StrideInBytes = sizeof(CParticleVertex);
	}
	m_d3dStreamOutputBufferView.BufferLocation = m_pd3dStreamOutput->GetGPUVirtualAddress();
	m_d3dStreamOutputBufferView.SizeInBytes = sizeof(CParticleVertex) * m_nMaxParticle;
	m_d3dStreamOutputBufferView.BufferFilledSizeLocation = m_pd3dReadBackBuffer->GetGPUVirtualAddress();

	*m_pMappedPointer = 0;
	pd3dCommandList->CopyResource(m_pd3dReadBackBuffer.Get(), m_pd3dUploadBuffer.Get());

	D3D12_STREAM_OUTPUT_BUFFER_VIEW d3dSOBufferView[1] = { m_d3dStreamOutputBufferView };
	pd3dCommandList->SOSetTargets(0, 1, d3dSOBufferView);

	CMesh::Render(pd3dCommandList);	// One-Path

	//pd3dCommandList->DrawInstanced(m_nVertices, 1, 0, 0);
}

// ���������� �ٲٰ� ���
void CParticleMesh::TwoPathRender(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	pd3dCommandList->SOSetTargets(0, 1, NULL);
	CMesh::Render(pd3dCommandList);
}

// ReadBack ���۸� �д´�.
void CParticleMesh::PostRender()
{
	m_pd3dStreamOutput.Swap(m_pd3dDrawBuffer);

	UINT64* pFilledSize = nullptr;
	m_pd3dReadBackBuffer->Map(0, NULL, (void**)&pFilledSize);
	m_nVertices = UINT(*pFilledSize) / sizeof(CParticleVertex);
	m_pd3dReadBackBuffer->Unmap(0, NULL);

	if (m_nVertices == 0)
		m_bStart = true;
}