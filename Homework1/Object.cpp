#include "Object.h"

CGameObject::CGameObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());

	::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dWorldBuffer, m_xmf4x4World);

	m_pd3dWorldBuffer->Map(0, NULL, (void**)&m_pMappedWorld);
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Pos)
{
	m_xmf4x4World._41 = xmf3Pos.x;
	m_xmf4x4World._42 = xmf3Pos.y;
	m_xmf4x4World._43 = xmf3Pos.z;
}