//=============================================
// Object.h
// GameObject�� ������ ���������� ������ ���
//=============================================
#pragma once
#include "stdafx.h"
#include "Mesh.h"
#include "Shader.h"
#include "DDSTextureLoader12.h"
#include "WICTextureLoader12.h"

// �ؽ�ó�� �ϳ��� �ƴ� �������� ���� �� �ִ�.

// �ϳ��� ������ �ؽ��� Ŭ������ ������
class CSingleTexture {
public:
	//CSingleTexture() {}
	CSingleTexture(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, const wchar_t* pszFileName, bool bDDS);	// ���� ��ι޾Ƽ� ���ҽ��� ���鵵�� ��������

	const ComPtr<ID3D12Resource>& getTexture();

	void ReleaseUploadBuffer();
protected:
	ComPtr<ID3D12Resource> m_pd3dTexture{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dUploadBuffer{ nullptr };
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

	template<class T>
	void SetShader(std::shared_ptr<T>& pShader)
	{
		m_pShader = pShader;
	}
	void SetMaterial(std::shared_ptr<CSingleTexture>& pMaterial)
	{
		m_pMaterial = pMaterial;
	}
	virtual void CreateDescriptorHeap(ComPtr<ID3D12Device>& pd3dDevice);
	virtual void CreateResourceView(ComPtr<ID3D12Device>& pd3dDevice);
	virtual void SetShaderVariables(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
	
	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::shared_ptr<CShader>& currentSetShader);
protected:
	std::shared_ptr<CMesh> m_pMesh{ nullptr };
	std::shared_ptr<CShader> m_pShader{ nullptr };
	std::shared_ptr<CSingleTexture> m_pMaterial{ nullptr };

	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT4X4* m_pMappedWorld{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dWorldBuffer{ nullptr };

	ComPtr<ID3D12DescriptorHeap> m_pd3dCbvSrvDescriptor;
};

//==========================================================================================

class CTerrainObject : public CGameObject {
public:
	CTerrainObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
		: CGameObject(pd3dDevice, pd3dCommandList) {}

	void SetMaterial2(std::shared_ptr<CSingleTexture>& pMaterial)
	{
		m_pMaterial2 = pMaterial;
	}

	virtual void CreateDescriptorHeap(ComPtr<ID3D12Device>& pd3dDevice);
	virtual void CreateResourceView(ComPtr<ID3D12Device>& pd3dDevice);
	virtual void SetShaderVariables(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);

private:
	std::shared_ptr<CSingleTexture> m_pMaterial2{ nullptr };
};

//==========================================================================================

class CSkyBoxObject {
public:
	CSkyBoxObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, ComPtr<ID3D12RootSignature>& pd3dRootSignature);

	void UpdatePosition(XMFLOAT3 pos);

	void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::shared_ptr<CShader>& currentSetShader);
private:
	std::vector<std::unique_ptr<CGameObject>> m_vPlanes;
};