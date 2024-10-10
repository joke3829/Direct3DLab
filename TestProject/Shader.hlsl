
struct VS_INPUT 
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT 
{
    float4 positionW : SV_POSITION;
    float4 color : COLOR;
};

struct Material
{
    float4 cDiffused;
    float4 cSpecular;
    float4 cAmbient;
    float4 cEmissive;
};

struct DirectionLight
{
    float4 cDiffused;   
    float4 cSpecular;
    float4 cAmbient;
    float3 dirction;    // πÊ«‚
};
//===============================================
cbuffer cbWorldInfo : register(b0)
{
    matrix gmtxWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProj : packoffset(c4);
    float3 cameraEye : packoffset(c8);
};

cbuffer cbMaterialInfo : register(b2)
{
    Material gMaterial;
}

cbuffer cbDirectionLightInfo : register(b3)
{
    DirectionLight gDirLight;
}
cbuffer cbTest : register(b4)
{
    float4 tett;
}

//===================================================

VS_OUTPUT VSDiffused(VS_INPUT input)
{
    VS_OUTPUT output;
    output.positionW = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProj);
    output.color = input.color;
    return output;
}


float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
    return input.color;
}

float4 VSNoShader(uint nVertexID : SV_VertexID) : SV_POSITION
{
    float4 output;
    if (nVertexID == 0)
        output = float4(0.0, 0.5, 0.5, 1.0);
    else if(nVertexID == 1)
        output = float4(0.5, -0.5, 0.5, 1.0);
    else if(nVertexID == 2)
        output = float4(-0.5, -0.5, 0.5, 1.0);
    
    return output;
}

float4 PSNoShader(float4 input : SV_POSITION) : SV_TARGET
{
    return float4(1.0f, 1.0f, 0.0f, 1.0f);
}

//========================================================================

struct VS_LIGHT_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_LIGHT_OUTPUT
{
    float4 positionW : SV_POSITION;
    float3 normalW : NORMAL;
};

VS_LIGHT_OUTPUT VSLight(VS_LIGHT_INPUT input)
{
    VS_LIGHT_OUTPUT output;
    return output;
}

float4 PSLight(VS_LIGHT_OUTPUT input) : SV_TARGET
{
    float4 dd = { 1.0, 0.0, 0.0, 1.0 };
    return dd;
}