#pragma once
#include "stdafx.h"

struct VIEW_PROJ_MATRIX {
	XMFLOAT4X4 xmf4x4View;
	XMFLOAT4X4 xmf4x4Proj;
};

class CCamera {
public:
	CCamera(ComPtr<ID3D12Device>& pd3dDevice);
	
	void SetViewportAndScissorRect(ID3D12GraphicsCommandList* pd3dCommandList);
	void SetCameraEye(XMFLOAT3 xmf3eye);

	XMFLOAT3 getCameraEye() const { return m_xmf3Eye; }
	
	void UpdateViewMatrix();
	void UpdateProjMatrix();
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
private:
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

	ComPtr<ID3D12Resource> m_pd3dViewProj{ nullptr };
	std::unique_ptr<VIEW_PROJ_MATRIX> m_vpMatrix;

	XMFLOAT4X4 m_xmf4x4ViewMatrix;
	XMFLOAT4X4 m_xmf4x4ProjMatrix;

	XMFLOAT3 m_xmf3Eye{ XMFLOAT3(0.0f, 0.0f, 0.0f) };
	XMFLOAT3 m_xmf3At{ XMFLOAT3(0.0f, 0.0f, 0.0f) };
	XMFLOAT3 m_xmf3Up{ XMFLOAT3(0.0f, 1.0f, 0.0f) };
	
	XMFLOAT3 m_xmf3dir{ XMFLOAT3(0.0, 0.0, 1.0) };

	float m_fFOV;		// FOV각
	float m_fAspect;	// 화면 비
	float m_fNear;		// 가까운
	float m_fFar;		// 먼
};

