#include "Camera.h"
CCamera::CCamera(ComPtr<ID3D12Device>& pd3dDevice)
{
	m_d3dViewport.Height = 720;
	m_d3dViewport.MaxDepth = 1.0f;
	m_d3dViewport.MinDepth = 0.0f;
	m_d3dViewport.TopLeftX = 0;
	m_d3dViewport.TopLeftY = 0;
	m_d3dViewport.Width = 1280;

	m_d3dScissorRect = { 0, 0, 1280, 720 };

	m_vpMatrix = std::make_unique<VIEW_PROJ_MATRIX>();

	m_fFOV = 90.0f;
	m_fAspect = 1080.0f / 720.0f;
	m_fNear = 1.0f;
	m_fFar = 500.0f;
}

void CCamera::SetViewportAndScissorRect(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

void CCamera::SetCameraEye(XMFLOAT3 eye)
{
	m_xmf3Eye = eye;
}

void CCamera::UpdateViewMatrix()
{
	XMStoreFloat4x4(&m_xmf4x4ViewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&m_xmf3Eye), XMLoadFloat3(&m_xmf3At), XMLoadFloat3(&m_xmf3Up)));
}

void CCamera::UpdateProjMatrix()
{
	XMStoreFloat4x4(&m_xmf4x4ProjMatrix, XMMatrixPerspectiveFovLH(m_fFOV, m_fAspect, m_fNear, m_fFar));
}