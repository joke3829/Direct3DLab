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
		XMFLOAT3 targetPos = m_pTarget->getPosition();
		XMStoreFloat3(&m_xmf3Eye, XMLoadFloat3(&targetPos) + (XMLoadFloat3(&m_xmf3Offset) * 30.0));
		//XMStoreFloat3(&m_xmf3At, XMVector3Normalize(XMLoadFloat3(&targetPos) - XMLoadFloat3(&m_xmf3Eye)));
		m_xmf3At = targetPos;
	}
	else {
		XMStoreFloat3(&m_xmf3At, (XMLoadFloat3(&m_xmf3Eye) + XMLoadFloat3(&m_xmf3Dir)));
	}
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

void CCamera::SetTarget(HGameObject* target)
{
	if (target) {
		m_pTarget = target;
		m_bThirdPerson = true;
	}
}

void CCamera::SetThirdPerson(bool bThirdPerson)
{
	m_bThirdPerson = bThirdPerson;
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

void CCamera::Rotate(int cxDelta, int cyDelta)
{
	float cx = (float)cxDelta / 3.0f;
	float cy = (float)cyDelta / 3.0f;

	if(m_bThirdPerson){
		XMMATRIX mtxrotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(cx));
		XMStoreFloat3(&m_xmf3Offset, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Offset), mtxrotate));

		XMVECTOR xmvRight = XMVector3Cross(XMLoadFloat3(&m_xmf3Up), XMLoadFloat3(&m_xmf3Offset));
		limitcy += cy;
		if (limitcy > 50.0f) {
			cy -= (limitcy - 50.0f);
			limitcy = 50.0f;
		}
		if (limitcy < -50.0f) {
			cy += -(limitcy + 50.0f);
			limitcy = -50.0f;
		}
		mtxrotate = XMMatrixRotationAxis(xmvRight, XMConvertToRadians(cy));
		XMStoreFloat3(&m_xmf3Offset, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Offset), mtxrotate));
	}
	else {
		XMMATRIX mtxrotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(cx));
		XMStoreFloat3(&m_xmf3Dir, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Dir), mtxrotate));

		XMVECTOR xmvRight = XMVector3Cross(XMLoadFloat3(&m_xmf3Up), XMLoadFloat3(&m_xmf3Dir));
		mtxrotate = XMMatrixRotationAxis(xmvRight, XMConvertToRadians(cy));
		XMStoreFloat3(&m_xmf3Dir, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Dir), mtxrotate));
	}
}

void CCamera::move()
{
	//if (forward)
		XMStoreFloat3(&m_xmf3Eye, XMLoadFloat3(&m_xmf3Eye) + (XMLoadFloat3(&m_xmf3Dir)));
}

XMFLOAT3 CCamera::getCameraEye() const
{
	return m_xmf3Eye;
}