//==================================================
// Shader.hlsl
// 셰이더 코드들을 모아둔 셰이더 파일
//==================================================

// 구조체 모음 =====================================

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

// 셰이더 변수 모음 ================================

// 상수 버퍼
// 0번 카메라 정보
cbuffer CameraInfo : register(b0)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProj : packoffset(c4);
    float3 cameraEye : packoffset(c8);
}
// 1번 월드변환과 반짝임 계수
cbuffer WorldInfo : register(b1)
{
    matrix gmtxWorld : packoffset(c0);
    float1 sh : packoffset(c4);
}
// 2번 전역 조명 정보
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

