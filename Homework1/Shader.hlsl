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

struct VS_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float4 positionW : POSITION;
    float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
};
//====================================================
struct VS_NLightTextred_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_NLightTextured_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};
//====================================================
struct VS_Diffused_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_Diffused_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
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
}
// 2번 전역 조명 정보
cbuffer DirectionLight : register(b2)
{
    float4 cDiffused;
    float4 cSpecular;
    float4 cAmbient;
    float3 LightDirction;
}
// 3번 반짝임 계수
cbuffer shInfo : register(b3)
{
    float1 sh;
}
// 지형 월드
cbuffer TerrainInfo : register(b4)
{
    matrix gmtxTerrainWorld : packoffset(c0);
}

Texture2D gtxtTexture : register(t0);
Texture2D gTerrainBaseTexture : register(t1);
Texture2D gTerrainDetailTexture : register(t2);
sampler gStaticSampler : register(s0);

//==================================================

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProj);
    output.positionW = mul(float4(input.position, 1.0f), gmtxWorld);
    output.normalW = mul(input.normal, (float3x3) gmtxWorld);
    output.uv = input.uv;
    return output;
}

float4 PSTextured(VS_TEXTURED_OUTPUT input)
{
    return gtxtTexture.Sample(gStaticSampler, input.uv);
}



VS_NLightTextured_OUTPUT VSNoLightTextured(VS_NLightTextred_INPUT input)
{
    VS_NLightTextured_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProj);
    output.uv = input.uv;
    return output;
}

float4 PSNoLightTextured(VS_NLightTextured_OUTPUT input) : SV_TARGET
{
    return gtxtTexture.Sample(gStaticSampler, input.uv);
}


VS_Diffused_OUTPUT VSDiffused(VS_Diffused_INPUT input)
{
    VS_Diffused_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProj);
    output.color = input.color;
    return output;
}

float4 PSDiffused(VS_Diffused_OUTPUT input) : SV_TARGET
{
    return input.color;
}

//====================================

struct VS_TERRAIN_INPUT
{
    float3 position : POSITION;
    float2 uv1 : TEXCOORD0;
    float2 uv2 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv1 : TEXCOORD0;
    float2 uv2 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
    VS_TERRAIN_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxTerrainWorld), gmtxView), gmtxProj);
    //output.position = mul(float4(input.position, 1.0f), gmtxView);
    output.uv1 = input.uv1;
    output.uv2 = input.uv2;
    return output;
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
    float4 baseColor = gTerrainBaseTexture.Sample(gStaticSampler, input.uv1);
    float4 detailColor = gTerrainDetailTexture.Sample(gStaticSampler, input.uv2);
    
    return saturate((baseColor * 0.7) + (detailColor * 0.3));

}