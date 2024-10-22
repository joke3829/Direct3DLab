#pragma once
#include "stdafx.h"
#include "Object.h"

struct CB_CAMERA_INFO {
	XMFLOAT4X4 xmf4x4View;
	XMFLOAT4X4 xmf4x4Proj;
	XMFLOAT3 xmf3Eye;
};

class CCamera {
public:
	CCamera(ComPtr<ID3D12Device>& pd3dDevice);
	void UpdateViewMatrix();
	virtual void UpdateProjMatrix(bool bOrtho = false);
	virtual void UpdateShaderVariables(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);

	void Rotate(int cxDelta, int cyDelta);
	void move();

	void SetCameraEye(XMFLOAT3 xmf3Eye);
	void SetViewportAndScissorRect(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);

	XMFLOAT3 getCameraEye() const;

	bool forward = false;
protected:
	bool m_bThirdPerson = false;
	CGameObject* m_pTarget{ nullptr };

	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

	ComPtr<ID3D12Resource> m_pd3dCameraBuffer;
	CB_CAMERA_INFO m_cameraInfo;
	CB_CAMERA_INFO* m_cInfoBegin;

	XMFLOAT3 m_xmf3Eye{ 0.0f, 0.0f, 0.0f };	// ����...?
	XMFLOAT3 m_xmf3At;
	XMFLOAT3 m_xmf3Up{ 0.0f, 1.0f, 0.0f };

	XMFLOAT3 m_xmf3Dir{ 0.0f, 0.0f ,1.0f }; // Eye�� �ش� ������ ���� At�� ���Ѵ�.
	XMFLOAT3 m_xmf3Offset{ 0.0f, 0.0f, 0.0f };	// 3��Ī ī�޶��� �� ���

	float m_fFOV = 60.0f;
	float m_fAspect = 1280.0f / 720.0f;
	float m_fNear = 1.0f;
	float m_fFar = 500.0f;


	// �� �������� �߰�����
};