//=============================================
// Object.h
// GameObject를 포함한 여러가지를 정리한 헤더
//=============================================
#pragma once
#include "stdafx.h"
#include "Mesh.h"
#include "Shader.h"
#include "DDSTextureLoader12.h"
#include "WICTextureLoader12.h"

// 텍스처는 하나가 아닌 여러개를 가질 수 있다.

// 하나만 가지는 텍스쳐 클래스를 만들어보자
class CSingleTexture {
public:
	//CSingleTexture() {}
	CSingleTexture(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, const wchar_t* pszFileName, bool bDDS);	// 파일 경로받아서 리소스를 만들도록 설계하자

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

	XMFLOAT3 getPosition() const;
	XMFLOAT3 getLook() const;

	XMFLOAT4X4 getWorldMatrix() const { return m_xmf4x4World; }

	BoundingOrientedBox& getOBB() { return m_OBB; }
	BoundingOrientedBox& getMeshOBB() { return m_pMesh->getOBB(); }

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

	void UpdateOBB();
	
	virtual void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::shared_ptr<CShader>& currentSetShader, bool bUseOwnShader = true);
protected:
	std::shared_ptr<CMesh> m_pMesh{ nullptr };
	std::shared_ptr<CShader> m_pShader{ nullptr };
	std::shared_ptr<CSingleTexture> m_pMaterial{ nullptr };

	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT4X4* m_pMappedWorld{ nullptr };
	ComPtr<ID3D12Resource> m_pd3dWorldBuffer{ nullptr };

	ComPtr<ID3D12DescriptorHeap> m_pd3dCbvSrvDescriptor;

	BoundingOrientedBox m_OBB = BoundingOrientedBox();
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

//=============================================================

class BulletObject : public CGameObject {
public:
	BulletObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);

	bool getExist() const { return m_bExist; }

	void SetPlayer(CGameObject* pPlayer);
	void Shoot();
	void Animate(float fElapsedTime);
	void setExist(bool exist) { m_bExist = exist; }

	void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::shared_ptr<CShader>& currentSetShader, bool bUseOwnShader = true);
protected:
	CGameObject* m_pPlayer;
	bool m_bExist;
	XMFLOAT3 m_xmf3StartPos;
	XMFLOAT3 m_xmf3Dir;
};

//==============================================================

class HGameObject : public CGameObject {
public:
	HGameObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::ifstream& inFile, std::unique_ptr<HGameObject>& pSibling, bool bSbiling = false);
	// 셰이더 추가 필요

	void setShader(std::shared_ptr<CShader>& pShader);

	void setPMatrix(XMFLOAT4X4& xmf4x4Parent);

	virtual void CreateDescriptorHeap(ComPtr<ID3D12Device>& pd3dDevice);
	virtual void CreateResourceView(ComPtr<ID3D12Device>& pd3dDevice);

	void UpdateWorldMatrix();
	void SetShaderVariables(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);

	bool collisionCheck(BoundingOrientedBox& obb, XMFLOAT4X4& xmf4x4look, bool forward = false);
	bool collisionCheck(CGameObject* obstacle);
	void AutoPilot(std::vector<std::unique_ptr<CGameObject>>& obstacles, float fElapsedTime);

	XMFLOAT3 getPosition() const;

	void move(eDirection dir, float fElapsedTime);

	void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::shared_ptr<CShader>& currentSetShader, bool bUseOwnShader = true);
protected:
	std::vector<std::unique_ptr<CSingleTexture>> m_vTextures;	// 텍스쳐 배열

	std::unique_ptr<HGameObject> m_pChild{ nullptr };
	std::unique_ptr<HGameObject> m_pSibling{ nullptr };

	XMFLOAT4X4 m_xmf4x4Parent;

	XMFLOAT3 m_xmf3Look{ 0.0, 0.0, 1.0 };
	XMFLOAT3 m_xmf3Up{ 0.0, 1.0, 0.0 };
	XMFLOAT3 m_xmf3Right{ 1.0, 0.0, 0.0 };
};


class CSmogObject : public CGameObject {
public:
	CSmogObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);


	template<class T>
	void SetDrawShader(std::shared_ptr<T>& pShader)
	{
		m_pDrawShader = pShader;
	}

	void Reset();

	void Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, std::shared_ptr<CShader>& currentSetShader);
	void PostRender(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);
protected:
	std::shared_ptr<CShader> m_pDrawShader;
};