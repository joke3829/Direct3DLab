//=============================================
// Object.h
// GameObject를 포함한 여러가지를 정리한 헤더
//=============================================
#pragma once
#include "stdafx.h"
#include "Mesh.h"

struct Material {

};

class CGameObject {
public:
	CGameObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);

	void SetPosition(XMFLOAT3 xmf3Pos);

	template<class T>
	void SetMesh(std::shared_ptr<T>& pMesh)
	{
		m_pMesh = pMesh;
	}
	void SetMaterial(std::shared_ptr<Material>& pMaterial)
	{
		m_pMaterial = pMaterial;
	}
	
	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
protected:
	std::shared_ptr<CMesh> m_pMesh{ nullptr };
	std::shared_ptr<Material> m_pMaterial{ nullptr };

	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT4X4* m_pMappedWorld{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dWorldBuffer{ nullptr };


};