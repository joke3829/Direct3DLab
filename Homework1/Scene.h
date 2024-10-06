//=============================================
// Scene.h
// ������ ������ ���
// CScene�� ��ӹ޾� ���� ������� ������.
//=============================================
#pragma once
#include "stdafx.h"

class CScene {
public:
	virtual void BuildObject(ComPtr<ID3D12GraphicsCommandList>&pd3dCommandList) {};
	virtual void CreateRootSignature(ComPtr<ID3D12Device>& pd3dDevice);

	virtual void Render(ComPtr<ID3D12Device>& pd3dDevice) {};
protected:
	ComPtr<ID3D12RootSignature> m_pd3dRootSignature{ nullptr };
};

class CMenuScene : public CScene {
public:
	void Render(ComPtr<ID3D12Device>& pd3dDevice);
private:

};
