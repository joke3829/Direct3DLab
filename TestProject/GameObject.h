#pragma once
#include "stdafx.h"
#include "Mesh.h"

struct Material {
	XMFLOAT4 cDiffused;
	XMFLOAT4 cSpecular;
	XMFLOAT4 cAmbient;
	XMFLOAT4 cEmissive;

};

class CGameObject {
public:
	CGameObject(ComPtr<ID3D12Device>& pd3dDevice);

	template<class T>
	void SetMesh(std::shared_ptr<T>& mesh)
	{
		if (m_pMesh) {
			m_pMesh.reset();
		}
		m_pMesh = mesh;
	};

	void SetPosition(float x, float y, float z);

	const XMFLOAT3 getPosition();

	virtual void CreateDescriptorHeap(ComPtr<ID3D12Device>& pd3dDevice) {};
	virtual void CreateCBV(ComPtr<ID3D12Device>& pd3dDevice) {};

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
protected:
	ComPtr<ID3D12Resource> m_xmf4WorldBuffer;
	XMFLOAT4X4 m_xmf4x4World;
	std::unique_ptr<XMFLOAT4X4> m_pxmf4x4World;

	XMFLOAT3 m_xmf3RotationAngle{XMFLOAT3(0.0f, 0.0f, 0.0f)};

	std::shared_ptr<CMesh> m_pMesh{ nullptr };
};

class CWallObject : public CGameObject {
public:
	CWallObject(ComPtr<ID3D12Device>& pd3dDevice) : CGameObject(pd3dDevice) {}
};

class CLightObject : public CGameObject {
public:
	CLightObject(ComPtr<ID3D12Device>& pd3dDevice);

	void CreateDescriptorHeap(ComPtr<ID3D12Device>& pd3dDevice);
	void CreateCBV(ComPtr<ID3D12Device>& pd3dDevice);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
protected:
	Material m_material;
	std::unique_ptr<Material> m_pMappedMaterial;

	ComPtr<ID3D12DescriptorHeap> m_pd3dWMDescriptorHeap;	// 월드, 마테리얼 서술자 힙
	UINT m_nCBVIncrementSize;	// 상수버퍼 증가 폭
};