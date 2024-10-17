#include "Camera.h"

CCamera::CCamera(ComPtr<ID3D12Device>& pd3dDevice)
{
	m_d3dViewport.Height = 720;
	m_d3dViewport.Width = 1280;
	m_d3dViewport.MaxDepth = 1.0f;
	m_d3dViewport.MinDepth = 0.0f;
	m_d3dViewport.TopLeftX = 0;
	m_d3dViewport.TopLeftY = 0;
	
	m_d3dScissorRect = { 0, 0, 1280, 720 };

	::CreateBufferResource(pd3dDevice, m_pd3dCameraBuffer, m_cameraInfo);
	m_pd3dCameraBuffer->Map(0, NULL, (void**)&m_cInfoBegin);

	UpdateProjMatrix(true);
}

void CCamera::UpdateViewMatrix()
{
	if (m_bThirdPerson) {
		// 오브젝트의 포지션을 받아 오프셋을 더해 eye를 구한다.
	}
	XMStoreFloat3(&m_xmf3At, (XMLoadFloat3(&m_xmf3Eye) + XMLoadFloat3(&m_xmf3Dir)));
	XMStoreFloat4x4(&(m_cameraInfo.xmf4x4View), XMMatrixLookAtLH(XMLoadFloat3(&m_xmf3Eye), XMLoadFloat3(&m_xmf3At), XMLoadFloat3(&m_xmf3Up)));
	m_cInfoBegin->xmf3Eye = m_xmf3Eye;
	XMStoreFloat4x4(&(m_cInfoBegin->xmf4x4View), XMMatrixTranspose(XMLoadFloat4x4(&(m_cameraInfo.xmf4x4View))));
	//m_cInfoBegin->xmf4x4View = m_cameraInfo.xmf4x4View;
}

void CCamera::UpdateProjMatrix(bool bOrtho)
{
	if (bOrtho)
		XMStoreFloat4x4(&(m_cameraInfo.xmf4x4Proj), XMMatrixOrthographicLH(1280.0f, 720.0f, 1.0f, 500.0f));
	else
		XMStoreFloat4x4(&(m_cameraInfo.xmf4x4Proj), XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFOV), m_fAspect, m_fNear, m_fFar));
}

void CCamera::SetCameraEye(XMFLOAT3 xmf3Eye)
{
	m_xmf3Eye = xmf3Eye;
}

void CCamera::UpdateShaderVariables(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	UpdateViewMatrix();

	XMStoreFloat4x4(&(m_cInfoBegin->xmf4x4Proj), XMMatrixTranspose(XMLoadFloat4x4(&(m_cameraInfo.xmf4x4Proj))));
	//m_cInfoBegin->xmf4x4Proj = m_cameraInfo.xmf4x4Proj;
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, m_pd3dCameraBuffer->GetGPUVirtualAddress());
}

void CCamera::SetViewportAndScissorRect(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}