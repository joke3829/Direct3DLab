//========================================
// Scene.cpp
// 장면들을 정의한 cpp
//========================================
#include "Scene.h"

void CScene::CreateRootSignature(ComPtr<ID3D12Device>& pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE d3dDescriptorRange[2];
	d3dDescriptorRange[0].NumDescriptors = 1;
	d3dDescriptorRange[0].BaseShaderRegister = 1;
	d3dDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRange[0].RegisterSpace = 0;
	d3dDescriptorRange[0].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[1].NumDescriptors = 1;
	d3dDescriptorRange[1].BaseShaderRegister = 0;
	d3dDescriptorRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[1].RegisterSpace = 0;
	d3dDescriptorRange[1].OffsetInDescriptorsFromTableStart = 1;

	// 0. 카메라 정보, 1. 월드, 텍스처 테이블
	D3D12_ROOT_PARAMETER d3dRootParameter[2];
	d3dRootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	d3dRootParameter[0].Descriptor.RegisterSpace = 0;
	d3dRootParameter[0].Descriptor.ShaderRegister = 0;
	d3dRootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	d3dRootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	d3dRootParameter[1].DescriptorTable.NumDescriptorRanges = 2;
	d3dRootParameter[1].DescriptorTable.pDescriptorRanges = d3dDescriptorRange;
	d3dRootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	/*D3D12_ROOT_PARAMETER d3dRootParameter[2];
	d3dRootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	d3dRootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	d3dRootParameter[0].Descriptor.RegisterSpace = 0;
	d3dRootParameter[0].Descriptor.ShaderRegister = 0;

	d3dRootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	d3dRootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	d3dRootParameter[1].Descriptor.RegisterSpace = 0;
	d3dRootParameter[1].Descriptor.ShaderRegister = 1;*/
	
	D3D12_STATIC_SAMPLER_DESC d3dStaticSamplerDesc;
	::ZeroMemory(&d3dStaticSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dStaticSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dStaticSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dStaticSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dStaticSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dStaticSamplerDesc.MipLODBias = 0.0f;
	d3dStaticSamplerDesc.MaxAnisotropy = 1;
	d3dStaticSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dStaticSamplerDesc.MinLOD = -FLT_MAX;
	d3dStaticSamplerDesc.MaxLOD = FLT_MAX;
	d3dStaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	d3dStaticSamplerDesc.RegisterSpace = 0;
	d3dStaticSamplerDesc.ShaderRegister = 0;
	d3dStaticSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	d3dRootSignatureDesc.NumParameters = 2;
	d3dRootSignatureDesc.NumStaticSamplers = 1;
	d3dRootSignatureDesc.pParameters = d3dRootParameter;
	d3dRootSignatureDesc.pStaticSamplers = &d3dStaticSamplerDesc;

	ID3DBlob* pd3dBlob{ nullptr };
	ID3DBlob* pd3dErrorBlob{ nullptr };
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pd3dBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dBlob->GetBufferPointer(), pd3dBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)m_pd3dRootSignature.GetAddressOf());

	if (pd3dBlob)
		pd3dBlob->Release();
	if (pd3dErrorBlob)
		pd3dErrorBlob->Release();
}

void CScene::SetCamera(std::shared_ptr<CCamera>& pCamera)
{
	m_pCamera = pCamera;
}

//========================================================================

void CMenuScene::BuildObject(ComPtr<ID3D12Device>& pd3dDevice, ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	CreateRootSignature(pd3dDevice);
	m_vShaders.reserve(1);
	m_vShaders.push_back(std::make_unique<CMenuShader>());
	
	for (std::unique_ptr<CShader>& temp : m_vShaders) {
		temp->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
		temp->BuildObject(pd3dDevice, pd3dCommandList);
	}
	/*m_vShaders[0]->CreatePipelineState(pd3dDevice, m_pd3dRootSignature);
	m_vShaders[0]->BuildObject(pd3dDevice, pd3dCommandList);*/
}

void CMenuScene::Render(ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature.Get());
	m_pCamera->SetViewportAndScissorRect(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);
	for (std::unique_ptr<CShader>& temp : m_vShaders) {
		temp->SetPipelineState(pd3dCommandList);
		temp->Render(pd3dCommandList);
	}
}