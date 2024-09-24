//==================================================
// Shader.hlsl
// ���̴� �ڵ���� ��Ƶ� ���̴� ����
//==================================================

// ����ü ���� =====================================

struct VS_TEXTURED_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PS_TEXTURED_OUTPUT
{
    float4 positionW : SV_POSITION;
    float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
};

// ���̴� ���� ���� ================================

// ��� ����
// 0�� ī�޶� ����
cbuffer CameraInfo : register(b0)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProj : packoffset(c4);
    float3 cameraEye : packoffset(c8);
}
// 1�� ���庯ȯ�� ��¦�� ���
cbuffer WorldInfo : register(b1)
{
    matrix gmtxWorld : packoffset(c0);
    float1 sh : packoffset(c4);
}
// 2�� ���� ���� ����
cbuffer DirectionLight : register(b2)
{
    float4 cDiffused;
    float4 cSpecular;
    float4 cAmbient;
    float3 LightDirction;
}

Texture2D gtxtTexture : register(t0);
sampler gStaticSampler : register(s0);

//==================================================

