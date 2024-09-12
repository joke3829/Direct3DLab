#pragma once
#include "stdafx.h"
#include "Mesh.h"


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

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
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