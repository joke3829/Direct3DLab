#include "GameObject.h"

CGameObject::CGameObject(ComPtr<ID3D12Device>& pd3dDevice)
{
	// 단위행렬로 초기화
	XMMATRIX xmtx = XMMatrixIdentity();
	XMStoreFloat4x4(&m_xmf4x4World, xmtx);

	::CreateUploadBuffer(pd3dDevice, m_xmf4WorldBuffer, m_xmf4x4World);
	m_pxmf4x4World = std::make_unique<XMFLOAT4X4>();

	m_xmf4WorldBuffer->Map(0, NULL, (void**)&m_pxmf4x4World);
}

//template<class T>
//void CGameObject::SetMesh(std::shared_ptr<T>& mesh)
//{
//	if (m_pMesh) {
//		m_pMesh.reset();
//	}
//	m_pMesh = mesh;
//}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x; m_xmf4x4World._42 = y; m_xmf4x4World._43 = z;
}

const XMFLOAT3 CGameObject::getPosition()
{
	return XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 루트 시그니처를 이용
	memcpy(m_pxmf4x4World.get(), &m_xmf4x4World, sizeof(XMFLOAT4X4));
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, m_xmf4WorldBuffer->GetGPUVirtualAddress());
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pMesh) {
		UpdateShaderVariables(pd3dCommandList);
		m_pMesh->Render();
	}
}