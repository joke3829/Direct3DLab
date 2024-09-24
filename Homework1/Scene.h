//=============================================
// Scene.h
// ������ ������ ���
// CScene�� ��ӹ޾� ���� ������� ������.
//=============================================
#pragma once
#include "stdafx.h"

class CScene {
public:
	virtual void CreateRootSignature(ComPtr<ID3D12Device>& pd3dDevice);

	virtual void Render(ComPtr<ID3D12Device>& pd3dDevice);
protected:
	ComPtr<ID3D12RootSignature> m_pd3dRootSignature{ nullptr };
};

